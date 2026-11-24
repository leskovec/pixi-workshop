# Pixi workshop: reproducible environments and containers

Material for the pixi workshop.

The whole workshop follows one example: a small Gray-Scott reaction-diffusion solver in C++, parallelised with MPI and built with CMake, plus Python scripts that plot its output. We manage it as a pixi project, run it with MPI on a laptop and a cluster, package it with pixi-build, and ship it to machines without pixi using pixi-pack.

Before the workshop: follow [SETUP.md](SETUP.md).

## How to follow

Everything you type is on the slides, one slide per step, numbered by block: step 4.3 is the third step of block 4. Fallen behind? Every step that changes `pixi.toml` has its result in `solutions/`: copy `solutions/4.3/pixi.toml` into `grayscott/` and continue with 4.4.

| Block | Topic | Solutions |
|---|---|---|
| 0 | Welcome, setup check | |
| 1 | Environment management on HPC (lecture) | |
| 2 | First workspace | |
| 3 | Organising a project (lecture) | |
| 4 | The example becomes a project | 4.1, 4.3, 4.4, 4.5 |
| 5 | Pixi on a cluster | uses 7.2 |
| 6 | MPI: bundled vs system | 6.1, 6.2, 6.4 |
| 7 | Packaging with pixi-build | 7.1, 7.2, 7.4 |
| 8 | Shipping with pixi-pack | |
| 9 | For fun | |

## Layout

```
grayscott/        the example code; you work here from block 4 on (no pixi.toml yet)
  src/              the MPI solver in C++
  CMakeLists.txt
  scripts/          plot.py, compare.py, hello_mpi.py
  environment.yml   a colleague's conda environment (step 4.7 imports it)
analysis/         a second project that uses the packaged solver (step 7.4)
solutions/        pixi.toml after each step: 4.1/, 4.3/, ... 7.4/
setup/            the setup check, run before the workshop (SETUP.md)
slides/           the slides (Slidev); to view: cd slides && pixi run dev
```
