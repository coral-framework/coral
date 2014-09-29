########################################################################
# Tests the code generation rules applied by the Coral compiler
########################################################################

project( TESTS_REGEN )

########################################################################
# Build the tests_regen executable
########################################################################

# Re-generate code for the 'co' module
set( CORAL_PATH
	"${CMAKE_BINARY_DIR}/modules"
	"${CMAKE_SOURCE_DIR}/modules"
)

set( CORAL_LAUNCHER_FLAGS "--no-abi-checks" )
CORAL_GENERATE_MODULE( _GENERATED_FILES co )
set( CORAL_LAUNCHER_FLAGS "" )

# Pass the CORAL_PATH as a precompiler definition
CORAL_GET_PATH_STRING( coralPathStr )
set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "CORAL_PATH=\"${coralPathStr}\"" )

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_CURRENT_BINARY_DIR}/generated
	${GTEST_INCLUDE_DIRS}
)

# Create the test executable
get_property( CO_SOURCES DIRECTORY ${CO_SOURCE_DIR} PROPERTY SOURCES )
add_definitions( -DBUILDING_CORAL )
add_executable( tests_regen EXCLUDE_FROM_ALL ${CO_SOURCES} ${_GENERATED_FILES} Main.cpp
	# Select a few tests from tests_core to run again in tests_regen
	../core/AnyTests.cpp
	../core/ReflectorTests.cpp
)
add_dependencies( tests_regen coral )

CORAL_TARGET_PROPERTIES( tests_regen )

set_target_properties( tests_regen PROPERTIES
	PROJECT_LABEL "Tests - Regeneration"
)

target_link_libraries( tests_regen ${GTEST_LIBRARIES} )

IF( UNIX )
    TARGET_LINK_LIBRARIES( tests_regen dl )
ENDIF()

########################################################################
# Register the test with CTest
########################################################################
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/output )
add_test(
	NAME tests_regen
	COMMAND $<TARGET_FILE:tests_regen> --gtest_output=xml:../output/TestsRegeneration$<CONFIGURATION>.xml
)
CORAL_TEST_ENVIRONMENT( tests_regen )

########################################################################
# Source Groups
########################################################################

source_group( "@Core" FILES ${CO_SOURCES} )
