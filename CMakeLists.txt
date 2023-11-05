# This file is part of LCSF Generator.

# LCSF Generator is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# LCSF Generator is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with LCSF Generator. If not, see <https://www.gnu.org/licenses/>

# Author: Jean-Roland Gosse

# CMake version
cmake_minimum_required(VERSION 3.14)

# Project
project(LCSF_Generator VERSION 1.4 LANGUAGES CXX)

# Includes
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Compile options
add_compile_options("-DAPP_VERSION=\"${PROJECT_VERSION}\"" "$<$<CONFIG:DEBUG>:-D_APP_DEBUG>")

# Build system debug option (optional)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Qt CMake variables
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# custom Qt5 directory location
# set(Qt5Core_DIR "/home/jgosse/Qt/5.15.2/gcc_64/lib/cmake/Qt5Core/")
# set(Qt5Widgets_DIR "/home/jgosse/Qt/5.15.2/gcc_64/lib/cmake/Qt5Widgets/")
# set(Qt5_DIR "/home/jgosse/Qt/5.15.2/gcc_64/lib/cmake/Qt5/")
# set(QT_QMAKE_EXECUTABLE "/home/jgosse/Qt/5.15.2/gcc_64/bin/qmake/qmake.exe")

# Find Qt5
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Check Qt version
set(REQ_QT5_VER 5.14.0)
if (Qt5Core_VERSION VERSION_LESS ${REQ_QT5_VER})
    message(FATAL_ERROR "Minimum supported Qt5 version is ${REQ_QT5_VER}, current version is ${Qt5Core_VERSION}")
endif()

# Target names
set(GUI_BIN_NAME "lcsf_generator_gui")
set(CLI_BIN_NAME "lcsf_generator_cli")
set(TESTS_BIN_NAME "lcsf_generator_tests")
set(CORE_LIB_NAME "lcsf_generator_lib")

# Cross-testing vars - set LCSF_C_STACK_ROOT_DIR accordingly
set(LCSF_C_STACK_ROOT_DIR "../lcsf_c_stack/")
set(CROSS_TEST_JSON "${PROJECT_SOURCE_DIR}/example/Test.json")
set(CROSS_TEST_A_IMPORT "${LCSF_C_STACK_ROOT_DIR}/src/protocols_a/Test_Main.c")
set(CROSS_TEST_B_IMPORT "${LCSF_C_STACK_ROOT_DIR}/src/protocols_b/Test_Main.c")

# Project subdirs
add_subdirectory(src)
add_subdirectory(tests)

# Cross-testing target
add_custom_target(cross_testing
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CLI_BIN_NAME} -l ${CROSS_TEST_JSON} -a ${CROSS_TEST_A_IMPORT} -b ${CROSS_TEST_B_IMPORT}
    COMMAND cmake --build ${LCSF_C_STACK_ROOT_DIR}/build --target run_tests
    COMMAND rm -rf RustOutput*
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Run cross-testing")
# Target depends on cli binary
add_dependencies(cross_testing ${CLI_BIN_NAME})