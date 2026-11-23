# Block 6: MPI (15:35–16:05)

Work in `grayscott/`. Solution: `solutions/block-6-mpi/pixi.toml`.

## Two ways to get MPI

| Route | In `pixi.toml` | Pros | Cons |
|---|---|---|---|
| bundled | `openmpi = ">=5.0,<6"` from conda-forge | identical on laptop and cluster, fully locked; the Linux build includes UCX for InfiniBand | launcher integration with Slurm and the fastest network settings need testing on each cluster |
| system | `openmpi = { version = "5.0.*", build = "external_*" }` from the `conda-forge/label/mpi-external` channel, plus `module load` | uses the MPI the centre tuned for its network | environment no longer self-contained; version must match the module; Linux only |

The `external_*` package is a placeholder: it contains no MPI library, it only tells the solver "MPI is provided". Everything that depends on MPI (mpi4py, h5py with MPI, ...) is then built against conda-forge's Open MPI headers and runs with the system library.

For an MPICH-compatible system MPI (MPICH, Intel MPI, Cray MPICH) the same works with `mpich = { version = "4.*", build = "external_*" }`.

## 1. MPI only where it is needed

In the block 4 manifest, `openmpi` is in `[dependencies]`, so even the plotting environment has it. Move it into a feature:

```toml
[feature.mpi-bundled.dependencies]
openmpi = ">=5.0,<6"
```

and add `mpi-bundled` to the `default` and `prod` environments, not to `analysis`.

```bash
pixi list -e analysis | grep -i mpi     # nothing
```

## 2. mpi4py

```toml
[feature.mpi-python.dependencies]
python = "3.13.*"
mpi4py = ">=4"

[feature.mpi-python.tasks]
hello = { cmd = "mpirun --oversubscribe -n {{ ranks }} python scripts/hello_mpi.py", args = [{ arg = "ranks", default = "4" }] }
```

```bash
pixi run hello
pixi run hello 8
```

## 3. The solver with more ranks

```bash
pixi run run 1 1024 2000
pixi run run 4 1024 2000
pixi run test               # 1 rank vs 4 ranks, bitwise identical
pixi run build/grayscott --version     # which MPI library is it linked to?
```

## 4. A cluster environment with the system MPI

```toml
[feature.mpi-system]
platforms = ["linux-64", "linux-aarch64"]
channels = [{ channel = "conda-forge/label/mpi-external", priority = 1 }]

[feature.mpi-system.dependencies]
openmpi = { version = "5.0.*", build = "external_*" }

[environments]
cluster = { features = ["mpi-system", "build", "mpi-python", "test"] }
```

```bash
pixi lock
pixi list -e cluster --platform linux-64 | grep -E "openmpi|mpi4py"
```

Note that `cluster` has no `solve-group`: it must be solved separately because its `openmpi` differs from the one in `default`.

## 5. On the cluster (demo)

```bash
module load OpenMPI/5.0.x                  # adapt
pixi install --locked -e cluster
pixi run -e cluster --frozen build
sbatch ../hpc/grayscott-system.sbatch
sbatch ../hpc/grayscott-bundled.sbatch     # compare the timings
```
