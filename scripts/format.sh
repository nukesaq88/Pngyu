#!/bin/bash
# Format C++ source files using clang-format

set -e

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

echo "Formatting C++ source files..."

# Check if clang-format is available
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format not found"
    echo "Install with: brew install clang-format"
    exit 1
fi

# Format all C++ source and header files
cd "$PROJECT_ROOT"
find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i

echo "✓ Formatting complete!"
