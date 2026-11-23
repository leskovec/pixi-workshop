# Setup before the workshop

Please do this before 25 November. It takes about 10 minutes, most of it downloading.

## 1. Operating system

- Linux or macOS: nothing extra.
- Windows: install WSL2 with Ubuntu (`wsl --install` in an administrator PowerShell, then reboot) and do everything below inside the Ubuntu terminal. The MPI and build exercises need Linux or macOS.

## 2. Install pixi

```bash
curl -fsSL https://pixi.sh/install.sh | PIXI_VERSION=v0.84.2 bash
```

Open a new terminal and check:

```bash
pixi --version      # pixi 0.84.2
```

On macOS, `brew install pixi` works too.

If you already have pixi: `pixi self-update --version 0.84.2`. Newer versions should work too, but the workshop is tested with 0.84.2.

Pixi also runs natively on Windows (`winget install prefix-dev.pixi`, or the PowerShell installer on https://pixi.sh). That is enough for the first exercise, but everything with MPI needs WSL2, because conda-forge has no Open MPI for Windows. So on Windows, install pixi inside WSL2 with the command above.

## 3. Get the material

```bash
git clone <repository URL>
cd pixi-workshop-examples
```

No git? `pixi global install git` installs it.

## 4. Check the setup and fill the package cache

```bash
cd setup
pixi run check
```

Expected output, after the downloads:

```
rank 0 of 2: setup OK
rank 1 of 2: setup OK
```

If this fails, send the full output to  before the workshop.

## 5. Optional: cluster access

Part of the second half is demonstrated on a SLING cluster. If you have an account, you can follow along there; see `hpc/pixi-env.sh` for the one-time pixi setup on a cluster.
