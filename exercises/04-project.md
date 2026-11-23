# Block 4: the Gray-Scott example becomes a pixi project (14:15–14:50)

Work in `grayscott/`. Solution: `solutions/block-4-project/pixi.toml`.

```bash
cd grayscott
ls          # CMakeLists.txt, src/, scripts/, environment.yml
```

## 1. Start from the colleague's conda environment

```bash
pixi init --import environment.yml
cat pixi.toml
pixi install
```

Everything from `environment.yml` is now in one `[dependencies]` table.

## 2. Build and run by hand

```bash
pixi run cmake -S . -B build -G Ninja
pixi run cmake --build build
pixi run mpirun -n 2 build/grayscott --size 256 --steps 10000
pixi run python scripts/plot.py output/v.npy
open output/v.png        # macOS; on Linux: xdg-open
```

## 3. Tasks

Turn those commands into tasks:

```bash
pixi task add configure "cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release"
pixi task add build "cmake --build build" --depends-on configure
pixi task add plot "python scripts/plot.py output/v.npy"
pixi task list
pixi run build
```

Then edit `pixi.toml` by hand to give `run` arguments with defaults:

```toml
run = { cmd = "mpirun --oversubscribe -n {{ ranks }} build/grayscott --size {{ size }} --steps {{ steps }}", args = [{ arg = "ranks", default = "2" }, { arg = "size", default = "256" }, { arg = "steps", default = "10000" }], depends-on = ["build"] }
```

```bash
pixi run run            # defaults
pixi run run 4 512      # 4 ranks, 512 x 512
```

Optional: add `inputs` and `outputs` to `configure` so pixi skips it when `CMakeLists.txt` has not changed.

## 4. Features and environments

Not everything is needed everywhere. Split the dependencies:

- `[feature.build.dependencies]`: `cxx-compiler`, `cmake`, `ninja`
- `[feature.analysis.dependencies]`: `python`, `numpy`, `matplotlib-base`
- `[feature.test.dependencies]`: `python`, `numpy`
- `[dependencies]`: only `openmpi`

Move each task into the feature it needs (`[feature.build.tasks]`, ...). Then define the environments:

```toml
[environments]
default = { features = ["build", "analysis", "test"], solve-group = "main" }
analysis = { features = ["analysis"], solve-group = "main" }
prod = { features = [], solve-group = "main" }
```

```bash
pixi install --all
pixi list -e prod          # how much smaller is it?
pixi run -e analysis plot
du -sh .pixi/envs/*
```

The shared `solve-group` guarantees that a package present in several environments has the same version in all of them.

## 5. A test

Add a `test` task to the `test` feature that runs the solver with 1 and with 4 ranks and compares the outputs with `scripts/compare.py`. Use `depends-on` to chain the runs. Check the solution if you are stuck.

```bash
pixi run test
```

## 6. More platforms

```bash
pixi workspace platform add linux-64 linux-aarch64 osx-arm64 osx-64
wc -l pixi.lock
```

The lock file now pins every package for all four platforms. A colleague on a Linux cluster gets exactly the versions you tested on your Mac.

## 7. Back to conda

Colleagues who stay with conda can still use the project:

```bash
pixi workspace export conda-environment -e default > environment-dev.yml
pixi workspace export conda-explicit-spec -e prod --platform linux-64 explicit/
```

## 8. Commit

```bash
git add pixi.toml pixi.lock
```

`pixi.lock` belongs in git. `.pixi/` does not (it is in `.gitignore`).
