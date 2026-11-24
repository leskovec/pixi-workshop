---
theme: seriph
title: "Pixi: reproducible environments on HPC"
author: Luka Leskovec
info: |
  EuroCC Slovenia / SLING workshop, 25 November 2026 (online).
background: /cover.jpg
class: text-left
colorSchema: light
aspectRatio: 16/9
canvasWidth: 980
transition: none
fonts:
  sans: Inter
  mono: JetBrains Mono
  provider: google
---

# Pixi
## Reproducible environments on HPC

<div class="mt-10 opacity-90">
EuroCC Slovenia / SLING workshop<br>
25 November 2026, online
</div>

<div class="abs-bl m-6 text-sm opacity-80">
Luka Leskovec · Faculty of Mathematics and Physics, University of Ljubljana
</div>

<!--
Welcome. Cover image: the Gray-Scott solver we build today, 640x640 grid, 30000 steps, F=0.029, k=0.057, 4 MPI ranks.
-->

---

# The afternoon

<div class="grid grid-cols-2 gap-10 text-sm">
<div>

**Part 1: introduction and setup**

0. Welcome, setup check
1. Environment management on HPC
2. *Hands-on:* first workspace
3. Tasks, features, environments
4. *Hands-on:* the example becomes a project

*Break*

</div>
<div>

**Part 2: practical workflow**

5. *Hands-on:* pixi on a cluster, `--locked` vs `--frozen`
6. *Hands-on:* MPI, bundled vs system
7. *Hands-on:* packaging with pixi-build
8. *Hands-on:* shipping with pixi-pack
9. For fun, wrap-up

</div>
</div>

---
layout: image-right
image: /cover.jpg
backgroundSize: cover
---

# One example all afternoon

The **Gray-Scott model**: two chemicals that react and diffuse on a 2D grid, and form patterns.


- A small C++ code, about 200 lines, built with CMake
- Parallel with MPI
- Python scripts plot the result
- Fast enough to run on a laptop, big enough to need a cluster if you want


<!--
The cover image and this one: 640 x 640 grid, 30000 steps, F = 0.029, k = 0.057, 4 MPI ranks, 7 s on a laptop.
-->

---

# Two chemicals, U and V

<div class="grid grid-cols-2 gap-10 mt-4">
<div>

Two reactions:

$$
U + 2V \;\longrightarrow\; 3V
$$

$$
V \;\longrightarrow\; P
$$

<div class="text-sm opacity-80 mt-4">P is an inert product, we do not follow it.</div>

</div>
<div>


- **V makes more of itself**, using up U on the way
- **V decays** into P at rate $k$, the *kill* rate
- **U is fed in** from a reservoir at rate $F$, the *feed* rate
- Both **diffuse**; U spreads twice as fast as V


</div>
</div>

<div class="mt-10">

$u(x, y, t)$ and $v(x, y, t)$ are the concentrations. We plot $v$.

</div>

---

# The equations, term by term

$$
\begin{aligned}
\frac{\partial u}{\partial t} &= \textcolor{#2b8cbe}{D_u \nabla^2 u} \;\textcolor{#b5367a}{-\; u v^2} \;\textcolor{#e66101}{+\; F\,(1-u)} \\[6pt]
\frac{\partial v}{\partial t} &= \textcolor{#2b8cbe}{D_v \nabla^2 v} \;\textcolor{#b5367a}{+\; u v^2} \;\textcolor{#e66101}{-\; (F+k)\,v}
\end{aligned}
$$

<div class="mt-4">

| | Term | Meaning |
|---|---|---|
| <span style="color:#2b8cbe">■</span> | $D\,\nabla^2$ | diffusion: each chemical spreads to its neighbours; $D_u = 0.16$, $D_v = 0.08$ |
| <span style="color:#b5367a">■</span> | $u v^2$ | the reaction $U + 2V \to 3V$: one U and two V meet, U turns into V |
| <span style="color:#e66101">■</span> | <span class="whitespace-nowrap">$F(1-u)$</span> | feed: $F$ is the rate at which we add U to the domain |
| <span style="color:#e66101">■</span> | <span class="whitespace-nowrap">$(F+k)\,v$</span> | removal: V decays into the inert P at rate $k$, and the feed flow washes it out at rate $F$ |

</div>

<!--
The (1 - u) in the feed: the reservoir has concentration 1, so the feed slows down as u approaches 1.
-->

---

# From a square to spots

Start: $u = 1$, $v = 0$ everywhere, with a square of $v = 0.25$ in the middle. $F = 0.035$, $k = 0.065$.

<div class="grid grid-cols-6 gap-2 mt-8 text-center text-sm">
<div><img src="/gs/evolution-0.png" class="rounded"/>start</div>
<div><img src="/gs/evolution-1000.png" class="rounded"/>1 000 steps</div>
<div><img src="/gs/evolution-3000.png" class="rounded"/>3 000</div>
<div><img src="/gs/evolution-6000.png" class="rounded"/>6 000</div>
<div><img src="/gs/evolution-12000.png" class="rounded"/>12 000</div>
<div><img src="/gs/evolution-25000.png" class="rounded"/>25 000</div>
</div>


- A spot of V grows by eating the U around it
- U runs out in its middle, so the spot splits in two
- The new spots push each other apart, until they fill the domain


<!--
Each image is a separate run of the solver with a different --steps; the noise is deterministic, so they are frames of the same evolution. 256 x 256 grid.
-->

---

# Why patterns form

<div class="grid grid-cols-2 gap-10 mt-4">
<div>


- V is **self-amplifying**: where there is a little V, $u v^2$ makes more
- Growing V **uses up U** in its neighbourhood
- U diffuses faster than V, so a spot of V sits in a **wider ring of low U**
- A second spot cannot grow inside that ring, so spots keep a **typical distance**


</div>
<div>

Local growth plus a longer-range brake is the recipe Turing described in 1952 for patterns in biology.

Here it gives spots, stripes, labyrinths or nothing at all, depending on just two numbers, $F$ and $k$.

<div class="flex gap-3 mt-6 text-center text-xs">
<div><img src="/gs/evolution-1000.png" class="w-32 rounded"/>1 000 steps</div>
<div><img src="/gs/evolution-3000.png" class="w-32 rounded"/>3 000 steps</div>
</div>

</div>
</div>

---

# Two numbers, many patterns

<div class="grid grid-cols-6 gap-3 mt-6 text-center text-sm">
<div><img src="/gs/pattern-spots.png" class="rounded"/>spots<br><span class="opacity-60">F 0.030, k 0.062</span></div>
<div><img src="/gs/pattern-labyrinth.png" class="rounded"/>labyrinth<br><span class="opacity-60">F 0.030, k 0.057</span></div>
<div><img src="/gs/pattern-holes.png" class="rounded"/>holes<br><span class="opacity-60">F 0.039, k 0.058</span></div>
<div><img src="/gs/pattern-chaos.png" class="rounded"/>chaos<br><span class="opacity-60">F 0.026, k 0.051</span></div>
<div><img src="/gs/pattern-arcs.png" class="rounded"/>moving arcs<br><span class="opacity-60">F 0.010, k 0.047</span></div>
<div><img src="/gs/pattern-extinct.png" class="rounded"/>V dies out<br><span class="opacity-60">F 0.014, k 0.045</span></div>
</div>


- More feed $F$: more U to grow on, V spreads
- More kill $k$: V disappears faster, patterns break into spots, then vanish
- Same code, same start, 15 000 steps each; only `--F` and `--k` change


<!--
In block 7 analysis/scan.py runs such a scan with the packaged solver.
-->

---

# How the code computes it

<div class="grid grid-cols-[1fr_220px] gap-8">
<div>

A grid of $N \times N$ cells, periodic at the edges. Every step, every cell:

$$
\nabla^2 u_{ij} \approx u_{i-1,j} + u_{i+1,j} + u_{i,j-1} + u_{i,j+1} - 4\,u_{ij}
$$

$$
u_{ij} \leftarrow u_{ij} + \Delta t \,\big( D_u \nabla^2 u_{ij} - u_{ij} v_{ij}^2 + F\,(1 - u_{ij}) \big)
$$

and the same for $v$.


- explicit Euler, $\Delta t = 1$, grid spacing 1
- a *stencil* code: each cell needs only its four neighbours
- 256 × 256 cells, 10 000 steps: about a second on a laptop


</div>
<div class="text-center text-sm">
<img src="/gs/stencil.png"/>
the 5-point stencil
</div>
</div>

<!--
Stability of explicit Euler for diffusion needs D dt <= 0.25 in 2D; with D_u = 0.16 and dt = 1 we are inside.
-->

---

# Parallel with MPI

<div class="grid grid-cols-[1fr_380px] gap-8">
<div>


- The grid is cut into horizontal **strips**, one per MPI rank
- To update its edge rows, a rank needs one row from each neighbour
- So every step, neighbours swap edge rows: the **halo exchange** (`MPI_Sendrecv`)
- At the end, rank 0 gathers the strips and writes `output/v.npy`
- The result is bitwise identical for any number of ranks, which gives us a test


</div>
<div>
<img src="/gs/decomposition.png"/>
</div>
</div>

<!--
Same structure as most stencil codes on HPC: weather, CFD, lattice QCD. That is why the example transfers to your own code.
-->

---
layout: center
---

# We will not write any code today

<div class="text-lg mt-6 max-w-160">

The solver is finished and works. We take it as given, like a code you inherit from a colleague, and ask how to **build, run and ship** it reproducibly:

</div>

<div class="mt-6 text-base">

| | |
|---|---|
| by hand with CMake, then as pixi tasks | block 4 |
| on a remote cluster | block 5 |
| with bundled MPI and with the cluster's MPI | block 6 |
| as a conda package with pixi-build | block 7 |
| as a self-contained archive with pixi-pack, instead of a container | block 8 |

</div>

<div class="mt-6 text-sm opacity-70">
No C++ knowledge needed. If you are curious: <code>grayscott/src/grayscott.cpp</code>
</div>

---

# Install pixi

<div class="grid grid-cols-[110px_1fr] gap-x-6 gap-y-3 items-start mt-2">

<div class="font-bold pt-2">Linux</div>
<div>

```bash
curl -fsSL https://pixi.sh/install.sh | PIXI_VERSION=v0.84.2 bash
```

</div>

<div class="font-bold pt-2">macOS</div>
<div>

```bash
curl -fsSL https://pixi.sh/install.sh | PIXI_VERSION=v0.84.2 bash
# or: brew install pixi
```

</div>

<div class="font-bold pt-2">Windows</div>
<div>

```powershell
wsl --install            # PowerShell as administrator, then reboot
```

<div class="text-sm mt-1 mb-2">then, in the Ubuntu terminal, the Linux command above. Pixi on its own also runs natively:</div>

```powershell
winget install prefix-dev.pixi
```

</div>
</div>

<div class="mt-4 text-sm">

Open a new terminal: `pixi --version` should print `pixi 0.84.2`. Already have pixi? `pixi self-update --version 0.84.2`

</div>

<!--
Native Windows pixi is enough for the first workspace and the fun demos. Everything with MPI (blocks 4 to 8) needs WSL2, Linux or macOS: conda-forge has no Open MPI for Windows.
Native installer with a pinned version: $Env:PIXI_VERSION = "v0.84.2"; powershell -ExecutionPolicy ByPass -c "irm -useb https://pixi.sh/install.ps1 | iex"
-->

---

# Setup check

<div class="step-tag">block 0</div>

```bash
git clone https://github.com/leskovec/pixi-workshop.git
cd pixi-workshop/setup
pixi run check
```

```text
rank 0 of 2: setup OK
rank 1 of 2: setup OK
```

<div class="mt-8">

The first run downloads compilers, Open MPI and Python, so it takes a few minutes.

On Windows, run this inside WSL2: the workshop example uses MPI, and conda-forge has no Open MPI for Windows.

Stuck? Post the error in the chat. Every step has a number; `solutions/4.3/pixi.toml` is the manifest after step 4.3, so you can always copy it in and continue.

</div>

---
layout: section
---

# Environment management on HPC

Block 1

---
zoom: 0.9
---

# The usual toolbox

| Tool | Works well for | Where it breaks |
|---|---|---|
| Modules | what the centre has built and tuned | the version you need is missing; every cluster names things differently |
| `pip` + `venv` | pure Python | anything with C, C++, Fortran or MPI underneath |
| conda | binaries for many languages | slow solves, a shared base environment, no lock file by default |
| Containers | fixing the whole OS layer | build tooling, image size, MPI across nodes |


And on top of that:

- home quotas and inode limits: today's development environment has 21 000 files
- "works on my laptop"
- solving the same `environment.yml` a year later gives a different environment


<!--
Ask: who uses which of these today? Quick poll in the chat.
-->

---

# The conda ecosystem

<div class="grid grid-cols-2 gap-8">
<div>

A conda package is a **binary** for one platform, with its dependencies declared.

That covers far more than Python:

- compilers: `gcc`, `clang`, `gfortran`
- MPI: `openmpi`, `mpich`
- libraries: HDF5, FFTW, BLAS/LAPACK, Boost
- R, Julia, Rust, Node.js, …

</div>
<div>

Channels:

- **conda-forge**: community-run, the default for pixi
- **bioconda**: life sciences, on top of conda-forge
- pixi uses only the channels listed in the manifest, in that order

</div>
</div>


---

# How pixi works: the big picture

<img src="/pixi-overview.svg" class="w-full mt-2" alt="pixi overview: you edit pixi.toml, pixi solves it into pixi.lock using the channels, downloads packages into the cache, links them into .pixi/envs/default, and runs your program; pixi.lock travels by git to colleagues and clusters"/>

<!--
Walk through it left to right: you only ever touch pixi.toml. Pixi asks the channels which packages exist and solves (1), writing pixi.lock. Then it downloads exactly the files in the lock into the cache (2), links them into the project (3), and runs your program in it (4). The lock file, not the environment, is what travels: git takes it to a colleague, a cluster, or to yourself a year later. The next slides zoom into each part.
-->

---

# How pixi works: three things in your project

<div class="grid grid-cols-3 gap-5 mt-6">

<div class="rounded-lg border-2 border-[#2b8cbe] p-4">
<div class="text-xl font-mono font-bold text-[#2b8cbe]">pixi.toml</div>
<div class="text-sm opacity-70 mb-3">the manifest · <b>you</b> write it</div>

What you **want**: channels, platforms, and loose requirements like `numpy = ">=2"`.

<div class="mt-3 text-sm">✔ commit to git</div>
</div>

<div class="rounded-lg border-2 border-[#b5367a] p-4">
<div class="text-xl font-mono font-bold text-[#b5367a]">pixi.lock</div>
<div class="text-sm opacity-70 mb-3">the lock file · <b>pixi</b> writes it</div>

What you **get**: every package with exact version, URL and hash, for every platform.

<div class="mt-3 text-sm">✔ commit to git</div>
</div>

<div class="rounded-lg border-2 border-[#e66101] p-4">
<div class="text-xl font-mono font-bold text-[#e66101]">.pixi/</div>
<div class="text-sm opacity-70 mb-3">the environments · <b>pixi</b> creates them</div>

The installed packages, made from the lock file. Safe to delete, pixi rebuilds it.

<div class="mt-3 text-sm">✘ never commit</div>
</div>

</div>

<div class="mt-8 text-center text-lg">

Same `pixi.lock` → same environment, on any machine with that platform.

</div>

<!--
Analogy for those who know it: pixi.toml is like requirements.txt or environment.yml, pixi.lock like a conda-lock or uv.lock file. .gitignore gets .pixi/ automatically from pixi init.
-->

---

# From a wish to a running program

<div class="grid grid-cols-4 gap-3 mt-6 text-center">

<div class="rounded-lg bg-[#2b8cbe]/10 p-3">
<div class="font-bold">1. Ask</div>
<div class="text-sm mt-2">you add a requirement to <code>pixi.toml</code></div>
</div>

<div class="rounded-lg bg-[#b5367a]/10 p-3">
<div class="font-bold">2. Solve</div>
<div class="text-sm mt-2">pixi picks versions that all fit together, for every platform, and writes <code>pixi.lock</code></div>
</div>

<div class="rounded-lg bg-[#e66101]/10 p-3">
<div class="font-bold">3. Install</div>
<div class="text-sm mt-2">downloads into a shared cache, then links into <code>.pixi/envs/default</code></div>
</div>

<div class="rounded-lg bg-gray-500/10 p-3">
<div class="font-bold">4. Run</div>
<div class="text-sm mt-2">starts your command with the environment active</div>
</div>

</div>

<div class="grid grid-cols-2 gap-8 mt-8">
<div v-click>

```bash
pixi add numpy          # steps 1, 2 and 3
pixi run python -V      # step 4
```

</div>
<div class="text-sm">

`pixi add numpy` writes `numpy = ">=2.5.5,<3"` into the manifest. The lock file gets `numpy-2.5.5-py315h3699dd1_1.conda` and the 26 other packages it needs on macOS, Python and OpenBLAS among them.

The solve is the slow, clever part. Pixi does it once, on your laptop, for all platforms. After that, installing is just downloading the listed files.

</div>
</div>

---

# What `pixi run` does, every time

<div class="grid grid-cols-4 gap-3 mt-4 text-center">

<div class="rounded-lg bg-[#b5367a]/10 p-3">
<div class="font-bold">1. Lock up to date?</div>
<div class="text-sm mt-2">If <code>pixi.toml</code> changed since <code>pixi.lock</code> was written: solve again</div>
</div>

<div class="rounded-lg bg-[#e66101]/10 p-3">
<div class="font-bold">2. Installed?</div>
<div class="text-sm mt-2">If <code>.pixi/</code> differs from <code>pixi.lock</code>: install the differences</div>
</div>

<div class="rounded-lg bg-[#2b8cbe]/10 p-3">
<div class="font-bold">3. Activate</div>
<div class="text-sm mt-2">set <code>PATH</code> and the environment variables, for this command only</div>
</div>

<div class="rounded-lg bg-gray-500/10 p-3">
<div class="font-bold">4. Run</div>
<div class="text-sm mt-2">your command, inside the environment</div>
</div>

</div>



- You never get a stale environment: edit `pixi.toml`, and the next `pixi run` catches up
- Activation only lives as long as the command, so there is no `conda activate` to forget
- `pixi shell` does the same, then opens a shell inside the environment
- On a cluster you may not want the first two checks: that is `--locked` and `--frozen`, in block 5


---

# A manifest

```toml {all|1-5|7-9|11-12|all}
[workspace]
name = "demo"
channels = ["conda-forge"]
platforms = ["linux-64", "osx-arm64"]
version = "0.1.0"

[dependencies]
python = "3.13.*"
numpy = ">=2"

[tasks]
hello = "python -c 'import numpy; print(numpy.__version__)'"
```

<v-click>

```bash
pixi add matplotlib    # edits pixi.toml, re-solves, updates pixi.lock, installs
pixi run hello
```

</v-click>

---

# The lock file

<div class="grid grid-cols-2 gap-6">
<div>

```yaml
version: 7
platforms:
- name: linux-64
  virtual-packages:
  - __glibc=2.28
  ...
environments:
  prod:
    packages:
      linux-64:
      - conda: https://conda.anaconda.org/
          conda-forge/linux-64/
          libfabric-2.7.0-ha770c72_0.conda
      ...
```

</div>
<div>

```yaml
- conda: https://conda.anaconda.org/
    conda-forge/osx-arm64/openmpi-5.0.11-
    h8e43103_1.conda
  sha256: c3673c4002408442199154125cf0...
  depends:
  - libgfortran5 >=15.3.0
  - libhwloc >=2.13.0,<2.13.1.0a0
  - __osx >=11.0
  ...
```

</div>
</div>


- Every package by exact URL and hash, for every platform and environment
- `__glibc=2.28`: the oldest Linux the environment will run on
- Commit it to git together with `pixi.toml`


---

# Where things live

<div class="grid grid-cols-[1.25fr_1fr] gap-8">
<div>

```text
~/.cache/rattler/cache/           # Linux
~/Library/Caches/rattler/cache/   # macOS
  pkgs/numpy-2.5.5-.../           ← downloaded once

my-project/
  pixi.toml
  pixi.lock
  .pixi/envs/default/             ← linked from the cache
  .pixi/envs/test/

other-project/
  .pixi/envs/default/             ← same numpy, no download,
                                     almost no extra space
~/.pixi/bin/                      ← pixi and pixi global tools
```

</div>
<div>


- One cache for all projects; environments are **linked** from it, not copied
- Ten projects with the same numpy cost one numpy
- `pixi info` prints these paths
- `pixi clean` removes `.pixi/`, `pixi clean cache` the cache
- On a cluster both move off `$HOME` (block 5)


</div>
</div>

<!--
Links: hard links on most Linux filesystems, copy-on-write clones on macOS (APFS), plain copies across filesystems. That last case matters on clusters where $HOME and scratch are separate filesystems.
-->

---
zoom: 0.85
---

# Pixi compared with conda

| | conda / mamba | pixi |
|---|---|---|
| Where environments live | named, in one central place | inside each project, `.pixi/` |
| How you describe one | `environment.yml`, optional | `pixi.toml`, always present |
| Lock file | extra tool (`conda-lock`) | always, for all platforms |
| Changing it | `conda install` edits the environment | `pixi add` edits the manifest, the environment follows |
| Using it | `conda activate` | `pixi run`, `pixi shell` |
| Base environment | yes | none |
| Packages | conda-forge, … | the same conda-forge packages, plus PyPI |

<div class="mt-6">

Same packages, different workflow: pixi feels like `cargo`, `npm` or `uv`, but for any language.

</div>

---

# The question for this afternoon

Can pixi replace containers? And in what sense?

<div class="mt-6">

| Why people use Apptainer or Docker on HPC | Where we test it with pixi |
|---|---|
| The same software stack on every machine, also next year | lock file · blocks 2–4 |
| It runs on the cluster, MPI across nodes | blocks 5–6 |
| Ship one thing, install nothing on the target | pixi-build, pixi-pack · blocks 7–8 |
| A different OS or system packages than the host has | block 8 |

</div>

<div class="mt-6">

At the end we fill in the verdict: what pixi replaces, and what it does not.

</div>

<!--
Ask the audience who uses containers today and why. Collect the reasons in the chat and map them onto the rows.
-->

---

# Hands-on: first workspace

<div class="opacity-70 -mt-2">Block 2 · in <code>~/pixi-playground</code></div>

<div class="steps" style="grid-template-rows: repeat(4, auto)">
<div><b>2.1</b> Create a workspace</div>
<div><b>2.2</b> Add dependencies, look at the lock file</div>
<div><b>2.3</b> Run things in the environment</div>
<div><b>2.4</b> Inspect: <code>list</code>, <code>tree</code>, <code>search</code>, <code>info</code></div>
<div><b>2.5</b> PyPI packages</div>
<div><b>2.6</b> Versions, removing, updating</div>
<div><b>2.7</b> Tools outside projects</div>
<div><b>2.8</b> Cleaning up</div>
</div>
---

# 2.1 Create a workspace

```bash
mkdir -p ~/pixi-playground && cd ~/pixi-playground
pixi init demo
cd demo
cat pixi.toml
```

The manifest lists the channels (where packages come from) and the platforms (which operating systems the lock file covers).

---

# 2.2 Add dependencies, look at the lock file

```bash
pixi add python numpy matplotlib-base
cat pixi.toml
less pixi.lock          # /numpy- and Enter jumps to numpy, q quits
```

- Which version of numpy did you get? Where is it written down?
- Find the URL and the sha256 of the numpy package in `pixi.lock`
- Where did pixi install it? `ls .pixi/envs/default`

---

# 2.3 Run things in the environment

```bash
pixi run python -c "import numpy; print(numpy.__version__, numpy.__file__)"
pixi shell          # a shell with the environment active; leave with exit
```

- There is no `activate` and no base environment
- `pixi run` first makes sure the environment matches the lock file

---

# 2.4 Inspect

```bash
pixi list
pixi tree numpy            # what numpy pulls in
pixi tree -i libopenblas   # the other way round: why is OpenBLAS here?
pixi search scipy
pixi info
```

---

# 2.5 PyPI packages

Some packages exist only on PyPI. Pixi resolves them together with the conda packages (using uv underneath):

```bash
pixi add --pypi cowsay
pixi run python -c "import cowsay; cowsay.cow('hello from PyPI')"
grep -A2 pypi-dependencies pixi.toml
```

---

# 2.6 Versions, removing, updating

```bash
pixi add "python=3.12.*"     # pin a version range
pixi remove matplotlib-base
pixi update                  # newest versions allowed by pixi.toml, rewrites pixi.lock
pixi upgrade                 # also loosens the constraints in pixi.toml
git diff pixi.toml           # (if you ran git init) see what upgrade changed
```

`update` respects the constraints in `pixi.toml`; `upgrade` changes them.

---

# 2.7 Tools outside projects

```bash
pixi global install ripgrep      # installs rg into its own environment, on your PATH
pixi global list
pixi exec --spec python=3.11 python -V          # one-off command, temporary environment
echo "no installation needed" | pixi exec cowpy
```

---

# 2.8 Cleaning up

```bash
pixi info | grep -i cache    # the shared package cache
pixi clean                   # removes .pixi/ in this workspace
```

- Deleting `.pixi/` is always safe: `pixi install` recreates it from `pixi.lock`
- `pixi clean cache` empties the shared cache. Not now: the next blocks need it

---
layout: section
---

# Organising a project

Block 3 · Tasks, features, environments

---
zoom: 0.84
---

# From code to manifest

<div class="step-tag">step 4.1</div>

You get code and no environment. The build files and imports say what it needs:

| In the code | conda-forge package |
|---|---|
| `cmake_minimum_required(VERSION 3.20)` | `cmake >=3.20` |
| `project(grayscott ... LANGUAGES CXX)` | `cxx-compiler` |
| `find_package(MPI REQUIRED ...)` | `openmpi` |
| we build with `cmake -G Ninja` | `ninja` |
| `import numpy`, `import matplotlib` | `python`, `numpy`, `matplotlib-base` |

```bash
pixi init
pixi add "cmake>=3.20" ninja cxx-compiler openmpi "python=3.13.*" numpy matplotlib-base
```

- `pixi search <name>` checks the conda-forge name
- Forgot `cxx-compiler`? CMake quietly takes the system's `/usr/bin/c++`
- Got an `environment.yml`? `pixi init --import environment.yml` (step 4.7)

---

# Tasks

<div class="step-tag">step 4.3</div>


```toml {all|2-4|5|6-10|all}
[tasks]
configure = { cmd = "cmake -S . -B build -G Ninja",
              inputs = ["CMakeLists.txt"],
              outputs = ["build/CMakeCache.txt"] }
build = { cmd = "cmake --build build", depends-on = ["configure"] }
run = { cmd = "mpirun -n {{ ranks }} build/grayscott --size {{ size }}",
        args = [{ arg = "ranks", default = "2" },
                { arg = "size", default = "256" }],
        depends-on = ["build"],
        description = "Run the solver" }
```


- `inputs` / `outputs`: skipped when nothing changed
- `depends-on`: `pixi run run` configures and builds first
- arguments: `pixi run run 4 512`
- the same task shell on Linux, macOS and Windows


---

# Features and environments

<div class="step-tag">step 4.4</div>


```toml {all|1-4|6-9|11-14|all}
[feature.build.dependencies]      # compilers, only for development
cxx-compiler = "*"
cmake = ">=3.20"
ninja = "*"

[feature.analysis.dependencies]   # plotting
python = "3.13.*"
numpy = ">=2"
matplotlib-base = "*"

[environments]
default  = { features = ["build", "analysis", "test"], solve-group = "main" }
analysis = { features = ["analysis"],                  solve-group = "main" }
prod     = { features = [],                            solve-group = "main" }
```

<v-click>

```bash
pixi run -e analysis plot
```

</v-click>

---
layout: fact
---

# 1.1 GB → 84 MB

<div class="step-tag">step 4.4</div>


`default` vs `prod` for the Gray-Scott solver on macOS

<div class="mt-6 text-base opacity-80">
One solve-group: <code>prod</code> gets exactly the same Open MPI as <code>default</code>,<br>
so what you tested is what runs.
</div>

<!--
Measured with du -sh .pixi/envs/* on osx-arm64. The Gray Scott school reports 1.3 GB -> 142 MB for their stencil code.
-->

---

# Platforms

<div class="step-tag">step 4.6</div>


```toml
[workspace]
platforms = ["linux-64", "linux-aarch64", "osx-arm64", "osx-64"]

[target.linux.tasks]
show = "xdg-open output/v.png"

[target.osx.tasks]
show = "open output/v.png"
```


- The lock file is solved for every listed platform, also the ones you are not on
- `[target.<platform>]`: extra dependencies or tasks for one platform only
- No `win-64` today: there is no Open MPI for Windows on conda-forge


---

# Hands-on: the example becomes a project

<div class="opacity-70 -mt-2">Block 4 · in <code>grayscott/</code></div>

<div class="steps" style="grid-template-rows: repeat(4, auto)">
<div><b>4.1</b> Write <code>pixi.toml</code> from the code</div>
<div><b>4.2</b> Build and run by hand</div>
<div><b>4.3</b> Tasks</div>
<div><b>4.4</b> Features and environments</div>
<div><b>4.5</b> A test</div>
<div><b>4.6</b> More platforms</div>
<div><b>4.7</b> conda in and out</div>
<div><b>4.8</b> Commit</div>
</div>
<div class="catchup">

Behind? `cp ../solutions/4.3/pixi.toml .` &nbsp;·&nbsp; snapshots after 4.1, 4.3, 4.4, 4.5

</div>

---
zoom: 0.9
---

# 4.1 Write pixi.toml from the code (1/2)

You got the code and nothing else. In `grayscott/`:

```bash
pixi init
cat CMakeLists.txt
head -20 scripts/plot.py
```

Start with the build tools only, and try to configure:

```bash
pixi add "cmake>=3.20" ninja
pixi run cmake -S . -B build -G Ninja
```

Read the output before the error:

- `Check for working CXX compiler: /usr/bin/c++`: the system compiler, not one from the environment. Pixi puts the environment first on `PATH`; it does not hide the rest
- `Could NOT find MPI`: nothing provides MPI

---
zoom: 0.9
---

# 4.1 Write pixi.toml from the code (2/2)

Look the missing pieces up, add them, configure again from scratch (CMake remembers the compiler in `build/`):

```bash
pixi search openmpi
pixi add openmpi cxx-compiler
rm -rf build
pixi run cmake -S . -B build -G Ninja     # compiler and MPI now from .pixi/envs/default
```

The Python side:

```bash
pixi add "python=3.13.*" numpy matplotlib-base
cat pixi.toml
```

- 3.13 is the Python the setup check already downloaded
- `matplotlib-base` is matplotlib without the GUI toolkits, enough for PNG files

<div class="catchup">

Behind? `cp ../solutions/4.1/pixi.toml .`

</div>

---

# 4.2 Build and run by hand

```bash
pixi run cmake --build build
pixi run mpirun -n 2 build/grayscott --size 256 --steps 10000
pixi run python scripts/plot.py output/v.npy
open output/v.png        # macOS; on Linux: xdg-open
```

Four long commands nobody will remember. Tasks next.

---
zoom: 0.82
---

# 4.3 Tasks

```bash
pixi task add configure "cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release"
pixi task add build "cmake --build build" --depends-on configure
pixi task add plot "python scripts/plot.py output/v.npy"
pixi task list
```

Then add `run` by hand, under `[tasks]` in `pixi.toml`:

```toml
run = { cmd = "mpirun --oversubscribe -n {{ ranks }} build/grayscott --size {{ size }} --steps {{ steps }}",
        args = [{ arg = "ranks", default = "2" }, { arg = "size", default = "256" },
                { arg = "steps", default = "10000" }],
        depends-on = ["build"] }
```

```bash
pixi run run            # defaults
pixi run run 4 512      # 4 ranks, 512 x 512
```

<div class="catchup">

Behind? `cp ../solutions/4.3/pixi.toml .`

</div>

---
zoom: 0.9
---

# 4.4 Features and environments

<div class="grid grid-cols-2 gap-6">
<div>

Split the dependencies:

- `[feature.build.dependencies]`: `cxx-compiler`, `cmake`, `ninja`
- `[feature.analysis.dependencies]`: `python`, `numpy`, `matplotlib-base`
- `[dependencies]`: only `openmpi`

Move each task into the feature it needs: `[feature.build.tasks]`, `[feature.analysis.tasks]`.

</div>
<div>

```toml
[environments]
default = { features = ["build", "analysis"],
            solve-group = "main" }
analysis = { features = ["analysis"],
             solve-group = "main" }
prod = { features = [], solve-group = "main" }
```

</div>
</div>

```bash
pixi install --all
pixi list -e prod          # how much smaller is it?
pixi run -e analysis plot
du -sh .pixi/envs/*
```

<div class="catchup">

Behind? `cp ../solutions/4.4/pixi.toml .`

</div>

---

# 4.5 A test

1 rank and 4 ranks must give bitwise identical output.

- Add a `test` feature with `python` and `numpy`, and add it to `default`
- Two tasks that run the solver with 1 and with 4 ranks, writing `output/test-1.npy` and `output/test-4.npy`
- A `test` task that compares them with `python scripts/compare.py output/test-1.npy output/test-4.npy`, with `depends-on` on both runs

```bash
pixi run test        # max |a - b| = 0.000e+00
```

<div class="catchup">

Behind? `cp ../solutions/4.5/pixi.toml .`

</div>

---

# 4.6 More platforms

```bash
wc -l pixi.lock
pixi workspace platform add linux-64 linux-aarch64 osx-arm64 osx-64
wc -l pixi.lock
```

- The lock file now pins every package for all four platforms
- A colleague on a Linux cluster gets exactly the versions you tested on your Mac
- No `win-64`: conda-forge has no Open MPI for Windows

---
zoom: 0.9
---

# 4.7 conda in and out

A colleague has used this code with conda all along: `environment.yml` is their environment. Pixi can start from it instead of from the code. Compare with what you wrote in 4.1:

```bash
mkdir /tmp/import && cd /tmp/import
pixi init --import ~/pixi-workshop/grayscott/environment.yml    # adapt the path
cat pixi.toml
cd -
```

The other direction, for colleagues who stay with conda:

```bash
pixi workspace export conda-environment -e default > environment-dev.yml
pixi workspace export conda-explicit-spec -e prod --platform linux-64 explicit/
```

---

# 4.8 Commit

```bash
git add pixi.toml pixi.lock
```

- `pixi.toml` and `pixi.lock` belong in git
- `.pixi/` does not; `pixi init` put it in `.gitignore`

---
layout: section
---

# Pixi on a cluster

Block 5 · `--locked`, `--frozen`, `--as-is`

---

# Hands-on: pixi on a cluster

<div class="opacity-70 -mt-2">Block 5 · 5.1–5.2 on the laptop, 5.3 on NSC</div>

<div class="steps" style="grid-template-rows: repeat(2, auto)">
<div><b>5.1</b> <code>--locked</code>, <code>--frozen</code>, <code>--as-is</code></div>
<div><b>5.2</b> Keep environments out of <code>$HOME</code></div>
<div><b>5.3</b> On the NSC cluster: install from the lock file, run with 8 ranks</div>
<div><b>5.4</b> Jupyter on a cluster</div>
</div>
<div class="catchup">

On NSC: `ssh pixiXXX@nsc-login.ijs.si`, then `salloc --reservation=delavnica --nodes=1 --ntasks=8 --time=02:00:00`

</div>

---

# 5.1 --locked, --frozen, --as-is

| Option | Checks lock vs manifest | Solves, rewrites lock | Installs | Use for |
|---|---|---|---|---|
| (none) | yes | yes | yes | everyday development |
| `--locked` | yes, error if out of date | no | yes | reproducing a committed environment |
| `--frozen` | no | no | yes, exactly `pixi.lock` | machines where nothing may change |
| `--as-is` | no | no | no | running an installed environment without touching it |

The same as environment variables: `PIXI_LOCKED=1`, `PIXI_FROZEN=1`.

---
zoom: 0.88
---

# 5.1 Try it: manifest and lock file disagree

```bash
mkdir /tmp/lockdemo && cd /tmp/lockdemo
pixi init && pixi add tree
```

Add jq to the manifest by hand, without relocking:

```bash
echo 'jq = "*"' >> pixi.toml   # [dependencies] is the last table
pixi install --locked          # refuses: the lock file no longer matches
pixi install --frozen          # installs exactly pixi.lock: no jq
pixi run --frozen which jq     # if anything: the system's jq
pixi run --as-is tree -L 1 .pixi
pixi install                   # relocks and installs, now jq is there
pixi run which jq
```

A package missing from the environment can quietly come from the system: pixi puts the environment first on `PATH`, it does not remove the rest.

---
zoom: 0.84
---

# 5.2 Keep environments out of $HOME

Environments are many small files and quickly fill a home quota. On a cluster, add to `~/.bashrc`:

```bash
PIXI_BASE=/path/to/project/storage/$USER       # adapt
export PIXI_HOME=$PIXI_BASE/pixi               # pixi itself, global tools, global config
export PIXI_CACHE_DIR=$PIXI_BASE/pixi-cache    # downloads; same filesystem as the envs
export PATH=$PIXI_HOME/bin:$PATH
```

and once: `pixi config set --global detached-environments "$PIXI_BASE/pixi-envs"`

Try detached environments in one project, on your laptop:

```bash
cd grayscott
pixi config set --local detached-environments /tmp/pixi-envs
pixi install && ls /tmp/pixi-envs
pixi config unset --local detached-environments
```

---
zoom: 0.9
---

# 5.3 On the NSC cluster: log in

You get a user name (`pixi001` to `pixi030`) and a password. Two nodes, `nsc-lou001` and `nsc-lou002`, are reserved for us.

```bash
ssh pixiXXX@nsc-login.ijs.si               # your own number
salloc --reservation=delavnica --nodes=1 --ntasks=8 --time=02:00:00
hostname                                   # nsc-lou001 or nsc-lou002
```

Everything from here on runs on that node. First pixi and the material:

```bash
curl -fsSL https://pixi.sh/install.sh | PIXI_VERSION=v0.84.2 bash
export PATH=$HOME/.pixi/bin:$PATH
git clone https://github.com/leskovec/pixi-workshop.git
cd pixi-workshop/grayscott
```

---
zoom: 0.84
---

# 5.3 On the NSC cluster: install and run

The manifest and lock file come from git. The lock file was solved on a laptop; `--locked` installs exactly that, or refuses.

```bash
cp ../solutions/7.2/pixi.{toml,lock} .
pixi install --locked -e prod              # download, and build the solver once
```

Activate once, without checking or installing anything, and run on all 8 cores:

```bash
bash                                       # a sub-shell, so the activation is easy to leave
eval "$(pixi shell-hook -e prod --as-is)"
which grayscott mpirun
mpirun -n 8 grayscott --size 4096 --steps 2000 --output output/bundled.npy
exit                                       # back to the plain shell on the node
```

`exit` without the sub-shell would end the whole `salloc` session.

---

# 5.4 Jupyter on a cluster

Pixi has no environment registry, so Jupyter does not find environments by itself. Add `ipykernel` and register the environment as a kernel:

```bash
pixi add -f analysis ipykernel
pixi run -e analysis python -m ipykernel install --user --name grayscott-analysis
```

---
layout: section
---

# MPI: bundled vs system

Block 6

---

# How hardware-specific is conda software?

| Layer | Example | Who takes care of it |
|---|---|---|
| Architecture | x86-64, ARM | **pixi**: one package list per platform in `pixi.lock` |
| CPU model | AVX2, AVX-512, Apple M | **the libraries**, at runtime: OpenBLAS, MKL and numpy pick their kernels when they load |
| Network | InfiniBand, Slingshot | **MPI**: the bundled one, or the cluster's own. That is this block |

<div class="mt-8 grid grid-cols-2 gap-6">
<div>

Prefer a different BLAS? One line:

```toml
[target.linux-64.dependencies]
libblas = { version = "*", build = "*mkl" }
```

</div>
<div class="text-sm">

| Platform | BLAS on conda-forge |
|---|---|
| linux-64 | openblas, mkl, blis, netlib |
| linux-aarch64 | openblas, nvpl, blis, netlib |
| macOS | openblas, accelerate, blis, netlib |

</div>
</div>

<!--
conda-forge builds OpenBLAS with DYNAMIC_ARCH=1: on this Mac it picks the VORTEX (Apple M1) kernels, on cluster nodes Haswell, SkylakeX, Zen, ... Check with: pixi exec --spec numpy --spec threadpoolctl python -c "import numpy, threadpoolctl; print(threadpoolctl.threadpool_info())"
Everything that links BLAS (numpy, scipy, your code) follows the libblas choice. MKL also dispatches at runtime.
The network is the one layer pixi cannot just download the right thing for. Hence bundled vs system MPI.
-->

---
zoom: 0.9
---

# Two ways to get MPI

| Route | In `pixi.toml` | Pros | Cons |
|---|---|---|---|
| bundled | `openmpi = ">=5.0,<6"` from conda-forge | identical on laptop and cluster, fully locked; includes UCX for InfiniBand | the fastest network settings need testing on each cluster |
| system | `openmpi = { version = "5.0.*", build = "external_*" }` plus `module load` | uses the MPI the centre tuned for its network | not self-contained; version must match the module; Linux only |

- `external_*` is a placeholder package: no MPI library inside, it only says "MPI is provided"
- mpi4py and anything else using MPI is built against conda-forge's headers and runs with the system library
- MPICH, Intel MPI, Cray MPICH: `mpich = { version = "4.*", build = "external_*" }`

---

# Hands-on: MPI, bundled vs system

<div class="opacity-70 -mt-2">Block 6 · in <code>grayscott/</code>, from 4.5</div>

<div class="steps" style="grid-template-rows: repeat(3, auto)">
<div><b>6.1</b> MPI only where it is needed</div>
<div><b>6.2</b> mpi4py and a <code>hello</code> task</div>
<div><b>6.3</b> The solver with more ranks</div>
<div><b>6.4</b> A cluster environment with the system MPI</div>
<div><b>6.5</b> On the NSC cluster: system MPI vs bundled</div>
</div>
<div class="catchup">

Behind? `cp ../solutions/6.1/pixi.toml .` &nbsp;·&nbsp; snapshots after 6.1, 6.2, 6.4

</div>

---

# 6.1 MPI only where it is needed

In `grayscott/`, from the end of block 4. `openmpi` is in `[dependencies]`, so even the plotting environment has it. Move it into a feature:

```toml
[feature.mpi-bundled.dependencies]
openmpi = ">=5.0,<6"
```

and add `mpi-bundled` to the `default` and `prod` environments, not to `analysis`.

```bash
pixi list -e analysis | grep -i mpi     # nothing
```

<div class="catchup">

Behind? `cp ../solutions/6.1/pixi.toml .`

</div>

---
zoom: 0.92
---

# 6.2 mpi4py

```toml
[feature.mpi-python.dependencies]
python = "3.13.*"
mpi4py = ">=4"

[feature.mpi-python.tasks]
hello = { cmd = "mpirun --oversubscribe -n {{ ranks }} python scripts/hello_mpi.py",
          args = [{ arg = "ranks", default = "4" }] }
```

Add `mpi-python` to the `default` environment.

```bash
pixi run hello
pixi run hello 8
```

<div class="catchup">

Behind? `cp ../solutions/6.2/pixi.toml .`

</div>

---

# 6.3 The solver with more ranks

```bash
pixi run run 1 1024 2000
pixi run run 4 1024 2000
pixi run test                            # 1 rank vs 4 ranks, bitwise identical
pixi run build/grayscott --version       # which MPI library is it linked to?
```

---
zoom: 0.88
---

# 6.4 A cluster environment with the system MPI

```toml
[feature.mpi-system]
platforms = ["linux-64", "linux-aarch64"]
channels = [{ channel = "conda-forge/label/mpi-external", priority = 1 }]

[feature.mpi-system.dependencies]
openmpi = { version = "5.0.*", build = "external_*" }

[environments]
cluster = { features = ["mpi-system", "build", "mpi-python", "test"] }
```

```bash
pixi lock
pixi list -e cluster --platform linux-64 | grep -E "openmpi|mpi4py"
```

`cluster` has no `solve-group`: its `openmpi` differs from the one in `default`, so it is solved on its own.

<div class="catchup">

Behind? `cp ../solutions/6.4/pixi.toml .`

</div>

---
zoom: 0.8
---

# 6.5 On the NSC cluster: system MPI vs bundled

On the node from 5.3, in `pixi-workshop/grayscott`. The manifest and lock file already contain the `cluster` environment.

```bash
module load OpenMPI/5.0.x                  # the site's Open MPI; module avail openmpi lists them
pixi install --locked -e cluster
pixi run -e cluster --frozen build         # compiled against the site's MPI
pixi run -e cluster --frozen build/grayscott --version
pixi run -e cluster --frozen mpirun -n 8 build/grayscott --size 4096 --steps 2000 --output output/system.npy
```

The bundled run from 5.3, to compare:

```bash
pixi run -e prod --frozen mpirun -n 8 grayscott --size 4096 --steps 2000 --output output/bundled.npy
```

`--version` names the site's Open MPI for `cluster`, conda-forge's for `prod`. Compare the Mcell-updates/s.

---
layout: section
---

# Packaging the solver with pixi-build

Block 7

---

# Hands-on: packaging with pixi-build

<div class="opacity-70 -mt-2">Block 7 · in <code>grayscott/</code>, from 6.4</div>

<div class="steps" style="grid-template-rows: repeat(3, auto)">
<div><b>7.1</b> Turn the workspace into a package</div>
<div><b>7.2</b> Use the package in the <code>prod</code> environment</div>
<div><b>7.3</b> Build the package file</div>
<div><b>7.4</b> Use it from another project (<code>analysis/</code>)</div>
<div><b>7.5</b> Optional: optimise for your CPU</div>
</div>
<div class="catchup">

Behind? `cp ../solutions/7.1/pixi.toml .` &nbsp;·&nbsp; snapshots after 7.1, 7.2, 7.4 (in <code>analysis/</code>)

</div>
---
zoom: 0.82
---

# 7.1 Turn the workspace into a package

Until now everyone compiled the solver in a development environment. As a conda package it installs like numpy: no compilers, no `build/`, a fixed version.

```bash
pixi workspace preview add pixi-build       # still a preview feature in pixi 0.84
```

```toml
[package]
name = "grayscott"
version = "0.1.0"

[package.build]
backend = { name = "pixi-build-cmake", version = "*" }

[package.host-dependencies]
openmpi = ">=5.0,<6"
```

- The CMake backend runs `cmake`, `cmake --build`, `cmake --install` with conda-forge compilers
- Host dependencies are compiled and linked against; Open MPI's run-exports add the run dependency

<div class="catchup">

Behind? `cp ../solutions/7.1/pixi.toml .`

</div>

---
zoom: 0.82
---

# 7.2 Use the package in the prod environment

```toml
[feature.solver.dependencies]
grayscott = { path = "." }

[feature.solver.tasks]
simulate = { cmd = "mpirun --oversubscribe -n {{ ranks }} grayscott --size {{ size }} --steps {{ steps }}",
             args = [{ arg = "ranks", default = "2" }, { arg = "size", default = "256" },
                     { arg = "steps", default = "10000" }] }
```

and change `prod` to `{ features = ["mpi-bundled", "solver"], solve-group = "main" }`.

```bash
pixi install -e prod             # builds the package, then installs it
pixi list -e prod                # grayscott, openmpi, no compilers
pixi run -e prod which grayscott
pixi run -e prod simulate 4
```

<div class="catchup">

Behind? `cp ../solutions/7.2/pixi.toml .`

</div>

---

# 7.3 Build the package file

```bash
pixi build -o dist
ls dist/                         # grayscott-0.1.0-<hash>_0.conda
pixi exec rattler-build package inspect dist/grayscott-*.conda
```

Look for the run dependencies: `openmpi >=5.0.x,<6.0a0` was added by Open MPI's run-exports. Nobody wrote it.

---
zoom: 0.88
---

# 7.4 Use it from another project

`analysis/scan.py` runs the solver for six $(F, k)$ pairs and plots a gallery. It only needs `grayscott` on `PATH`.

```bash
cd ../analysis
pixi init
pixi workspace preview add pixi-build
pixi add python numpy matplotlib-base
```

In `pixi.toml`, under `[dependencies]`: `grayscott = { path = "../grayscott" }`

```bash
pixi task add scan "python scan.py --ranks 4"
pixi run scan
open data/gallery.png
```

The analysis project does not know or care how the solver is built.

<div class="catchup">

Behind? `cp ../solutions/7.4/pixi.toml .   (in analysis/)`

</div>

---

# 7.5 Optional: optimise for your CPU


Compiler flags for your own code go into the build configuration:

```toml
[package.build.config]
extra-args = ["-DCMAKE_CXX_FLAGS=-march=x86-64-v3"]
```

<div class="grid grid-cols-2 gap-8 mt-4">
<div>

| Flag | Runs on |
|---|---|
| `-march=x86-64-v3` | x86 with AVX2: Haswell (2013) and newer, AMD Zen |
| `-march=native` | the build machine's CPU, and newer |
| `-mcpu=native` | the same, on ARM and Apple |

</div>
<div>


- `pixi run -e prod simulate 1` rebuilds the package and runs it: compare Mcell-updates/s
- On a MacBook (M1 Pro): about 490 with and without. The stencil is limited by memory, not by arithmetic
- The flag travels with the package: built for a new CPU, it fails with *illegal instruction* on an older node
- Remove the lines again before block 8


</div>
</div>

<!--
One flag per -DCMAKE_CXX_FLAGS value: "-O3 -mcpu=native" breaks, the backend splits it at the space. Setting CMAKE_CXX_FLAGS replaces conda's default flags; the Release build still adds -O3.
Measured on M1 Pro, 256 x 256, 10000 steps, 1 rank, three runs each: 463-496 Mcell/s with -mcpu=native, 481-504 without.
-->

---

# Outlook: not practised today

- Publish the `.conda` file to a channel (`pixi publish`, prefix.dev, S3, Artifactory) and replace `path = "../grayscott"` with a version requirement
- Build variants: one package per MPI flavour or CPU instruction set, instead of one flag for everyone
- Cross-compiling, e.g. `pixi build --target-platform linux-aarch64`

---
layout: section
---

# Shipping with pixi-pack

Block 8

---

# Hands-on: shipping with pixi-pack

<div class="opacity-70 -mt-2">Block 8 · in <code>grayscott/</code>, from 7.2</div>

<div class="steps" style="grid-template-rows: repeat(3, auto)">
<div><b>8.1</b> Pack the <code>prod</code> environment</div>
<div><b>8.2</b> Unpack and run without pixi</div>
<div><b>8.3</b> A single self-extracting installer</div>
<div><b>8.4</b> Packing for another platform</div>
<div><b>8.5</b> Can pixi replace a container?</div>
</div>
<div class="catchup">

Behind? `cp ../solutions/7.2/pixi.toml .`

</div>
---

# 8.1 Pack the prod environment

In `grayscott/` with the block 7 manifest.

```bash
pixi global install pixi-pack pixi-unpack
pixi-pack -e prod                      # -> environment.tar (about 25 MB)
```

pixi-pack downloads every package of `prod` from the lock file, builds `grayscott` from source with pixi-build, and puts everything into one tar file.

<div class="catchup">

Behind? `cp ../solutions/7.2/pixi.toml .`

</div>

---
zoom: 0.88
---

# 8.2 Unpack and run without pixi

```bash
mkdir /tmp/unpacked && cp environment.tar /tmp/unpacked && cd /tmp/unpacked
pixi-unpack environment.tar            # -> env/ and activate.sh
ls

# a clean shell, pixi not on PATH
env -i HOME=$HOME PATH=/usr/bin:/bin bash -c \
  'source activate.sh && mpirun --oversubscribe -n 2 grayscott --size 256 --output v.npy'
```

On a cluster it runs the same way.

---

# 8.3 A single self-extracting installer

```bash
cd -                                   # back to grayscott/
pixi-pack -e prod --create-executable -o grayscott-installer.sh
mkdir /tmp/inst && cd /tmp/inst && /path/to/grayscott-installer.sh
```

The installer contains `pixi-unpack` itself, so the target machine needs nothing but a shell.

---

# 8.4 Packing for another platform

```bash
pixi-pack -e prod -p linux-64 -o grayscott-linux.tar
```

- Pure conda-forge environments pack for any platform
- Ours contains a package built from source, and C++ is not cross-compiled from macOS to Linux here, so this fails on a Mac
- Pack on a Linux machine (WSL2, a login node, CI), or build the `.conda` there with `pixi build` and add it with `--inject grayscott-0.1.0-*.conda`

---
zoom: 0.92
---

# 8.5 Can pixi replace containers? The verdict


<div class="grid grid-cols-2 gap-10 mt-2">
<div>

### Pixi replaces them for


- **the same stack everywhere**: `pixi.lock` pins every package by hash, for Linux x86 and ARM and macOS at once
- **shipping**: `pixi-pack` gives one tar or a self-extracting installer; no runtime, no root, no image build
- **MPI codes**: the bundled MPI or the cluster's own, no matching of container and host MPI
- **daily work**: change a dependency, and the next `pixi run` catches up in seconds


</div>
<div>

### Containers still win for


- **a different OS than the host**: pixi uses the host's kernel and glibc, and needs glibc ≥ `__glibc` in `pixi.lock` (2.28 here)
- **software outside conda-forge and PyPI**: OS packages, vendor binaries built for one distribution
- **services and image-only platforms**: databases, Kubernetes, cloud batch


</div>
</div>

<!--
glibc 2.28 = RHEL/Rocky 8 and newer. A cluster on an older OS is a case for a container.
Ask: which of the right-hand rows matter for your own work?
-->

---
layout: section
---

# For fun

Block 9 · the same tool outside HPC

---

# Hands-on: for fun

<div class="opacity-70 -mt-2">Block 9 · demo</div>

<div class="steps" style="grid-template-rows: repeat(3, auto)">
<div><b>9.1</b> ROOT, the particle physics toolkit</div>
<div><b>9.2</b> samtools from bioconda, without installing it</div>
<div><b>9.3</b> R with ggplot2</div>
<div><b>9.4</b> ROS 2 with RViz</div>
<div><b>9.5</b> Mojo</div>
</div>
---

# 9.1 ROOT, the particle physics toolkit

```bash
pixi global install root
root -l -b -q -e 'TF1 f("f","sin(x)/x",0,10); printf("integral = %g\n", f.Integral(0,10));'
```

---

# 9.2 A bioconda tool, without installing it

```bash
pixi exec -c conda-forge -c bioconda samtools --version
```

---
zoom: 0.8
---

# 9.3 R

```bash
mkdir r-demo && cd r-demo
pixi init && pixi add r-base r-ggplot2
pixi run Rscript -e 'library(ggplot2); ggsave("cars.png", ggplot(mtcars, aes(wt, mpg)) + geom_point())'
```

---

# 9.4 ROS 2 with RViz

Needs a desktop session.

```bash
mkdir ros-demo && cd ros-demo
pixi init -c robostack-jazzy -c conda-forge
pixi add ros-jazzy-desktop
pixi run rviz2
```

---

# 9.5 Mojo

```bash
mkdir mojo-demo && cd mojo-demo
pixi init -c https://conda.modular.com/max -c conda-forge
pixi add mojo
printf 'def main():\n    print("Hello from Mojo")\n' > hello.mojo
pixi run mojo hello.mojo
```

---
layout: center
class: text-center
---

# Thank you

<div class="mt-8 text-lg">

Material: [github.com/leskovec/pixi-workshop](https://github.com/leskovec/pixi-workshop)

</div>

<div class="mt-8 text-sm opacity-80">

[pixi documentation](https://pixi.prefix.dev/latest/) · [Carpentries Incubator: Introduction to Pixi](https://carpentries-incubator.github.io/pixi-introduction/)

</div>

---
layout: section
---

# Backup

---

# Backup: CPU-optimised software

<div class="grid grid-cols-2 gap-8 text-sm">
<div>

**Is conda-forge software optimised for our cluster?**
Built for a baseline CPU per architecture. Performance libraries choose CPU-specific kernels at runtime: conda-forge's OpenBLAS is built with `DYNAMIC_ARCH=1`, MKL dispatches by itself.

**Can I rebuild OpenBLAS with my own flags?**
In principle: the conda-forge recipe with pixi-build's rattler-build backend, as a source dependency. Long build, only runs on that CPU, and it must replace `libopenblas` everywhere. Measure first; the runtime dispatch is usually close.

</div>
<div>

**Can I mark my package as "needs AVX2"?**
conda-forge has `_x86_64-microarch-level`, but pixi solves `linux-64` for a generic x86-64 CPU, so locking fails (pixi 0.84). Document the requirement instead, or build on the oldest node type.

**When Spack or EasyBuild instead?**
When the whole stack must be compiled for your CPU and site, including libraries without runtime dispatch. Pixi installs prebuilt binaries and does not do that.

</div>
</div>
