#!/bin/bash
# Build and run the solver against the cluster's own Open MPI (block 7
# manifest, `cluster` environment with the external_* placeholder).
# On the login node, once, in pixi-workshop/grayscott:
#
#   module load OpenMPI/5.0.x          # adapt; match the version in pixi.toml
#   pixi install --locked -e cluster
#   pixi run -e cluster --frozen build
#
# then submit from the same folder: sbatch ../hpc/grayscott-system.sbatch

#SBATCH --job-name=gs-system
#SBATCH --reservation=delavnica    # workshop nodes on NSC; remove outside the workshop
##SBATCH --partition=<partition>   # not needed with the reservation; set it otherwise
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --time=00:10:00
#SBATCH --output=gs-system-%j.out

set -euo pipefail
module load OpenMPI/5.0.x          # adapt, same as above
cd "$SLURM_SUBMIT_DIR"             # the grayscott/ folder you submitted from

eval "$(pixi shell-hook -e cluster --as-is)"

build/grayscott --version           # should report the site's Open MPI
srun build/grayscott --size 4096 --steps 2000 --output output/system.npy

# The mpi4py check from block 6 works the same way:
# srun python scripts/hello_mpi.py
