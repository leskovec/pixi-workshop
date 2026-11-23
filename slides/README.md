# Slides

The workshop slides, written in Markdown for [Slidev](https://sli.dev). pixi provides Node.js, and npm installs Slidev from `package-lock.json`.

```bash
cd slides
pixi run dev       # live preview at http://localhost:3030, opens the browser
pixi run export    # slides.pdf
pixi run build     # static site in dist/
```

The first run installs Slidev (about 600 MB in `node_modules/`) and a headless Chromium for the export.

- `slides.md`: the deck
- `style.css`: larger code font, no font ligatures (so `>=` stays `>=`)
- `public/cover.jpg`: Gray-Scott output, 640 × 640 grid, 30 000 steps, F = 0.029, k = 0.057
- `public/gs/`, `figures.py`: the figures for the Gray-Scott slides and the script that makes them

In the browser: arrow keys to move, `o` for the overview, `p` for presenter mode with notes and timer, `d` for dark mode.

The Gray-Scott figures in `public/gs/` come from the solver itself. To remake them, build it in `grayscott/` and run `pixi run python ../slides/figures.py` there.
