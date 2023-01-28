# This file is part of LCSF C Generator.
#
# LCSF C Generator is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LCSF C Generator is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this project. If not, see <https://www.gnu.org/licenses/>
#
# Author: Jean-Roland Gosse

# Find GoogleTest
find_package(GTest)

if (GTEST_FOUND)
    message(STATUS "Google test found, tests will be built")
    include_directories(${GTEST_INCLUDE_DIRS})

    macro(package_add_test TESTNAME)
        # create an exectuable in which the tests will be stored
        add_executable(${TESTNAME} ${ARGN})
        # link the Google test infrastructure
        target_link_libraries(${TESTNAME} ${GTEST_BOTH_LIBRARIES} pthread)
        # add tests to Google test
        gtest_discover_tests(${TESTNAME})
        set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
    endmacro()

    # Test Executables
   	package_add_test(${TESTS_BIN_NAME} test_attribute.cpp test_command.cpp test_extractor.cpp
        test_generator.cpp test_deschandler.cpp test_data.cpp)
    target_link_libraries(${TESTS_BIN_NAME} ${CORE_LIB_NAME})
    target_include_directories(${TESTS_BIN_NAME} PUBLIC "../include/")

    # Custom target to run tests
    add_custom_target(run_tests
      COMMAND ./${TESTS_BIN_NAME}
      COMMENT "Run tests"
      WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    # Target depends on test binary
    add_dependencies(run_tests ${TESTS_BIN_NAME})

else()
    message(WARNING "Google test not found, tests will not be built")
endif()