# Block 8: shipping with pixi-pack, and can it replace a container? (16:35–16:50)

Work in `grayscott/` with the block 7 manifest.

## 1. Pack the prod environment

```bash
pixi global install pixi-pack pixi-unpack
pixi-pack -e prod                      # -> environment.tar (about 25 MB)
```

pixi-pack downloads every package in the `prod` environment from the lock file, builds `grayscott` from source with pixi-build, and puts everything into one tar file.

## 2. Unpack and run without pixi

```bash
mkdir /tmp/unpacked && cp environment.tar /tmp/unpacked && cd /tmp/unpacked
pixi-unpack environment.tar            # -> env/ and activate.sh
ls

# a clean shell, pixi not on PATH
env -i HOME=$HOME PATH=/usr/bin:/bin bash -c \
  'source activate.sh && mpirun --oversubscribe -n 2 grayscott --size 256 --steps 2000 --output /tmp/unpacked/v.npy'
```

## 3. A single self-extracting installer

```bash
cd -                                   # back to grayscott/
pixi-pack -e prod --create-executable -o grayscott-installer.sh
mkdir /tmp/inst && cd /tmp/inst && /path/to/grayscott-installer.sh
```

The installer contains `pixi-unpack` itself, so the target machine needs nothing but a shell.

## 4. Packing for another platform

```bash
pixi-pack -e prod -p linux-64 -o grayscott-linux.tar
```

Pure conda-forge environments pack for any platform. Our environment contains a package built from source, and C++ code is not cross-compiled from macOS to Linux here, so this fails on a Mac. Two ways around it:

- run `pixi-pack -p linux-64` on a Linux machine (WSL2, a cluster login node, a CI runner), or
- build the `.conda` file on Linux with `pixi build`, then pack without the source dependency and add the package with `--inject grayscott-0.1.0-*.conda`.

Unpacked on the cluster, it runs with `hpc/grayscott-unpacked.sbatch`.

## 5. Can pixi replace a container? (discussion)

Go through the reasons people use Apptainer or Docker on HPC, and check each against what you did today.

| What a container gives you | With pixi and pixi-pack | Replaced? |
|---|---|---|
| The same software on every machine | `pixi.lock`: exact packages and hashes, per platform | yes |
| Several platforms | one lock file for Linux x86-64, Linux ARM and macOS | yes |
| Nothing to install on the target | `pixi-pack` tar or self-extracting installer; no runtime, no root | yes |
| MPI across nodes | bundled MPI or the site MPI (block 6) | yes |
| Quick changes | edit `pixi.toml`, the next `pixi run` catches up | yes |
| Its own OS and glibc | uses the host's kernel and glibc; needs glibc at least `__glibc` from `pixi.lock` | no |
| Software outside conda-forge and PyPI | not available | no |
| Services, Kubernetes, cloud batch | not what pixi is for | no |

Which of the "no" rows matter for your own work?
