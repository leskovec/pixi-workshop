#!/bin/bash
# Run the solver from an environment shipped with pixi-pack (block 8).
# No pixi needed on the cluster. Unpack once on the login node:
#
#   mkdir -p /path/to/gs-env && cd /path/to/gs-env
#   ./grayscott-installer.sh          # or: pixi-unpack environment.tar
#
# This creates env/ and activate.sh.

#SBATCH --job-name=gs-unpacked
#SBATCH --partition=cpu            # adapt to the cluster
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --time=00:10:00
#SBATCH --output=gs-unpacked-%j.out

set -euo pipefail
source /path/to/gs-env/activate.sh

grayscott --version
mpirun grayscott --size 2048 --steps 2000 --output "$PWD/unpacked.npy"
