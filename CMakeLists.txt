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
project(LCSF_Generator VERSION 1.3 LANGUAGES CXX)

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

# Find Qt5 (manual)
# set(Qt5Core_DIR "/home/jean-roland/Qt/5.13.1/gcc_64/lib/cmake/Qt5Core/")
# set(Qt5Widgets_DIR "/home/jean-roland/Qt/5.13.1/gcc_64/lib/cmake/Qt5Widgets/")
# set(Qt5_DIR "/home/jean-roland/Qt/5.13.1/gcc_64/lib/cmake/Qt5/")
# set(QT_QMAKE_EXECUTABLE "/home/jean-roland/Qt/5.13.1/gcc_64/bin/qmake/qmake.exe")

# Find Qt5
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Check Qt version
if (Qt5Core_VERSION VERSION_LESS 5.10.0)
    message(FATAL_ERROR "Minimum supported Qt5 version is 5.10, current version is ${Qt5Core_VERSION}")
endif()

# Target names
set(GUI_BIN_NAME "lcsf_generator_gui")
set(CLI_BIN_NAME "lcsf_generator_cli")
set(TESTS_BIN_NAME "lcsf_generator_tests")
set(CORE_LIB_NAME "lcsf_generator_lib")

# Project subdirs
add_subdirectory(src)
add_subdirectory(tests)