# Config file
configure_file (
    "version.h.in"
    "version.h"
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Libraries
add_library(LCSF_Generator_lib SHARED attribute.cpp codeextractor.cpp codegenerator.cpp command.cpp)
target_link_libraries(LCSF_Generator_lib Qt5::Core)

# Executables
add_executable(LCSF_Generator mainwindow.ui mainwindow.cpp mainwindow.h main.cpp)
target_link_libraries(LCSF_Generator LCSF_Generator_lib)
target_link_libraries(LCSF_Generator Qt5::Widgets)

add_executable(LCSF_Generator_CLI CLI_main.cpp)
target_link_libraries(LCSF_Generator_CLI LCSF_Generator_lib)