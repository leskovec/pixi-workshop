"""Check that two grayscott outputs agree, e.g. runs with 1 and 4 MPI ranks."""

import sys

import numpy as np


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: compare.py A.npy B.npy")
    a, b = np.load(sys.argv[1]), np.load(sys.argv[2])
    diff = np.max(np.abs(a - b))
    print(f"max |a - b| = {diff:.3e}")
    sys.exit(0 if diff < 1e-12 else 1)


if __name__ == "__main__":
    main()
