# Block 5: pixi on HPC (15:15–15:35)

Mostly an instructor demo on the cluster. Steps 1 and 2 also work on a laptop.

## 1. `--locked`, `--frozen`, `--as-is`

```bash
mkdir /tmp/lockdemo && cd /tmp/lockdemo
bash /path/to/pixi-workshop-examples/hpc/locked-vs-frozen.sh
```

| Option | Checks lock file against manifest | Solves / rewrites lock | Installs / updates environment | Use for |
|---|---|---|---|---|
| (none) | yes | yes | yes | everyday development |
| `--locked` | yes, error if out of date | no | yes | reproducing a committed environment; catches a manifest edited without relocking |
| `--frozen` | no | no | yes, exactly what `pixi.lock` says | installing on machines where nothing may change |
| `--as-is` | no | no | no (`--frozen` + `--no-install`) | batch jobs, after installing on the login node |

The same switches exist as environment variables (`PIXI_LOCKED=1`, `PIXI_FROZEN=1`), handy in job scripts.

The demo also shows that a package missing from the environment can silently come from the system instead: pixi puts the environment first on `PATH`, it does not remove the rest.

## 2. Keep environments out of `$HOME`

```bash
pixi info                       # Cache dir, environments
cat hpc/pixi-env.sh             # PIXI_HOME, PIXI_CACHE_DIR, detached-environments
```

Try detached environments in one project:

```bash
cd grayscott
pixi config set --local detached-environments /tmp/pixi-envs
pixi install
ls /tmp/pixi-envs
pixi config unset --local detached-environments
```

## 3. On the cluster (demo)

```bash
source pixi-env.sh                         # see hpc/pixi-env.sh
git clone <repo> && cd pixi-workshop-examples/grayscott
cp ../solutions/block-7-pixi-build/grayscott/pixi.toml .
pixi install --locked -e prod              # on the login node: solve + download once
sbatch ../hpc/grayscott-bundled.sbatch     # job uses --as-is
```

What to point out in `hpc/grayscott-bundled.sbatch`:

- absolute `cd` to the project, jobs do not start in the submit directory on every system
- `eval "$(pixi shell-hook -e prod --as-is)"` activates once; `srun pixi run ...` would start pixi in every rank
- no conda or Python modules loaded

## 4. Jupyter on a cluster

pixi has no environment registry, so Jupyter does not find environments by itself. Add `ipykernel` to the environment and register it:

```bash
pixi add -f analysis ipykernel
pixi run -e analysis python -m ipykernel install --user --name grayscott-analysis
```
