# Block 9: for fun (16:50–17:00)

A few things the same tool does outside HPC. Each is one or two commands.

## ROOT, the particle physics toolkit

```bash
pixi global install root
root -l -b -q -e 'TF1 f("f","sin(x)/x",0,10); printf("integral = %g\n", f.Integral(0,10));'
```

## A bioinformatics tool from bioconda, without installing it

```bash
pixi exec -c conda-forge -c bioconda samtools --version
```

## R

```bash
mkdir r-demo && cd r-demo
pixi init && pixi add r-base r-ggplot2
pixi run Rscript -e 'library(ggplot2); ggsave("cars.png", ggplot(mtcars, aes(wt, mpg)) + geom_point())'
```

## ROS 2 with RViz (needs a desktop session)

```bash
mkdir ros-demo && cd ros-demo
pixi init -c robostack-jazzy -c conda-forge
pixi add ros-jazzy-desktop
pixi run rviz2
```

## Mojo

```bash
mkdir mojo-demo && cd mojo-demo
pixi init -c https://conda.modular.com/max -c conda-forge
pixi add mojo
echo 'def main():
    print("Hello from Mojo")' > hello.mojo
pixi run mojo hello.mojo
```

## Where to continue

- pixi documentation: https://pixi.prefix.dev/latest/
- Carpentries Incubator, Introduction to Pixi: https://carpentries-incubator.github.io/pixi-introduction/
- Carpentries Incubator, Reproducible ML Workflows (CUDA, containers, Slurm): https://carpentries-incubator.github.io/reproducible-ml-workflows/
- prefix.dev blog tutorials: https://prefix.dev/blog/tag/tutorial
- Gray Scott HPC school material: https://cta-lapp.pages.in2p3.fr/COURS/GRAY_SCOTT_REVOLUTIONS/GrayScott2026/
