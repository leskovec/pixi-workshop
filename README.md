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

