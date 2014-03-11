########################################################################
# More tests for the Coral core
########################################################################

project( TESTS_MODULEA )

########################################################################
# Build the 'moduleA' module
########################################################################

# Run the coral compiler on the module; we also set the CORAL_PATH with
# a repeated CSL dir to check whether it would cause a problem.
set( CORAL_PATH
	"${CMAKE_BINARY_DIR}/modules"
	"${CMAKE_SOURCE_DIR}/modules"
	"${CMAKE_SOURCE_DIR}/modules"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
CORAL_GENERATE_MODULE( _GENERATED_FILES moduleA )

include_directories( ${CMAKE_CURRENT_BINARY_DIR}/generated )

file( GLOB _HEADERS *.h )
file( GLOB _SOURCES *Adapter.cpp Test*.cpp *Annotation.cpp )

add_library( moduleA MODULE EXCLUDE_FROM_ALL ${_HEADERS} ${_SOURCES} ${_GENERATED_FILES} )
add_dependencies( moduleA coral )

CORAL_MODULE_TARGET( "moduleA" moduleA )

set_target_properties( moduleA PROPERTIES
	PROJECT_LABEL "Module A"
)

########################################################################
# Build the test executable
########################################################################

# Pass the CORAL_PATH as a precompiler definition
CORAL_GET_PATH_STRING( CORAL_PATH_STR )
set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "CORAL_PATH=\"${CORAL_PATH_STR}\"" )

# Gather test source files in the current directory
file( GLOB _TESTS *Tests.cpp )

# Create test executable
add_executable( tests_moduleA EXCLUDE_FROM_ALL Main.cpp ${_TESTS} )
add_dependencies( tests_moduleA moduleA )

CORAL_TARGET( tests_moduleA )

set_target_properties( tests_moduleA PROPERTIES
	PROJECT_LABEL "Tests - Module A"
)

target_link_libraries( tests_moduleA ${GTEST_LIBRARIES} )

########################################################################
# Register the test
########################################################################
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/output )
add_test(
	NAME tests_moduleA
	COMMAND $<TARGET_FILE:tests_moduleA> --gtest_output=xml:../output/TestsModuleA$<CONFIGURATION>.xml
)
CORAL_TEST_ENVIRONMENT( tests_moduleA )

########################################################################
# If Valgrind is available, repeat the test checking for memory leaks
########################################################################
if( VALGRIND_COMMAND )
	add_test(
		NAME tests_moduleA_MemoryCheck
		COMMAND ${VALGRIND_COMMAND} --leak-check=full --show-reachable=yes --num-callers=15 --dsymutil=yes
		--log-file=${CMAKE_BINARY_DIR}/ValgrindModuleA$<CONFIGURATION>.log --error-exitcode=13
		--suppressions=${CMAKE_SOURCE_DIR}/tests/valgrind.supp $<TARGET_FILE:tests_moduleA>
	)
endif()
