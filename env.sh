#!/bin/zsh
#
# Sets up the shell environment to run gundamFitter, then cd's into this
# tutorial repo. Run with: source env.sh
#
# The directory layout and variable names below follow GUNDAM's own
# "Getting Started" guide verbatim:
#   https://gundam-organization.github.io/gundam/GettingStarted.html
# If you installed GUNDAM by following that guide as-is, this should work
# with no edits. If your install differs (different WORK_DIR, a custom
# CMAKE_INSTALL_PREFIX, etc.), edit the four exports below to match.

# Load ROOT environment (assumes ROOT was installed via Homebrew; if not,
# source your own ROOT setup script here instead).
source $(brew --prefix root)/bin/thisroot.sh

# GUNDAM directory layout, per the official Getting Started guide.
# EDIT IF YOUR INSTALL DIFFERS FROM THE GUIDE'S DEFAULTS:
export WORK_DIR="$HOME/Documents/Work"
export INSTALL_DIR="$WORK_DIR/Install"
export BUILD_DIR="$WORK_DIR/Build"
export REPO_DIR="$WORK_DIR/Repositories"

# Add GUNDAM executable and libraries (the official guide's "Adding GUNDAM
# to PATH" step). DYLD_LIBRARY_PATH is added alongside LD_LIBRARY_PATH
# since this is macOS — the official guide only documents LD_LIBRARY_PATH.
export PATH="$INSTALL_DIR/gundam/bin:$PATH"
export LD_LIBRARY_PATH="$INSTALL_DIR/gundam/lib:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH="$INSTALL_DIR/gundam/lib:$DYLD_LIBRARY_PATH"

# cd into this tutorial repo, wherever it was cloned — no editing needed,
# this resolves to the directory this script itself lives in.
cd "$(dirname "${0:A}")"

echo "ROOT: $(which root)"
echo "GUNDAM: $(which gundamFitter)"
echo "Current directory: $(pwd)"
