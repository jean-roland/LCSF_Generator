#!/bin/bash

set -euo pipefail

# Builds a Linux release archive and drafts release notes from git history.
# Windows build and the GitHub upload are not handled here (need a cross
# toolchain / github.com auth that aren't assumed present).

OUT_DIR=${1:-./dist}

# Version comes from CMakeLists, single source of truth
VER=$(sed -n 's/.*LCSF_Generator VERSION \([0-9.]*\).*/\1/p' CMakeLists.txt)
ARCH=$(uname -m)
NAME=LCSF_Generator-${VER}-linux-${ARCH}
BUILD_DIR=build-release
STAGE=$OUT_DIR/$NAME

echo "Packaging LCSF Generator $VER for linux-$ARCH"

# Clean release build + tests
cmake -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C "$BUILD_DIR"
ninja -C "$BUILD_DIR" run_tests

# Stage a portable bin/ + lib/ tree (RPATH is $ORIGIN/../lib)
rm -rf "$STAGE"
DESTDIR=$(mktemp -d)
DESTDIR="$DESTDIR" ninja -C "$BUILD_DIR" install >/dev/null
mkdir -p "$STAGE"
mv "$DESTDIR"/usr/local/bin "$STAGE/bin"
mv "$DESTDIR"/usr/local/lib "$STAGE/lib"
rm -rf "$DESTDIR"
cp README.md LICENCE.md "$STAGE/"

# Archive
tar -C "$OUT_DIR" -czf "$OUT_DIR/$NAME.tar.gz" "$NAME"
echo "Archive: $OUT_DIR/$NAME.tar.gz"

# Draft changelog from the last tag to HEAD
LAST_TAG=$(git describe --tags --abbrev=0 2>/dev/null || echo "")
NOTES=$OUT_DIR/RELEASE_NOTES_${VER}.md
{
    echo "# LCSF Generator $VER"
    echo
    [[ -n "$LAST_TAG" ]] && echo "Changes since $LAST_TAG:" || echo "Changes:"
    echo
    echo "## Features"; git log --pretty="- %s" ${LAST_TAG:+$LAST_TAG..HEAD} --grep="^feat" | sed 's/^- feat: /- /'
    echo; echo "## Fixes"; git log --pretty="- %s" ${LAST_TAG:+$LAST_TAG..HEAD} --grep="^fix" | sed 's/^- fix: /- /'
    echo; echo "## Build / internal"; git log --pretty="- %s" ${LAST_TAG:+$LAST_TAG..HEAD} --grep="^\(build\|chore\|refactor\|test\)" | sed 's/^- [a-z]*: /- /'
} > "$NOTES"
echo "Release notes: $NOTES (review/curate before publishing)"
