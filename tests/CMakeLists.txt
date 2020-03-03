# Find GoogleTest
find_package(GTest)
if (GTEST_FOUND)
	message(STATUS "Google test found, tests will be built")
	include_directories(${GTEST_INCLUDE_DIRS})

	macro(package_add_test TESTNAME)
	    # create an exectuable in which the tests will be stored
	    add_executable(${TESTNAME} ${ARGN})
	    # link the Google test infrastructure
	    target_link_libraries(${TESTNAME} ${GTEST_BOTH_LIBRARIES})
	    # add tests to Google test
	    gtest_discover_tests(${TESTNAME})
	    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
	endmacro()

	# Test Executables
else()
	message(WARNING "Google test not found, tests will not be built")
endif()