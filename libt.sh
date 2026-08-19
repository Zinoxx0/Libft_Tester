#!/bin/bash

# Find out exactly where this script lives
TESTER_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
USER_DIR="$PWD"

# Look for .c files in the current user directory
# We use nullglob so that if no files exist, the array is truly empty instead of containing "*.c"
shopt -s nullglob
SRC_FILES=("$USER_DIR"/*.c)
shopt -u nullglob

if [ ${#SRC_FILES[@]} -eq 0 ]; then
    echo "❌ Error: No .c files found in $USER_DIR"
    echo "💡 Please navigate to your libft project folder and run 'libt' from there."
    exit 1
fi

# CACHE CHECK: Avoid cross-project contamination
CACHE_FILE="$TESTER_DIR/.last_dir"

if [ -f "$CACHE_FILE" ]; then
    LAST_DIR=$(cat "$CACHE_FILE")
    if [ "$LAST_DIR" != "$USER_DIR" ]; then
        echo "🧹 Switched to a new directory. Cleaning old tester cache..."
        make -C "$TESTER_DIR" clean >/dev/null
    fi
fi

# Save the current directory for next time
echo "$USER_DIR" > "$CACHE_FILE"

echo "⚙️  Compiling libft_tester against: $USER_DIR"
echo "--------------------------------------------------"

# Run make and stop if there's a compilation error
if ! make -C "$TESTER_DIR" SRC_DIR="$USER_DIR" all; then
    echo "--------------------------------------------------"
    echo "❌ Compilation failed. Please fix your errors and try again."
    exit 1
fi

echo "--------------------------------------------------"
echo "🚀 Running tester..."
echo ""

# Move into the tester's directory so it can find 'tests/bin/'
cd "$TESTER_DIR" || exit 1
./libft_tester "$@"

# return the user to their original directory when done
cd "$USER_DIR" || exit 1