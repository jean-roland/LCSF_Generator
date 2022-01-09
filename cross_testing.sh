#!/bin/bash

# Run from LCSF_Generator directory with LCSF_C_Stack directory next to it

# Generate protcol with import
./build/src/LCSF_Generator_CLI -l example/Test.json -a ../LCSF_C_Stack/src/protocol/Test_Main.c -b ../LCSF_C_Stack/b-tests/Test_Main.c

# Run LCSF_C_Stack tests
cd ../LCSF_C_Stack &&
ceedling test:all &&
cd ./b-tests &&
./b-tests.sh