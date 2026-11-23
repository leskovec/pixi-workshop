#!/bin/bash
# Build and run the solver against the cluster's own Open MPI (block 6/7
# manifest, `cluster` environment with the external_* placeholder).
# On the login node, once:
#
#   module load OpenMPI/5.0.x          # adapt; match the version in pixi.toml
#   cd /path/to/grayscott
#   pixi install --locked -e cluster
#   pixi run -e cluster --frozen build
#
# then submit: sbatch grayscott-system.sbatch

#SBATCH --job-name=gs-system
#SBATCH --partition=cpu            # adapt to the cluster
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=8
#SBATCH --time=00:10:00
#SBATCH --output=gs-system-%j.out

set -euo pipefail
module load OpenMPI/5.0.x          # adapt, same as above
cd /path/to/grayscott

eval "$(pixi shell-hook -e cluster --as-is)"

build/grayscott --version           # should report the site's Open MPI
srun build/grayscott --size 4096 --steps 2000 --output output/system.npy

# The mpi4py check from block 6 works the same way:
# srun python scripts/hello_mpi.py
