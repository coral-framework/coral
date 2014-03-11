########################################################################
# Tests for the Lua Module
########################################################################

project( TESTS_LUA )

########################################################################
# Build the 'tests_lua' executable
########################################################################

# Generate the necessary mappings for building the application
set( CORAL_PATH
	"${CMAKE_BINARY_DIR}/modules"
	"${CMAKE_SOURCE_DIR}/modules"
	"${CMAKE_SOURCE_DIR}/tests/moduleA"
	"${CMAKE_SOURCE_DIR}/tests/moduleB"
	"${CMAKE_SOURCE_DIR}/tests/core/data/csl"
	"${CMAKE_CURRENT_SOURCE_DIR}/modules"
)

CORAL_GENERATE_MAPPINGS( _GENERATED_FILES co.ISystem lua.IState moduleA.IHuman )

# Pass the CORAL_PATH as a precompiler definition
CORAL_GET_PATH_STRING( CORAL_PATH_STR )
set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "CORAL_PATH=\"${CORAL_PATH_STR}\"" )

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}/generated
	${GTEST_INCLUDE_DIRS}
)

# Create the test executable
add_executable( tests_lua EXCLUDE_FROM_ALL Main.cpp LuaTests.cpp ${_GENERATED_FILES} )
add_dependencies( tests_lua lua moduleA )

CORAL_TARGET( tests_lua )

set_target_properties( tests_lua PROPERTIES
	PROJECT_LABEL "Tests - Lua"
)

target_link_libraries( tests_lua ${GTEST_LIBRARIES} )

########################################################################
# Register tests with CTest
########################################################################
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/output )
add_test(
	NAME tests_lua
	COMMAND $<TARGET_FILE:tests_lua> --gtest_output=xml:../output/TestsLua$<CONFIGURATION>.xml
)
CORAL_TEST_ENVIRONMENT( tests_lua )

CORAL_ADD_TEST( launch_lua_component lua.launcher.Component one two three )
set_tests_properties( launch_lua_component PROPERTIES
	PASS_REGULAR_EXPRESSION "ARGS: one two three"
)

########################################################################
# If Valgrind is available, repeat the test checking for memory leaks
########################################################################
if( VALGRIND_COMMAND )
	add_test(
		NAME tests_lua_MemoryCheck
		COMMAND ${VALGRIND_COMMAND} --leak-check=full --show-reachable=yes --num-callers=30 --dsymutil=yes
			--log-file=${CMAKE_BINARY_DIR}/ValgrindLua$<CONFIGURATION>.log --error-exitcode=13
			--suppressions=${CMAKE_SOURCE_DIR}/tests/valgrind.supp $<TARGET_FILE:tests_lua>
	)
endif()

########################################################################
# Source Groups
########################################################################

source_group( "@Mappings" FILES ${_GENERATED_FILES} )
