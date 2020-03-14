# Find GoogleTest
find_package(GTest)
if (GTEST_FOUND)
    message(STATUS "Google test found, tests will be built")
    include_directories(${GTEST_INCLUDE_DIRS})

    macro(package_add_test TESTNAME)
        # create an exectuable in which the tests will be stored
        add_executable(${TESTNAME} ${ARGN})
        # link the Google test infrastructure
        target_link_libraries(${TESTNAME} ${GTEST_BOTH_LIBRARIES} pthread LCSF_Generator_lib)
        # add tests to Google test
        gtest_discover_tests(${TESTNAME})
        set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
    endmacro()

    # Test Executables
   	package_add_test(test_attribute test_attribute.cpp)
    package_add_test(test_command test_command.cpp)
else()
    message(WARNING "Google test not found, tests will not be built")
endif()