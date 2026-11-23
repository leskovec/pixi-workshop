# Pixi settings for a cluster. Adapt the paths, then add
#   source /path/to/pixi-env.sh
# to ~/.bashrc. Keep everything on project or scratch storage, not in $HOME:
# environments are many small files and quickly fill a home quota.

PIXI_BASE=/path/to/project/storage/$USER     # adapt

# pixi itself, global tools (`pixi global install`) and the global config
export PIXI_HOME=$PIXI_BASE/pixi
# downloaded packages; on the same filesystem as the environments so that
# pixi can hardlink files instead of copying them
export PIXI_CACHE_DIR=$PIXI_BASE/pixi-cache
export PATH=$PIXI_HOME/bin:$PATH

# One-time setup, run by hand after sourcing this file:
#
#   curl -fsSL https://pixi.sh/install.sh | PIXI_VERSION=v0.84.2 bash
#   pixi config set --global detached-environments "$PIXI_BASE/pixi-envs"
#
# With detached-environments set, `pixi install` puts each project's
# environments under $PIXI_BASE/pixi-envs instead of <project>/.pixi/envs,
# so projects can live in $HOME while environments do not.
