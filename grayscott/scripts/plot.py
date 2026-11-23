"""Plot the v field written by grayscott (a 2D NumPy .npy file) as a PNG."""

import argparse
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", nargs="?", default="output/v.npy")
    parser.add_argument("-o", "--output", help="PNG file (default: input with .png suffix)")
    args = parser.parse_args()

    v = np.load(args.input)
    out = Path(args.output) if args.output else Path(args.input).with_suffix(".png")

    fig, ax = plt.subplots(figsize=(6, 6))
    ax.imshow(v, cmap="magma", interpolation="nearest")
    ax.set_axis_off()
    ax.set_title(f"Gray-Scott v, {v.shape[0]} x {v.shape[1]}")
    fig.savefig(out, dpi=150, bbox_inches="tight")
    print(f"wrote {out}")


if __name__ == "__main__":
    main()
