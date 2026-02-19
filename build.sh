#!/bin/bash
# Build script for OscarTutorials-GameTank
# Usage: ./build.sh <tutorial_directory_name>
# Example: ./build.sh 0010_HelloColors

set -e

if [ -z "$1" ]; then
    echo "Usage: $0 <tutorial_name>"
    echo "Available tutorials:"
    for d in */; do
        [ -d "$d" ] && [ "$d" != "lib/" ] && echo "  ${d%/}"
    done
    exit 1
fi

TUTORIAL="$1"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
TUTORIAL_DIR="$SCRIPT_DIR/$TUTORIAL"

if [ ! -d "$TUTORIAL_DIR" ]; then
    echo "Error: Tutorial directory '$TUTORIAL' not found"
    exit 1
fi

# Find the .c source file in the tutorial directory
SOURCE=$(find "$TUTORIAL_DIR" -maxdepth 1 -name '*.c' | head -1)
if [ -z "$SOURCE" ]; then
    echo "Error: No .c file found in $TUTORIAL_DIR"
    exit 1
fi

# Locate oscar64 compiler
OSCAR64="$SCRIPT_DIR/../oscar64/build/oscar64"
if [ ! -x "$OSCAR64" ]; then
    # Try Release subdirectory
    OSCAR64="$SCRIPT_DIR/../oscar64/build/Release/oscar64"
fi
if [ ! -x "$OSCAR64" ]; then
    echo "Error: oscar64 compiler not found at ../oscar64/build/oscar64"
    echo "Build it first: cd ../oscar64 && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build"
    exit 1
fi

# Oscar64 include path (for crt.h, <c64/types.h>, <gametank/gametank.h> etc.)
OSCAR64_INCLUDE="$SCRIPT_DIR/../oscar64/include"

# Output file in tutorial directory
BASENAME=$(basename "$SOURCE" .c)
OUTPUT="$TUTORIAL_DIR/$BASENAME.gtr"

echo "Building $TUTORIAL..."
echo "  Source:  $SOURCE"
echo "  Output:  $OUTPUT"

"$OSCAR64" \
    -tm=gametank \
    -n \
    -O2 \
    -ii="$OSCAR64_INCLUDE" \
    -i="$SCRIPT_DIR/lib" \
    "$SOURCE" \
    -o="$OUTPUT"

if [ -f "$OUTPUT" ]; then
    SIZE=$(wc -c < "$OUTPUT" | tr -d ' ')
    echo "  Success! $OUTPUT ($SIZE bytes)"
else
    echo "  Error: Build failed, no output produced"
    exit 1
fi
