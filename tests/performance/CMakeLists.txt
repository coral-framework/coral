########################################################################
# Performance Tests
########################################################################

project( TESTS_PERFORMANCE )

########################################################################
# Build the 'tests_performance' executable
########################################################################

# Generate the necessary mappings for building the application
set( CORAL_PATH
	"${CMAKE_BINARY_DIR}/modules"
	"${CMAKE_SOURCE_DIR}/modules"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

CORAL_GENERATE_MAPPINGS( _GENERATED_FILES co.ISystem lua.IState )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/generated )

# Pass the CORAL_PATH as a precompiler definition
CORAL_GET_PATH_STRING( CORAL_PATH_STR )
set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "CORAL_PATH=\"${CORAL_PATH_STR}\"" )

# Create the test executable
add_executable( tests_performance EXCLUDE_FROM_ALL Main.cpp PerformanceTests.cpp ${_GENERATED_FILES} )
add_dependencies( tests_performance coral )

CORAL_TARGET( tests_performance )

set_target_properties( tests_performance PROPERTIES
	PROJECT_LABEL "Tests - Performance"
)

target_link_libraries( tests_performance ${GTEST_LIBRARIES} )

########################################################################
# Register tests with CTest
########################################################################
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/output )
add_test(
	NAME tests_performance
	COMMAND $<TARGET_FILE:tests_performance> --gtest_output=xml:../output/TestsPerformance$<CONFIGURATION>.xml
)
CORAL_TEST_ENVIRONMENT( tests_performance )

########################################################################
# Source Groups
########################################################################

source_group( "@Mappings" FILES ${_GENERATED_FILES} )
