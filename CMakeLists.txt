# CMake version
cmake_minimum_required(VERSION 3.10)

# Project
project(LCSF_Generator VERSION 1.1 LANGUAGES CXX)

#CMAKE_ variables
#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt directory (switch to your own)
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

# Directories
include_directories(src)
add_subdirectory(src)
include_directories(tests)
add_subdirectory(tests)