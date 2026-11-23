"""Parameter scan: run grayscott for several (F, k) pairs and plot a gallery.

Expects the `grayscott` executable and `mpirun` on PATH, which is the case
inside a pixi environment that depends on the grayscott package.
"""

import argparse
import subprocess
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

# (F, k) pairs giving visibly different patterns
PARAMS = [
    (0.022, 0.051),  # stripes
    (0.035, 0.065),  # dividing spots
    (0.030, 0.062),  # spots filling the domain
    (0.026, 0.055),  # worms
    (0.040, 0.060),  # labyrinth
    (0.014, 0.054),  # wandering spots
]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ranks", type=int, default=2)
    parser.add_argument("--size", type=int, default=200)
    parser.add_argument("--steps", type=int, default=8000)
    parser.add_argument("--outdir", default="data")
    args = parser.parse_args()

    outdir = Path(args.outdir)
    outdir.mkdir(exist_ok=True)

    fields = []
    for F, k in PARAMS:
        out = outdir / f"v_F{F}_k{k}.npy"
        subprocess.run(
            ["mpirun", "--oversubscribe", "-n", str(args.ranks), "grayscott",
             "--size", str(args.size), "--steps", str(args.steps),
             "--F", str(F), "--k", str(k), "--output", str(out)],
            check=True,
        )
        fields.append(np.load(out))

    fig, axes = plt.subplots(2, 3, figsize=(9, 6))
    for ax, (F, k), v in zip(axes.flat, PARAMS, fields):
        ax.imshow(v, cmap="magma", interpolation="nearest")
        ax.set_title(f"F={F}, k={k}", fontsize=9)
        ax.set_axis_off()
    fig.tight_layout()
    fig.savefig(outdir / "gallery.png", dpi=120)
    print(f"wrote {outdir / 'gallery.png'}")


if __name__ == "__main__":
    main()
