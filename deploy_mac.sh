#!/bin/bash
# Deploy script for distribution
# Run this after building in Qt Creator

set -e

BUILD_DIR="build/Desktop-Release"

if [ ! -d "$BUILD_DIR/Pngyu.app" ]; then
    echo "Error: Pngyu.app not found in $BUILD_DIR"
    echo "Please build the project in Qt Creator first (Release mode)"
    exit 1
fi


echo "Deploying Qt frameworks..."
QT_LIB_PATH=$(qmake -query QT_INSTALL_LIBS)
echo macdeployqt "$BUILD_DIR/Pngyu.app" -always-overwrite -libpath="$QT_LIB_PATH"
macdeployqt "$BUILD_DIR/Pngyu.app" -always-overwrite -libpath="$QT_LIB_PATH"

echo ""
echo "Code signing..."
codesign --deep --force --sign - "$BUILD_DIR/Pngyu.app"

echo ""
echo "Creating DMG..."
DMG_NAME="Pngyu.dmg"
DMG_PATH="$BUILD_DIR/$DMG_NAME"
VOLUME_NAME="Pngyu"

# Remove old DMG if exists
rm -f "$DMG_PATH"

# Create temporary directory for DMG contents
TMP_DMG_DIR=$(mktemp -d)
cp -R "$BUILD_DIR/Pngyu.app" "$TMP_DMG_DIR/"

# Create DMG
hdiutil create -volname "$VOLUME_NAME" -srcfolder "$TMP_DMG_DIR" -ov -format UDZO "$DMG_PATH"

# Clean up
rm -rf "$TMP_DMG_DIR"

echo ""
echo "✓ Deployment complete!"
echo "Distributable app: $BUILD_DIR/Pngyu.app"
echo "DMG file: $DMG_PATH"
echo ""
echo "You can now distribute the DMG file to other users."
