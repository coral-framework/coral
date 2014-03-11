########################################################################
# Initialization
########################################################################

cmake_minimum_required( VERSION 2.8 )

project( CORAL )

# Add our own modules dir to the modules path
set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

########################################################################
# Global Coral Settings
########################################################################

# Coral Version
set( CORAL_VERSION_MAJOR	"0" )
set( CORAL_VERSION_MINOR	"8" )
set( CORAL_VERSION_PATCH	"0" )
set( CORAL_VERSION "${CORAL_VERSION_MAJOR}.${CORAL_VERSION_MINOR}.${CORAL_VERSION_PATCH}" )

# Code generation rules revision
set( CORAL_COMPILER_OUTPUT_REVISION 17 )

########################################################################
# Global Build Settings
########################################################################

if( NOT MSVC )
	# Enable extra GCC warnings for extra-nice code
	add_definitions( -Wall -Wextra -Wunused -Wwrite-strings -Wno-ignored-qualifiers )
	# For C++ sources only:
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual" )
endif()

# Include our CMake package so we can use all CORAL_*() macros
set( CORAL_ROOT ${CMAKE_SOURCE_DIR} )
set( CORAL_BIN_ROOT ${CMAKE_BINARY_DIR} )
set( CORAL_LAUNCHER coral )
set( CORAL_LIBRARIES core )
find_package( Coral QUIET )

# Shared include paths for all subprojects
include_directories( "${CMAKE_BINARY_DIR}/include" )

########################################################################
# Packaging and Installation
########################################################################

set( CPACK_PACKAGE_NAME					"coral" )
set( CPACK_PACKAGE_VERSION_MAJOR		"${CORAL_VERSION_MAJOR}" )
set( CPACK_PACKAGE_VERSION_MINOR		"${CORAL_VERSION_MINOR}" )
set( CPACK_PACKAGE_VERSION_PATCH		"${CORAL_VERSION_PATCH}" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY	"C++ Component Framework" )

include( CPack )

########################################################################
# Subdirectories
########################################################################

add_subdirectory( src )

enable_testing()
add_subdirectory( tests )
