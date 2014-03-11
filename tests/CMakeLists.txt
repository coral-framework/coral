########################################################################
# Unit Tests
########################################################################

# Disable sign-compare warnings (unavoidable when using GTest)
if( NOT MSVC )
	add_definitions( "-Wno-sign-compare" )
endif()

# We require the GTest Framework
find_package( GTest )
if( GTEST_FOUND )

	# On UNIX, GTest generally depends on pthreads, and since CMake
	# does not add pthreads to GTEST_LIBRARIES, we do it...
	if( UNIX )
		list( APPEND GTEST_LIBRARIES pthread )
	endif()

	# Auto-include GTest in all test projects
	include_directories( ${GTEST_INCLUDE_DIRS} )

	# Check if Valgrind is available for the memory check tests
	find_program( VALGRIND_COMMAND valgrind )
	if( NOT VALGRIND_COMMAND )
		message( "WARNING: valgrind not found - memory tests have been disabled!" )
	endif()

	add_subdirectory( core )
	add_subdirectory( lua )
	add_subdirectory( moduleA )
	add_subdirectory( moduleB )
	add_subdirectory( regen )
	add_subdirectory( performance )

	# Add a custom target to build all test executables
	add_custom_target( testsuites DEPENDS launcher coral coralc
		tests_core tests_lua tests_moduleA tests_moduleB tests_regen tests_performance
	)

else()
	message( "WARNING: gtest not found - most tests have been disabled!" )
endif()

########################################################################
# Smoke test the deployed executables
########################################################################

set( CORAL_PATH_STR "${CMAKE_BINARY_DIR}/modules,${CMAKE_SOURCE_DIR}/modules" )

# Test the Launcher Back End
add_test( NAME launcher COMMAND launcher -p ${CORAL_PATH_STR} lua.Launcher noscript )
set_tests_properties( launcher PROPERTIES
	PASS_REGULAR_EXPRESSION "launch script 'noscript' could not be found in the path"
)
CORAL_TEST_ENVIRONMENT( launcher )

# Test the Launcher Front End
add_test( NAME coral COMMAND coral -p ${CORAL_PATH_STR} lua.Launcher noscript )
set_tests_properties( coral PROPERTIES
	PASS_REGULAR_EXPRESSION "launch script 'noscript' could not be found in the path"
)

# Test the Compiler Front End
add_test( NAME coralc COMMAND coralc -g nomodule )
set_tests_properties( coralc PROPERTIES
	ENVIRONMENT "CORAL_PATH=${CORAL_PATH_STR}"
	PASS_REGULAR_EXPRESSION "Generating code for module 'nomodule'"
)
