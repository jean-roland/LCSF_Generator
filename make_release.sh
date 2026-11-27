#!/bin/bash

set -euo pipefail

# Builds a Linux release archive and drafts release notes from git history.
# CI runs it, builds the Windows archive and drafts the GitHub release
# (.github/workflows/ci.yml).

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

# Draft changelog from the last tag to HEAD, a tag on HEAD itself excluded
LAST_TAG=$(git describe --tags --abbrev=0 HEAD^ 2>/dev/null || echo "")
NOTES=$OUT_DIR/RELEASE_NOTES_${VER}.md
{
    echo "# LCSF Generator $VER"
    echo
    [[ -n "$LAST_TAG" ]] && echo "Changes since $LAST_TAG:" || echo "Changes:"
    echo
    # Commit type and scope prefix, e.g. "feat(gui): "
    STRIP_TYPE='s/^- [a-z]+(\([^)]*\))?: /- /'
    echo "## Features"; git log --pretty="- %s" ${LAST_TAG:+$LAST_TAG..HEAD} --grep="^feat" | sed -E "$STRIP_TYPE"
    echo; echo "## Fixes"; git log --pretty="- %s" ${LAST_TAG:+$LAST_TAG..HEAD} --grep="^fix" | sed -E "$STRIP_TYPE"
    echo; echo "## Build / internal"; git log --pretty="- %s" ${LAST_TAG:+$LAST_TAG..HEAD} --grep="^\(build\|chore\|refactor\|test\)" | sed -E "$STRIP_TYPE"
} > "$NOTES"
echo "Release notes: $NOTES (review/curate before publishing)"
