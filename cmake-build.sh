#!/usr/bin/env bash
# cmake-build.sh

set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"

# Let's not build the SDL client for now.
cmake -S . -B "$BUILD_DIR" -DXPILOT_BUILD_SDL_CLIENT=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j"$(nproc)"
#cmake --build "$BUILD_DIR" -j"$(nproc)" --verbose
