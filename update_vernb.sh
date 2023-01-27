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
BRIDGE_H_PATH=$DATA_DIR/model_bridge.h
BRIDGE_AC_PATH=$DATA_DIR/model_bridge_a.c
BRIDGE_BC_PATH=$DATA_DIR/model_bridge_b.c
DESC_PATH=$DATA_DIR/model_desc.c
MAIN_H_PATH=$DATA_DIR/model_main.h
MAIN_AC_PATH=$DATA_DIR/model_main_a.c
MAIN_BC_PATH=$DATA_DIR/model_main_b.c
RAWMAIN_H_PATH=$DATA_DIR/model_raw_main.h
RAWMAIN_AC_PATH=$DATA_DIR/model_raw_main_a.c
RAWMAIN_BC_PATH=$DATA_DIR/model_raw_main_b.c
FILE_LIST=($TESTS_DATA $BRIDGE_H_PATH $BRIDGE_AC_PATH $BRIDGE_BC_PATH $DESC_PATH $MAIN_H_PATH $MAIN_AC_PATH $MAIN_BC_PATH $RAWMAIN_H_PATH $RAWMAIN_AC_PATH $RAWMAIN_BC_PATH)

# Modify CMakeFiles
sed -i -e "s/\(LCSF_Generator LANGUAGES CXX VERSION \).*)/\1$VER_NB)/" $CMAKELISTS_PATH

# Parse model files
for FILE in ${FILE_LIST[@]}; do
    if [[ ! -f "$FILE" ]]; then
        echo "$FILE doesn't exist."
        exit -1
    fi
    sed -i -e "s/\(LCSF Generator v\).*/\1${VER_NB}/" ${FILE}
done
echo "Version change done."