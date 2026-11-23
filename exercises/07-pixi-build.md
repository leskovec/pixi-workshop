# Block 7: packaging the solver with pixi-build (16:05–16:35)

Solutions: `solutions/block-7-pixi-build/grayscott/pixi.toml` and `solutions/block-7-pixi-build/analysis/pixi.toml`.

Until now, every user had to compile the solver in a development environment. Once it is a conda package, it installs like numpy: no compilers, no `build/` directory, a fixed version.

## 1. Turn the workspace into a package

pixi-build is still a preview feature (pixi 0.84):

```bash
cd grayscott
pixi workspace preview add pixi-build
```

Add to `pixi.toml`:

```toml
[package]
name = "grayscott"
version = "0.1.0"

[package.build]
backend = { name = "pixi-build-cmake", version = "*" }

[package.host-dependencies]
openmpi = ">=5.0,<6"
```

- The CMake backend runs `cmake`, `cmake --build` and `cmake --install` with conda-forge compilers.
- Host dependencies are what the code is compiled and linked against. Open MPI's run-exports add the matching run dependency to the package automatically.

## 2. Use the package in the prod environment

```toml
[feature.solver.dependencies]
grayscott = { path = "." }

[feature.solver.tasks]
simulate = { cmd = "mpirun --oversubscribe -n {{ ranks }} grayscott --size {{ size }} --steps {{ steps }}", args = [{ arg = "ranks", default = "2" }, { arg = "size", default = "256" }, { arg = "steps", default = "10000" }] }
```

and change `prod` to `{ features = ["mpi-bundled", "solver"], solve-group = "main" }`.

```bash
pixi install -e prod             # builds the package, then installs it
pixi list -e prod                # grayscott, openmpi, no compilers
pixi run -e prod which grayscott
pixi run -e prod simulate 4
```

## 3. Build the package file

```bash
pixi build -o dist
ls dist/                         # grayscott-0.1.0-<hash>_0.conda
pixi exec rattler-build package inspect dist/grayscott-*.conda
```

Look for the run dependencies: `openmpi >=5.0.x,<6.0a0` was added by Open MPI's run-exports.

## 4. Use it from another project

```bash
cd ../analysis
pixi init
pixi workspace preview add pixi-build
pixi add python numpy matplotlib-base
```

Add the solver as a path dependency in `pixi.toml`:

```toml
[dependencies]
grayscott = { path = "../grayscott" }
```

and a task that runs the parameter scan:

```bash
pixi task add scan "python scan.py --ranks 4"
pixi run scan
open data/gallery.png
```

The analysis project does not know or care how the solver is built.

## 5. Optional: optimise for your CPU

If you finish early. Compiler flags for your own code go into the build configuration of the package. Add to `pixi.toml`, below `[package.build]`:

```toml
[package.build.config]
extra-args = ["-DCMAKE_CXX_FLAGS=-march=native"]
```

Use `-march=native` on x86 (Linux, WSL2, Intel Mac) and `-mcpu=native` on ARM (Apple silicon, Linux aarch64). Put one flag per `-DCMAKE_CXX_FLAGS=...`: the backend splits the value at spaces, so `"-DCMAKE_CXX_FLAGS=-O3 -march=native"` fails. The Release build adds `-O3` anyway.

Compare the speed with and without the flag. pixi notices the changed manifest and rebuilds the package on the next run:

```bash
pixi run -e prod simulate 1 256 10000    # note the Mcell-updates/s; run it 2-3 times
```

On a MacBook with an M1 Pro both versions run at about 490 Mcell-updates/s: this stencil code is limited by memory bandwidth, not by arithmetic. On x86 you may see a difference. Either way, measure before you rely on a flag.

The flag is baked into the package. `-march=native` optimises for the machine that builds it, and the package can fail with *illegal instruction* on an older CPU. For a package that should run on all nodes of a cluster, target the oldest one, for example `-march=x86-64-v3` (AVX2: Intel Haswell and newer, AMD Zen).

Remove the `[package.build.config]` lines again before block 8.

## Outlook (not practised)

- Publish the `.conda` file to a channel (`pixi publish`, prefix.dev, S3, Artifactory) and replace `path = "../grayscott"` with a version requirement.
- Build variants: one package per MPI flavour or CPU instruction set, instead of one flag for everyone.
- Cross-compiling, e.g. `pixi build --target-platform linux-aarch64`.
