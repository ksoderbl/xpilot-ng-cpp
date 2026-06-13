#!/usr/bin/env bash
# cmake-build.sh

set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"

# Let's not build the SDL client for now.
cmake -S . -B build \
  -DXPILOT_BUILD_SDL_CLIENT=OFF \
  -DXPILOT_BUILD_X11_CLIENT=ON \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build -j"$(nproc)"
cmake --build "$BUILD_DIR" -j"$(nproc)"
#cmake --build "$BUILD_DIR" -j"$(nproc)" --verbose
