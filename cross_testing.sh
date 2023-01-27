#!/bin/bash

# Run from LCSF_Generator root directory with LCSF_C_Stack directory next to it

# Generate protcol with import
./build/bin/lcsf_generator_cli -l example/Test.json -a ../LCSF_C_Stack/src/protocols_a/Test_Main.c -b ../LCSF_C_Stack/src/protocols_b/Test_Main.c

# Run LCSF_C_Stack tests
cd ../LCSF_C_Stack/ &&
cmake --build build/ --target run_tests
cd -