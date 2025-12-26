#!/bin/bash
# Qt 6 Build Script for Pngyu
# This script creates a separate build directory to keep source clean

set -e

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

BUILD_DIR="build"
PROJECT_FILE="Pngyu.pro"

echo -e "${YELLOW}Building Pngyu with Qt 6...${NC}"

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
fi

# Change to build directory
cd "$BUILD_DIR"

# Run qmake
echo "Running qmake..."
qmake ../"$PROJECT_FILE" -spec macx-clang CONFIG+=x86_64 CONFIG+=sdk_no_version_check

# Build
echo "Building..."
make -j$(sysctl -n hw.ncpu)

echo -e "${GREEN}Build complete!${NC}"
echo "Application: $BUILD_DIR/Pngyu.app"
echo "To run: open $BUILD_DIR/Pngyu.app"
