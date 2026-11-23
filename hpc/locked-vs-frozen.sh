#!/usr/bin/env bash
# Demonstrates what --locked, --frozen and --as-is do when pixi.toml and
# pixi.lock disagree. Run it from an empty scratch directory:
#
#   mkdir /tmp/lockdemo && cd /tmp/lockdemo && bash /path/to/locked-vs-frozen.sh

set -u

step() { printf '\n\033[1m$ %s\033[0m\n' "$*"; "$@"; echo "(exit code $?)"; }

pixi init --channel conda-forge . > /dev/null
step pixi add tree

echo
echo "### Edit pixi.toml by hand: add jq, but do not relock."
sed -i.bak 's/^\[dependencies\]$/[dependencies]\njq = "*"/' pixi.toml && rm pixi.toml.bak
grep -A3 '^\[dependencies\]' pixi.toml

echo
echo "### --locked: refuses, because the lock file no longer matches the manifest."
step pixi install --locked

echo
echo "### --frozen: installs exactly what pixi.lock says (no jq), lock file untouched."
echo "### If jq is found at all, it is the system one: pixi prepends the environment to PATH."
step pixi install --frozen
step pixi run --frozen which jq

echo
echo "### --as-is: like --frozen, and does not even touch the installed environment."
step pixi run --as-is tree -L 1 .pixi

echo
echo "### default: relocks and installs, now jq is there."
step pixi install
step pixi run which jq
