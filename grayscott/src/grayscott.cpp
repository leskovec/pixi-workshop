// Gray-Scott reaction-diffusion on a periodic N x N grid, parallelised with MPI.
//
//   du/dt = Du * lap(u) - u v^2 + F (1 - u)
//   dv/dt = Dv * lap(v) + u v^2 - (F + k) v
//
// The grid is split into horizontal strips, one per MPI rank. Each step the
// ranks exchange one halo row with their neighbours above and below. At the
// end rank 0 gathers v and writes it as a NumPy .npy file.
//
// The initial noise depends only on the global cell index and every cell is
// updated with the same operations, so the output is bitwise identical for
// any number of ranks.

#include <mpi.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include <sys/stat.h>

#ifndef GRAYSCOTT_VERSION
#define GRAYSCOTT_VERSION "unknown"
#endif

namespace {

struct Params {
    int size = 256;
    int steps = 10000;
    double Du = 0.16;
    double Dv = 0.08;
    double F = 0.035;
    double k = 0.065;
    double dt = 1.0;
    std::string output = "output/v.npy";
};

void usage(const char* prog) {
    std::printf(
        "Usage: %s [options]\n"
        "  --size N       grid size N x N (default 256)\n"
        "  --steps S      number of time steps (default 10000)\n"
        "  --F value      feed rate (default 0.035)\n"
        "  --k value      kill rate (default 0.065)\n"
        "  --output FILE  output .npy file (default output/v.npy)\n"
        "  --version      print version and MPI library\n",
        prog);
}

bool parse(int argc, char** argv, Params& p, int rank) {
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() -> const char* {
            if (i + 1 >= argc) {
                if (rank == 0) std::fprintf(stderr, "missing value for %s\n", a.c_str());
                std::exit(1);
            }
            return argv[++i];
        };
        if (a == "--size") p.size = std::atoi(next());
        else if (a == "--steps") p.steps = std::atoi(next());
        else if (a == "--F") p.F = std::atof(next());
        else if (a == "--k") p.k = std::atof(next());
        else if (a == "--output") p.output = next();
        else if (a == "--version") {
            if (rank == 0) {
                char lib[MPI_MAX_LIBRARY_VERSION_STRING];
                int len = 0;
                MPI_Get_library_version(lib, &len);
                std::printf("grayscott %s\n%s\n", GRAYSCOTT_VERSION, lib);
            }
            return false;
        } else {
            if (rank == 0) usage(argv[0]);
            return false;
        }
    }
    return true;
}

// Deterministic noise in [0, 1) from the global cell index (splitmix64).
double noise(std::uint64_t i) {
    std::uint64_t z = i + 0x9e3779b97f4a7c15ULL;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    z = z ^ (z >> 31);
    return static_cast<double>(z >> 11) / 9007199254740992.0;
}

// Minimal writer for a 2D float64 array in NumPy .npy format (version 1.0).
void write_npy(const std::string& path, const std::vector<double>& data, int rows, int cols) {
    std::string header = "{'descr': '<f8', 'fortran_order': False, 'shape': (" +
                         std::to_string(rows) + ", " + std::to_string(cols) + "), }";
    // magic (6) + version (2) + header length (2) + header must be a multiple of 64
    std::size_t total = 10 + header.size() + 1;
    header.append((64 - total % 64) % 64, ' ');
    header.push_back('\n');

    // create the parent directory (one level) if needed
    const auto slash = path.find_last_of('/');
    if (slash != std::string::npos && slash > 0) mkdir(path.substr(0, slash).c_str(), 0755);
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        std::fprintf(stderr, "cannot open %s for writing\n", path.c_str());
        return;
    }
    const char magic[] = "\x93NUMPY\x01\x00";
    out.write(magic, 8);
    std::uint16_t hlen = static_cast<std::uint16_t>(header.size());
    out.put(static_cast<char>(hlen & 0xff));
    out.put(static_cast<char>(hlen >> 8));
    out.write(header.data(), static_cast<std::streamsize>(header.size()));
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size() * sizeof(double)));
}

}  // namespace

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank = 0, nranks = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    Params p;
    if (!parse(argc, argv, p, rank)) {
        MPI_Finalize();
        return 0;
    }
    const int N = p.size;
    if (N < nranks) {
        if (rank == 0) std::fprintf(stderr, "grid size %d is smaller than the number of ranks %d\n", N, nranks);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Rows owned by each rank; the first N % nranks ranks get one extra row.
    std::vector<int> counts(nranks), displs(nranks);
    for (int r = 0, offset = 0; r < nranks; ++r) {
        int rows = N / nranks + (r < N % nranks ? 1 : 0);
        counts[r] = rows * N;
        displs[r] = offset * N;
        offset += rows;
    }
    const int nrows = counts[rank] / N;
    const int row0 = displs[rank] / N;
    const int up = (rank - 1 + nranks) % nranks;
    const int down = (rank + 1) % nranks;

    // Local arrays with one halo row above (row 0) and below (row nrows + 1).
    const std::size_t cells = static_cast<std::size_t>(nrows + 2) * N;
    std::vector<double> u(cells, 1.0), v(cells, 0.0), u2(cells), v2(cells);
    auto at = [N](int i, int j) { return static_cast<std::size_t>(i) * N + j; };

    // Seed a square in the middle of the domain, with a little noise everywhere.
    for (int i = 1; i <= nrows; ++i) {
        const int gi = row0 + i - 1;
        for (int j = 0; j < N; ++j) {
            const double r = noise(static_cast<std::uint64_t>(gi) * N + j);
            const bool centre = gi > 2 * N / 5 && gi < 3 * N / 5 && j > 2 * N / 5 && j < 3 * N / 5;
            u[at(i, j)] = centre ? 0.50 + 0.02 * r : 1.0 - 0.02 * r;
            v[at(i, j)] = centre ? 0.25 + 0.02 * r : 0.02 * r;
        }
    }

    auto exchange = [&](std::vector<double>& a) {
        // first owned row goes up, bottom halo comes from below
        MPI_Sendrecv(&a[at(1, 0)], N, MPI_DOUBLE, up, 0,
                     &a[at(nrows + 1, 0)], N, MPI_DOUBLE, down, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // last owned row goes down, top halo comes from above
        MPI_Sendrecv(&a[at(nrows, 0)], N, MPI_DOUBLE, down, 1,
                     &a[at(0, 0)], N, MPI_DOUBLE, up, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    };

    MPI_Barrier(MPI_COMM_WORLD);
    const double t0 = MPI_Wtime();

    for (int step = 0; step < p.steps; ++step) {
        exchange(u);
        exchange(v);
        for (int i = 1; i <= nrows; ++i) {
            for (int j = 0; j < N; ++j) {
                const int jl = (j == 0) ? N - 1 : j - 1;
                const int jr = (j == N - 1) ? 0 : j + 1;
                const double uc = u[at(i, j)], vc = v[at(i, j)];
                const double lap_u = u[at(i - 1, j)] + u[at(i + 1, j)] + u[at(i, jl)] + u[at(i, jr)] - 4.0 * uc;
                const double lap_v = v[at(i - 1, j)] + v[at(i + 1, j)] + v[at(i, jl)] + v[at(i, jr)] - 4.0 * vc;
                const double uvv = uc * vc * vc;
                u2[at(i, j)] = uc + p.dt * (p.Du * lap_u - uvv + p.F * (1.0 - uc));
                v2[at(i, j)] = vc + p.dt * (p.Dv * lap_v + uvv - (p.F + p.k) * vc);
            }
        }
        std::swap(u, u2);
        std::swap(v, v2);
    }

    const double elapsed = MPI_Wtime() - t0;

    std::vector<double> field;
    if (rank == 0) field.resize(static_cast<std::size_t>(N) * N);
    MPI_Gatherv(&v[at(1, 0)], counts[rank], MPI_DOUBLE,
                field.data(), counts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        write_npy(p.output, field, N, N);
        const double updates = static_cast<double>(N) * N * p.steps;
        std::printf("grayscott %s: %d x %d grid, %d steps, %d ranks, %.2f s, %.1f Mcell-updates/s\n",
                    GRAYSCOTT_VERSION, N, N, p.steps, nranks, elapsed, updates / elapsed / 1e6);
        std::printf("wrote %s\n", p.output.c_str());
    }

    MPI_Finalize();
    return 0;
}
