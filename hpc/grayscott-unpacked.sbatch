#!/bin/bash
# Run the solver from an environment shipped with pixi-pack (block 8).
# No pixi needed on the cluster. Unpack once on the login node:
#
#   mkdir -p ~/gs-env && cd ~/gs-env
#   ./grayscott-installer.sh          # or: pixi-unpack environment.tar
#
# This creates env/ and activate.sh. Submit from ~/gs-env.

#SBATCH --job-name=gs-unpacked
#SBATCH --reservation=delavnica    # workshop nodes on NSC; remove outside the workshop
##SBATCH --partition=<partition>   # not needed with the reservation; set it otherwise
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --time=00:10:00
#SBATCH --output=gs-unpacked-%j.out

set -euo pipefail
cd "$SLURM_SUBMIT_DIR"
source ./activate.sh

grayscott --version
mpirun grayscott --size 2048 --steps 2000 --output "$PWD/unpacked.npy"
