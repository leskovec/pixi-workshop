"""Figures for the Gray-Scott slides, made with the workshop solver itself.

Run from grayscott/ after `pixi run build` (block 4 manifest or later):

    pixi run python ../slides/figures.py

Writes PNGs to slides/public/gs/.
"""

import subprocess
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import FancyArrowPatch, Rectangle

OUT = Path(__file__).resolve().parent / "public" / "gs"
SOLVER = "build/grayscott"


def run(F, k, steps, size=256, ranks=4):
    out = Path("output") / f"fig-{F}-{k}-{steps}-{size}.npy"
    if not out.exists():
        subprocess.run(
            ["mpirun", "--oversubscribe", "-n", str(ranks), SOLVER,
             "--size", str(size), "--steps", str(steps),
             "--F", str(F), "--k", str(k), "--output", str(out)],
            check=True, stdout=subprocess.DEVNULL)
    return np.load(out)


def save_field(v, name, vmax=None):
    fig = plt.figure(figsize=(4, 4), dpi=100)
    ax = fig.add_axes([0, 0, 1, 1])
    ax.axis("off")
    ax.imshow(v, cmap="magma", vmin=0, vmax=vmax or max(v.max(), 1e-9),
              interpolation="bicubic")
    fig.savefig(OUT / name)
    plt.close(fig)


def evolution():
    for steps in [0, 1000, 3000, 6000, 12000, 25000]:
        save_field(run(0.035, 0.065, steps), f"evolution-{steps}.png", vmax=0.45)


def gallery():
    params = {
        "spots": (0.030, 0.062),
        "labyrinth": (0.030, 0.057),
        "holes": (0.039, 0.058),
        "chaos": (0.026, 0.051),
        "arcs": (0.010, 0.047),
        "extinct": (0.014, 0.045),
    }
    for name, (F, k) in params.items():
        save_field(run(F, k, 15000), f"pattern-{name}.png")


def stencil():
    fig, ax = plt.subplots(figsize=(4, 4), dpi=100)
    ax.set_xlim(-0.6, 4.6); ax.set_ylim(-0.6, 4.6); ax.set_aspect("equal"); ax.axis("off")
    for i in range(5):
        for j in range(5):
            ax.add_patch(Rectangle((i - 0.45, j - 0.45), 0.9, 0.9, fc="#eeeeee", ec="#bbbbbb"))
    for (i, j), w in {(2, 2): "−4", (1, 2): "1", (3, 2): "1", (2, 1): "1", (2, 3): "1"}.items():
        centre = (i, j) == (2, 2)
        ax.add_patch(Rectangle((i - 0.45, j - 0.45), 0.9, 0.9,
                               fc="#b5367a" if centre else "#f4a582", ec="#555555"))
        ax.text(i, j, w, ha="center", va="center", fontsize=20,
                color="white" if centre else "black")
    fig.savefig(OUT / "stencil.png", bbox_inches="tight", transparent=True)
    plt.close(fig)


def decomposition():
    n, ranks = 12, 3
    rows = n // ranks
    colours = ["#fde0c5", "#c6dbef", "#c7e9c0"]
    fig, ax = plt.subplots(figsize=(6.4, 4.4), dpi=100)
    ax.set_xlim(-0.5, n + 5.5); ax.set_ylim(-0.5, n + 0.5); ax.set_aspect("equal"); ax.axis("off")
    for r in range(ranks):
        y0 = n - (r + 1) * rows
        ax.add_patch(Rectangle((0, y0), n, rows, fc=colours[r], ec="#555555", lw=1.5))
        ax.text(n + 0.6, y0 + rows / 2, f"rank {r}", va="center", fontsize=15)
        for y in range(1, rows):
            ax.plot([0, n], [y0 + y, y0 + y], color="#999999", lw=0.5)
    for x in range(1, n):
        ax.plot([x, x], [0, n], color="#999999", lw=0.5)
    # halo exchange across the boundaries between ranks
    for r in range(ranks - 1):
        y = n - (r + 1) * rows
        ax.add_patch(Rectangle((0, y - 0.5), n, 1, fc="none", ec="#b5367a", lw=2.5, ls="--"))
        ax.add_patch(FancyArrowPatch((n + 3.6, y + 0.9), (n + 3.6, y - 0.9),
                                     arrowstyle="<|-|>", mutation_scale=18, color="#b5367a", lw=2))
    ax.text(n + 4.2, n / 2, "halo\nexchange\nevery step", va="center", fontsize=13, color="#b5367a")
    fig.savefig(OUT / "decomposition.png", bbox_inches="tight", transparent=True)
    plt.close(fig)


if __name__ == "__main__":
    OUT.mkdir(parents=True, exist_ok=True)
    if not Path(SOLVER).exists():
        sys.exit("build the solver first: pixi run build")
    evolution(); gallery(); stencil(); decomposition()
    print("wrote", OUT)
