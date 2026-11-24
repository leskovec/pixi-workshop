# Block 5: pixi on HPC (15:15–15:35)

Mostly an instructor demo on the cluster. Steps 1 and 2 also work on a laptop.

## 1. `--locked`, `--frozen`, `--as-is`

```bash
mkdir /tmp/lockdemo && cd /tmp/lockdemo
bash /path/to/pixi-workshop/hpc/locked-vs-frozen.sh
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

## 3. On the NSC cluster

You get a user name (`pixi001` to `pixi030`) and a password during the workshop. Two nodes, `nsc-lou001` and `nsc-lou002`, are reserved for us; the job scripts in `hpc/` ask for the reservation `delavnica`.

```bash
ssh pixiXXX@nsc-login.ijs.si               # your own number

# once: pixi and the material
curl -fsSL https://pixi.sh/install.sh | PIXI_VERSION=v0.84.2 bash
export PATH=$HOME/.pixi/bin:$PATH
git clone https://github.com/leskovec/pixi-workshop.git
cd pixi-workshop/grayscott
```

The manifest and the lock file come from git: the lock file was solved on a laptop, and `--locked` makes the cluster install exactly that.

```bash
cp ../solutions/block-7-pixi-build/grayscott/pixi.{toml,lock} .
pixi install --locked -e prod              # on the login node: download and build once
sbatch ../hpc/grayscott-bundled.sbatch     # the job uses --as-is
squeue -u $USER
cat gs-bundled-*.out
```

`pixi install` also builds the solver as a conda package; block 7 shows how that works.

What to look at in `hpc/grayscott-bundled.sbatch`:

- `#SBATCH --reservation=delavnica` sends the job to the workshop nodes
- `eval "$(pixi shell-hook -e prod --as-is)"` activates once in the batch shell; `srun pixi run ...` would start pixi in every rank
- `--as-is`: the job never solves, downloads or installs, so compute nodes need no internet
- no conda or Python modules loaded

For your own projects, `hpc/pixi-env.sh` shows how to keep pixi's cache and environments out of a small `$HOME`.

## 4. Jupyter on a cluster

pixi has no environment registry, so Jupyter does not find environments by itself. Add `ipykernel` to the environment and register it:

```bash
pixi add -f analysis ipykernel
pixi run -e analysis python -m ipykernel install --user --name grayscott-analysis
```
