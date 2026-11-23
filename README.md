# Pixi workshop: reproducible environments and containers

Material for the EuroCC Slovenia pixi workshop, 25 November 2026, 13:00–17:00 (online).

The whole workshop follows one example: a small Gray-Scott reaction-diffusion solver in C++, parallelised with MPI and built with CMake, plus Python scripts that plot its output. We manage it as a pixi project, run it with MPI on a laptop and a cluster, package it with pixi-build, and ship it to machines without pixi using pixi-pack.

Before the workshop: follow [SETUP.md](SETUP.md).

## Layout

```
grayscott/            the example code; you work here (no pixi.toml yet)
  src/grayscott.cpp     MPI solver, writes output/v.npy
  CMakeLists.txt
  scripts/              plot.py, compare.py, hello_mpi.py
  environment.yml       a colleague's conda environment (block 4 starts from it)
analysis/scan.py      a second project that uses the packaged solver (block 7)
exercises/            step-by-step instructions per block
solutions/            pixi.toml files to copy in when you fall behind
hpc/                  cluster settings, Slurm scripts, --locked/--frozen demo
setup/                setup check, run before the workshop
slides/               the lecture slides (Slidev), pixi run dev to view
```

## Programme

| Time | Block | Instructions | Solution |
|---|---|---|---|
| 13:00 | 0. Welcome and setup check | [SETUP.md](SETUP.md) | |
| 13:10 | 1. Lecture: environment management on HPC | | |
| 13:30 | 2. First workspace | [exercises/02-first-workspace.md](exercises/02-first-workspace.md) | |
| 14:05 | 3. Lecture: tasks, features, environments | | |
| 14:15 | 4. The example becomes a project | [exercises/04-project.md](exercises/04-project.md) | `solutions/block-4-project/` |
| 15:00 | Break | | |
| 15:15 | 5. Pixi on HPC, `--locked` vs `--frozen` | [exercises/05-hpc.md](exercises/05-hpc.md) | `hpc/` |
| 15:35 | 6. MPI: bundled vs system | [exercises/06-mpi.md](exercises/06-mpi.md) | `solutions/block-6-mpi/` |
| 16:05 | 7. Packaging the solver with pixi-build | [exercises/07-pixi-build.md](exercises/07-pixi-build.md) | `solutions/block-7-pixi-build/` |
| 16:35 | 8. pixi-pack: can it replace a container? | [exercises/08-pixi-pack.md](exercises/08-pixi-pack.md) | |
| 16:50 | 9. For fun, wrap-up | [exercises/09-fun.md](exercises/09-fun.md) | |

## Catching up

Each solution is a complete `pixi.toml` for `grayscott/`:

```bash
cd grayscott
cp ../solutions/block-6-mpi/pixi.toml .
pixi install
```

## The solver

```bash
grayscott --size 256 --steps 10000 --F 0.035 --k 0.065 --output output/v.npy
grayscott --version          # also prints the MPI library in use
```

The grid is split into horizontal strips, one per MPI rank, with a halo exchange every step. The output does not depend on the number of ranks (`pixi run test` checks that 1 and 4 ranks agree bitwise).

## Tested with

pixi 0.84.2 and pixi-pack 0.7.12 on macOS arm64, with Open MPI 5.0.11 from conda-forge:

- the block 4, 6 and 7 manifests solve for linux-64, linux-aarch64, osx-arm64 and osx-64; build, `test`, `hello`, `plot`, `simulate` and the analysis `scan` run on osx-arm64
- `pixi build`, `pixi-pack` and `pixi-unpack` (tar and self-extracting installer) on osx-arm64
- `hpc/locked-vs-frozen.sh`, detached environments, conda import and export

Not tested yet: anything on Linux, the Slurm scripts, and the block 9 demos except `samtools`.
