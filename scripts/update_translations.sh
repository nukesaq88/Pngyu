#!/bin/bash
# Update translation files by extracting strings from source code

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Check if lupdate is available in PATH
if ! command -v lupdate >/dev/null 2>&1; then
    echo "Error: lupdate not found. Please ensure Qt is installed and in PATH."
    echo "Or run: ./scripts/build_mac.sh first to configure Qt paths."
    exit 1
fi

echo "Updating translation files..."
lupdate "$PROJECT_ROOT/src" \
    -ts "$PROJECT_ROOT/translations/pngyu_ja.ts" \
        "$PROJECT_ROOT/translations/pngyu_en.ts"

echo ""
echo "✓ Translation files updated!"
echo ""
echo "To edit translations, use Qt Linguist:"

# Try to find Linguist.app
LUPDATE_PATH=$(which lupdate)
QT_BIN_DIR=$(dirname "$LUPDATE_PATH")
if [ -d "$QT_BIN_DIR/Linguist.app" ]; then
    echo "  open \"$QT_BIN_DIR/Linguist.app\" translations/pngyu_ja.ts"
else
    echo "  # Open Qt Linguist and load translations/pngyu_ja.ts"
fi
