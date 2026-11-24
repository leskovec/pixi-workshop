#!/bin/bash
# Run the packaged solver with Open MPI from conda-forge (block 7 manifest,
# `prod` environment). Install the environment on the login node first:
#
#   cd pixi-workshop/grayscott
#   cp ../solutions/block-7-pixi-build/grayscott/pixi.{toml,lock} .
#   pixi install --locked -e prod
#
# then submit from the same folder: sbatch ../hpc/grayscott-bundled.sbatch

#SBATCH --job-name=gs-bundled
#SBATCH --reservation=delavnica    # workshop nodes on NSC; remove outside the workshop
##SBATCH --partition=<partition>   # not needed with the reservation; set it otherwise
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --time=00:10:00
#SBATCH --output=gs-bundled-%j.out

set -euo pipefail
cd "$SLURM_SUBMIT_DIR"             # the grayscott/ folder you submitted from

# Activate once in the batch shell. --as-is: no solve, no install, no network.
# (`srun pixi run ...` would start pixi once per MPI rank.)
eval "$(pixi shell-hook -e prod --as-is)"

which grayscott mpirun
grayscott --version

# Option 1: Open MPI's own launcher, which picks up the Slurm allocation.
mpirun grayscott --size 4096 --steps 2000 --output output/bundled.npy

# Option 2: launch through Slurm. Works if Slurm's PMIx is compatible with the
# conda-forge Open MPI; check `srun --mpi=list` first.
# srun --mpi=pmix grayscott --size 4096 --steps 2000 --output output/bundled.npy
