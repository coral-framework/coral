########################################################################
# Even more tests for the Coral core
########################################################################

project( TESTS_MODULEB )

########################################################################
# Build the 'moduleB' module library
########################################################################

# Run the coral compiler on the module
set( CORAL_PATH
	"${CMAKE_BINARY_DIR}/modules"
	"${CMAKE_SOURCE_DIR}/modules"
	"${CMAKE_SOURCE_DIR}/tests/moduleA"
	"${CMAKE_SOURCE_DIR}/tests/moduleB"
)

CORAL_GENERATE_MODULE( _GENERATED_FILES moduleB
	co.ITypeTransaction
	moduleA.TestInterface
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR}/generated )

add_library( moduleB MODULE EXCLUDE_FROM_ALL ModulePart.cpp ${_GENERATED_FILES} )
add_dependencies( moduleB coral )

CORAL_MODULE_TARGET( "moduleB" moduleB )

set_target_properties( moduleB PROPERTIES
	PROJECT_LABEL "Module B"
)

########################################################################
# Build the 'tests_moduleB' executable
########################################################################

# Pass the CORAL_PATH as a precompiler definition
CORAL_GET_PATH_STRING( CORAL_PATH_STR )
set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "CORAL_PATH=\"${CORAL_PATH_STR}\"" )

# Gather test source files in the current directory
file( GLOB TEST_SOURCE_FILES *Tests.cpp )

# Create test executable
add_executable( tests_moduleB EXCLUDE_FROM_ALL Main.cpp ${TEST_SOURCE_FILES} )
add_dependencies( tests_moduleB moduleB )

CORAL_TARGET( tests_moduleB )

set_target_properties( tests_moduleB PROPERTIES
	PROJECT_LABEL "Tests - Module B"
)

target_link_libraries( tests_moduleB ${GTEST_LIBRARIES} )

########################################################################
# Register the test with CTest
########################################################################
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/output )
add_test(
	NAME tests_moduleB
	COMMAND $<TARGET_FILE:tests_moduleB> --gtest_output=xml:../output/TestsModuleB$<CONFIGURATION>.xml
)
CORAL_TEST_ENVIRONMENT( tests_moduleB )

########################################################################
# If Valgrind is available, repeat the test checking for memory leaks
########################################################################
if( VALGRIND_COMMAND )
	add_test(
		NAME tests_moduleB_MemoryCheck
		COMMAND ${VALGRIND_COMMAND} --leak-check=full --show-reachable=yes --num-callers=15 --dsymutil=yes
		--log-file=${CMAKE_BINARY_DIR}/ValgrindModuleB$<CONFIGURATION>.log --error-exitcode=13
		--suppressions=${CMAKE_SOURCE_DIR}/tests/valgrind.supp $<TARGET_FILE:tests_moduleB>
	)
endif()
