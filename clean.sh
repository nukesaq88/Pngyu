#!/bin/bash
# Clean build artifacts from Pngyu project

set -e

echo "Cleaning build artifacts..."

# Remove build directory
if [ -d "build" ]; then
    echo "Removing build directory..."
    rm -rf build
fi

# Remove any artifacts in source directory (in case of in-source build)
echo "Removing in-source build artifacts..."
rm -f *.o
rm -f moc_*.cpp moc_*.h
rm -f ui_*.h
rm -f qrc_*.cpp
rm -f moc_predefs.h
rm -f .qmake.stash
rm -f Makefile
rm -rf *.app

echo "Clean complete!"
