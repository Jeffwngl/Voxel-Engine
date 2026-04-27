#!/bin/bash

set -e

echo "Installing dependencies."

sudo apt update

sudo apt install -y \
    libgl1-mesa-dev \
    libwayland-dev \
    wayland-protocols \
    libxkbcommon-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    pkg-config

echo "Done."