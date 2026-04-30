# This file is part of LCSF Generator.
#
# LCSF Generator is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LCSF Generator is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this project. If not, see <https://www.gnu.org/licenses/>
#
# Author: Jean-Roland Gosse

# Libraries
add_library(${CORE_LIB_NAME} SHARED attribute.cpp docgenerator.cpp codeextractor.cpp codegenerator.cpp rustgenerator.cpp rustextractor.cpp command.cpp deschandler.cpp)
target_link_libraries(${CORE_LIB_NAME} Qt5::Core)
target_include_directories(${CORE_LIB_NAME} PUBLIC "../include/")

# Executables
add_executable(${GUI_BIN_NAME} mainwindow.ui mainwindow.cpp mainwindow.h main.cpp)
target_link_libraries(${GUI_BIN_NAME} ${CORE_LIB_NAME} Qt5::Widgets)
target_include_directories(${GUI_BIN_NAME} PUBLIC "../include/")

add_executable(${CLI_BIN_NAME} CLI_main.cpp)
target_link_libraries(${CLI_BIN_NAME} ${CORE_LIB_NAME})
target_include_directories(${CLI_BIN_NAME} PUBLIC "../include/")

# Install
set_target_properties(${CORE_LIB_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
set_target_properties(${GUI_BIN_NAME} ${CLI_BIN_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
install(TARGETS ${GUI_BIN_NAME} ${CLI_BIN_NAME} ${CORE_LIB_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
