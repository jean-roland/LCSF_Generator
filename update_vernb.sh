#!/bin/bash

set -euo pipefail

# Usage
if [[ $# -ne 1 ]]; then
    echo "This script will update LCSF Generator version number."
    echo "Script usage: $0 <version_number>"
    exit -1
fi

VER_NB=$1
CMAKELISTS_PATH=./CMakeLists.txt
TESTS_DATA=./tests/test_data.cpp
DATA_DIR=./tests/data
# Every golden file whose header carries the generator version
FILE_LIST=($TESTS_DATA $(grep -rl "LCSF Generator v" $DATA_DIR))

# Modify CMakeFiles
sed -i -e "s/\(LCSF_Generator VERSION \).*)/\1$VER_NB LANGUAGES CXX)/" $CMAKELISTS_PATH

# Parse model files
for FILE in ${FILE_LIST[@]}; do
    if [[ ! -f "$FILE" ]]; then
        echo "$FILE doesn't exist."
        exit -1
    fi
    sed -i -e "s/\(LCSF Generator v\).*/\1${VER_NB}/" ${FILE}
done
echo "Version change done."