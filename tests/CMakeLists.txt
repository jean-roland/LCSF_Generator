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
   	package_add_test(LCSF_Generator_lib_tests test_attribute.cpp test_command.cpp test_extractor.cpp test_generator.cpp test_deschandler.cpp test_data.cpp)
    target_link_libraries(LCSF_Generator_lib_tests LCSF_Generator_lib)
else()
    message(WARNING "Google test not found, tests will not be built")
endif()