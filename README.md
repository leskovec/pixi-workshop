# Pixi workshop: reproducible environments and containers

Material for the pixi workshop.

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
|---|---|---|
|0. Welcome and setup check | [SETUP.md](SETUP.md) | |
|1. Lecture: environment management on HPC | | |
|2. First workspace | [exercises/02-first-workspace.md](exercises/02-first-workspace.md) | |
|3. Lecture: tasks, features, environments | | |
|4. The example becomes a project | [exercises/04-project.md](exercises/04-project.md) | `solutions/block-4-project/` |
|Break | | |
|5. Pixi on HPC, `--locked` vs `--frozen` | [exercises/05-hpc.md](exercises/05-hpc.md) | `hpc/` |
|6. MPI: bundled vs system | [exercises/06-mpi.md](exercises/06-mpi.md) | `solutions/block-6-mpi/` |
|7. Packaging the solver with pixi-build | [exercises/07-pixi-build.md](exercises/07-pixi-build.md) | `solutions/block-7-pixi-build/` |
|8. pixi-pack: can it replace a container? | [exercises/08-pixi-pack.md](exercises/08-pixi-pack.md) | |
|9. For fun, wrap-up | [exercises/09-fun.md](exercises/09-fun.md) | |


