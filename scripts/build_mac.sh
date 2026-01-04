#!/bin/bash
# Build script for macOS

set -e

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

# Build type (default: Release)
BUILD_TYPE="${BUILD_TYPE:-Release}"
BUILD_DIR="$PROJECT_ROOT/build/$BUILD_TYPE"

echo "Building Pngyu ($BUILD_TYPE)..."
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring CMake..."
cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build
echo ""
echo "Building..."
cmake --build .

echo ""
echo "✓ Build complete!"
echo "Application: $BUILD_DIR/Pngyu.app"
echo ""
echo "To create a distributable package, run:"
echo "  ./deploy_mac.sh"
