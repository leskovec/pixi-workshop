# Block 2: first workspace (13:30–14:05)

Work in a scratch directory outside the repository.

```bash
mkdir -p ~/pixi-playground && cd ~/pixi-playground
```

## 1. Create a workspace

```bash
pixi init demo
cd demo
cat pixi.toml
```

The manifest lists channels (where packages come from) and platforms (which operating systems the lock file covers).

## 2. Add dependencies and look at the lock file

```bash
pixi add python numpy matplotlib-base
cat pixi.toml
less pixi.lock
```

Questions:

- Which version of numpy did you get? Where is it written down?
- Find the URL and the sha256 of the numpy package in `pixi.lock`.
- Where did pixi install it? (`ls .pixi/envs/default`)

## 3. Run things in the environment

```bash
pixi run python -c "import numpy; print(numpy.__version__, numpy.__file__)"
pixi shell          # a shell with the environment active; leave with `exit`
```

There is no `activate` and no base environment. `pixi run` makes sure the environment matches the lock file before running the command.

## 4. Inspect

```bash
pixi list
pixi tree numpy         # why is each package there?
pixi search scipy
pixi info
```

## 5. PyPI packages

Some packages exist only on PyPI. pixi resolves them together with the conda packages (using uv underneath):

```bash
pixi add --pypi cowsay
pixi run python -c "import cowsay; cowsay.cow('hello from PyPI')"
grep -A2 pypi-dependencies pixi.toml
```

## 6. Versions, removing, updating

```bash
pixi add "python=3.12.*"     # pin a version range
pixi remove matplotlib-base
pixi update                  # newest versions allowed by pixi.toml, rewrites pixi.lock
pixi upgrade                 # also loosens the constraints in pixi.toml
git diff pixi.toml           # (if you ran git init) see what upgrade changed
```

`update` respects the constraints in `pixi.toml`; `upgrade` changes them.

## 7. Tools outside projects

```bash
pixi global install ripgrep      # installs `rg` into its own environment, on your PATH
pixi global list
pixi exec --spec python=3.11 python -V    # one-off command in a temporary environment
pixi exec cowpy "no installation needed"
```

## 8. Cleaning up

```bash
pixi info | grep -i cache    # shared package cache
pixi clean                   # removes .pixi/ in this workspace
pixi clean cache             # removes the shared cache (asks first)
```

Deleting `.pixi/` is always safe: `pixi install` recreates it from `pixi.lock`.
