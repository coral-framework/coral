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
get_property( CORE_SOURCES DIRECTORY ${CORE_SOURCE_DIR} PROPERTY SOURCES )
add_definitions( -DBUILDING_CORAL_CORE )
add_executable( tests_regen EXCLUDE_FROM_ALL ${CORE_SOURCES} ${_GENERATED_FILES} Main.cpp
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

if( NOT MSVC )
	# Disable warnings for auto-generated code
	set_property( SOURCE "${CORE_SOURCE_DIR}/types/csl/scanner.cc" APPEND PROPERTY COMPILE_FLAGS "-Wno-unused-parameter" )
endif()

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

source_group( "@Core" FILES ${CORE_SOURCES} )
