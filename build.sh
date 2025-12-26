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

# Post-build: Fix QtDBus framework
echo "Adding QtDBus framework..."
QT_LIB_PATH=$(qmake -query QT_INSTALL_LIBS)
QTDBUS_SRC="$QT_LIB_PATH/QtDBus.framework"
QTDBUS_DST="Pngyu.app/Contents/Frameworks/QtDBus.framework"

if [ -d "$QTDBUS_SRC" ]; then
    rsync -a "$QTDBUS_SRC/" "$QTDBUS_DST/" --exclude=Headers 2>/dev/null || true
    
    # Fix install names
    install_name_tool -id @executable_path/../Frameworks/QtDBus.framework/Versions/A/QtDBus \
        "$QTDBUS_DST/Versions/A/QtDBus" 2>/dev/null || true
    
    install_name_tool -change @rpath/QtDBus.framework/Versions/A/QtDBus \
        @executable_path/../Frameworks/QtDBus.framework/Versions/A/QtDBus \
        "Pngyu.app/Contents/Frameworks/QtGui.framework/Versions/A/QtGui" 2>/dev/null || true
    
    # Re-sign the app
    codesign --deep --force --sign - Pngyu.app 2>/dev/null || true
    echo "QtDBus framework added and app re-signed"
fi

echo -e "${GREEN}Build complete!${NC}"
echo "Application: $BUILD_DIR/Pngyu.app"
echo "To run: open $BUILD_DIR/Pngyu.app"
