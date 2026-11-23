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

- Welcome, setup check
- Environment management on HPC
- *Hands-on:* first workspace
- Tasks, features, environments
- *Hands-on:* the example becomes a project

*Break*

</div>
<div>

**Part 2: practical workflow**

- Pixi on a cluster, `--locked` vs `--frozen`
- *Hands-on:* MPI, bundled vs system
- *Hands-on:* packaging with pixi-build
- *Hands-on:* shipping with pixi-pack
- For fun, wrap-up

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

<div class="mt-8">

| | Term | Meaning |
|---|---|---|
| <span style="color:#2b8cbe">■</span> | $D\,\nabla^2$ | diffusion: each chemical spreads to its neighbours; $D_u = 0.16$, $D_v = 0.08$ |
| <span style="color:#b5367a">■</span> | $u v^2$ | the reaction $U + 2V \to 3V$: one U and two V meet, U turns into V |
| <span style="color:#e66101">■</span> | $F(1-u)$ | feed: U is topped up towards concentration 1 |
| <span style="color:#e66101">■</span> | $(F+k)\,v$ | removal: V is washed out at rate $F$ and decays at rate $k$ |

</div>

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
| on a cluster with Slurm | block 5 |
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

```bash
git clone <repository URL>
cd pixi-workshop-examples/setup
pixi run check
```

```text
rank 0 of 2: setup OK
rank 1 of 2: setup OK
```

<div class="mt-8">

The first run downloads compilers, Open MPI and Python, so it takes a few minutes.

On Windows, run this inside WSL2: the workshop example uses MPI, and conda-forge has no Open MPI for Windows.

Stuck? Post the error in the chat. Every hands-on block has a complete solution in `solutions/`, so you can always copy it in and continue.

</div>

---
layout: section
---

# Environment management on HPC

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
layout: center
class: text-center
---

# Hands-on: first workspace

`exercises/02-first-workspace.md`

---
layout: section
---

# Organising a project

Tasks, features, environments

---

# Tasks

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
layout: center
class: text-center
---

# Hands-on: the example becomes a project

`exercises/04-project.md`

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
layout: section
---

# Packaging the solver with pixi-build

Block 7

---

# Optional: optimise for your CPU

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


</div>
</div>

<!--
One flag per -DCMAKE_CXX_FLAGS value: "-O3 -mcpu=native" breaks, the backend splits it at the space. Setting CMAKE_CXX_FLAGS replaces conda's default flags; the Release build still adds -O3.
Measured on M1 Pro, 256 x 256, 10000 steps, 1 rank, three runs each: 463-496 Mcell/s with -mcpu=native, 481-504 without.
-->

---
layout: section
---

# Shipping with pixi-pack

Block 8

---
zoom: 0.92
---

# Can pixi replace containers? The verdict

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
layout: center
class: text-center
---

# Thank you

<div class="mt-8 text-lg">

Material: `<repository URL>`

Questions later: [](mailto:)

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
