#	Find the Coral framework.
#
#	The following variables are accepted as input:
#		CORAL_ROOT (as a CMake or environment variable)
#			- Directory where Coral was installed (the "install prefix").
#		CORAL_PATH (as a CMake or environment variable)
#			- List of extra module repositories that should be passed to the Coral Compiler.
#
#	This module defines the following variables:
#		CORAL_FOUND - Whether the Coral framework was found.
#		CORAL_ROOT - Absolute path to the Coral SDK root.
#		CORAL_PATH - By default, contains "${CORAL_ROOT}/modules".
#		CORAL_LIBRARIES - Coral's optimized and debug libraries (whichever are available).
#		CORAL_INCLUDE_DIRS - Directories for including the Coral headers.
#		CORAL_LAUNCHER - Path to the 'coral' executable.
#
#	The following utility functions simplify the task of invoking the Coral Compiler.
#
#	function CORAL_GENERATE_MODULE( generatedSourceFiles moduleName [extraDependency ...] )
#		Generates source code, plus all required mappings, to implement the Coral module specified
#		by moduleName. All files are generated in a dir named "generated", relative to the current
#		CMake binary dir. Generated source files that should be added to the module's target are
#		added to the list variable indicated as the first parameter.
#
#	function CORAL_GENERATE_MAPPINGS( generatedHeaderFiles [typeName ...] )
#		Generates mappings for the given list of types. All files are generated in a dir named
#		"generated", relative to the current CMake binary dir. Generated files are added to the
#		list variable indicated as the first parameter.
#

cmake_minimum_required( VERSION 2.8.5 )

################################################################################
# Initialization
################################################################################

# Initialize CORAL_ROOT
if( NOT CORAL_ROOT )
	file( TO_CMAKE_PATH "$ENV{CORAL_ROOT}" CORAL_ROOT )
	if( NOT CORAL_ROOT )
		find_path( CORAL_ROOT coralc PATH_SUFFIXES .. PATHS ${CMAKE_MODULE_PATH} )
		if( NOT CORAL_ROOT )
			message( FATAL_ERROR "Could not determine the CORAL_ROOT" )
		endif()
	endif()
endif()

# Initialize CORAL_PATH
if( NOT CORAL_PATH )
	file( TO_CMAKE_PATH "$ENV{CORAL_PATH}" CORAL_PATH )
	if( NOT CORAL_PATH )
		set( CORAL_PATH "${CORAL_ROOT}/modules" )
	endif()
endif()

# CORAL_BIN_ROOT is equivalent to CORAL_ROOT, except when building coral
if( NOT CORAL_BIN_ROOT )
	set( CORAL_BIN_ROOT ${CORAL_ROOT} )
endif()

# Set default output dir for module libraries
if( NOT CORAL_MODULE_OUTPUT_DIR )
	set( CORAL_MODULE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/modules" )
endif()

if( APPLE )
	# On OSX use only the standard 64-bit architecture by default
	set( CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_64_BIT)" )
endif()

# Require minimal support for C++11
if( MSVC )
	# On Windows require VS 2010+
	if( MSVC_VERSION LESS 1600 )
		message( FATAL_ERROR "Coral requires Visual Studio 2010 or newer." )
	endif()
elseif( APPLE )
	# On OSX require Clang 3.0+
	if( XCODE_VERSION )
		set( CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11" )
		set( CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++" )
	else()
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++" )
	endif()
else()
	# On Linux require GCC 4.5+
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )
endif()

# Launcher arguments to invoke the coral compiler
set( CORAL_COMPILER_ARGS --csl acd co.compiler.Compile )

# Populate CORAL_CONFIG_SUFFIXES with "", "_DEBUG", etc. for all config types.
foreach( config IN LISTS CMAKE_CONFIGURATION_TYPES ITEMS ${CMAKE_BUILD_TYPE} )
	string( TOUPPER "${config}" config )
	list( APPEND CORAL_CONFIG_SUFFIXES "_${config}" )
endforeach()
set( CORAL_CONFIG_SUFFIXES "" ${CORAL_CONFIG_SUFFIXES} ) # prepend the empty suffix
list( REMOVE_DUPLICATES CORAL_CONFIG_SUFFIXES )

################################################################################
# Function to get the current CORAL_PATH as a comma-separated string
################################################################################
function( CORAL_GET_PATH_STRING coralPathStr )
	set( result )
	foreach( repo ${CORAL_PATH} )
		if( result )
			set( result "${result},${repo}" )
		else()
			set( result "${repo}" )
		endif()
	endforeach()
	set( ${coralPathStr} ${result} PARENT_SCOPE )
endfunction()

################################################################################
# Function to generate mappings for a list of types
################################################################################
function( CORAL_GENERATE_MAPPINGS generatedHeaders )
	set( outDir "${CMAKE_CURRENT_BINARY_DIR}/generated" )

	# Initialize the list with important files that are always generated.
	set( resultList
		"${outDir}/co/ISystem.h"
	)

	foreach( typeName ${ARGN} )
		string( REPLACE "." "/" typePath ${typeName} )
		list( APPEND resultList "${outDir}/${typePath}.h" )
	endforeach()

	list( REMOVE_DUPLICATES resultList )

	CORAL_GET_PATH_STRING( coralPathStr )

	add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/force_out_of_date"
		COMMAND ${CMAKE_COMMAND} -E echo "Forcing dependency check for Coral..."
	)

	add_custom_command( OUTPUT ${resultList}
		COMMAND ${CORAL_LAUNCHER} -p "${coralPathStr}" ${CORAL_LAUNCHER_FLAGS} ${CORAL_COMPILER_ARGS} ${ARGN}
		DEPENDS ${CORAL_LAUNCHER} "${CMAKE_CURRENT_BINARY_DIR}/force_out_of_date"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Running the Coral Compiler..."
	)

	set( ${generatedHeaders} ${resultList} PARENT_SCOPE )

	# "make clean" should delete the coralc cache file
	set_property( DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${outDir}/__coralc_cache.lua" )
endfunction()

################################################################################
# Function to generate code for a module (all extra args are passed to coralc)
################################################################################
function( CORAL_GENERATE_MODULE generatedSourceFiles moduleName )
	set( outDir "${CMAKE_CURRENT_BINARY_DIR}/generated" )

	CORAL_GET_PATH_STRING( coralPathStr )

	# If the Coral Compiler is currently available...
	if( EXISTS ${CORAL_LAUNCHER} )
		# Use the Compiler to get the actual list of generated source files
		execute_process(
			COMMAND ${CORAL_LAUNCHER} -p "${coralPathStr}" ${CORAL_LAUNCHER_FLAGS}
						${CORAL_COMPILER_ARGS} --list -g ${moduleName} ${ARGN}
			OUTPUT_VARIABLE resultList
		)
		if( NOT resultList )
			message( FATAL_ERROR "Unexpected error invoking the Coral Compiler." )
		endif()
		if( resultList MATCHES "\\*\\*\\* Error \\*\\*\\*" )
			string( SUBSTRING "${resultList}" 14 -1 errorMsg )
			message( FATAL_ERROR "Error reported by the Coral compiler: ${errorMsg}" )
		endif()
		string( REGEX REPLACE "([^\n]+)\n*" "${outDir}/\\1;" resultList ${resultList} )
	else()
		# Fall back to the default all-in-one source file
		set( resultList "${outDir}/__AllInOne.cpp" )
	endif()

	add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/force_out_of_date"
		COMMAND ${CMAKE_COMMAND} -E echo "Forcing dependency check for Coral..."
	)

	add_custom_command( OUTPUT ${resultList}
		COMMAND ${CORAL_LAUNCHER} -p "${coralPathStr}" ${CORAL_LAUNCHER_FLAGS}
					${CORAL_COMPILER_ARGS} -g ${moduleName} ${ARGN}
		DEPENDS ${CORAL_LAUNCHER} "${CMAKE_CURRENT_BINARY_DIR}/force_out_of_date"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Running the Coral Compiler..."
	)

	set( ${generatedSourceFiles} ${resultList} PARENT_SCOPE )

	# "make clean" should delete the coralc cache file
	set_property( DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${outDir}/__coralc_cache.lua" )
endfunction()

################################################################################
# Defines a target to generate docs for a module. Passes extra args to coralc.
################################################################################
function( CORAL_GENERATE_DOX targetName moduleName outDir )
	CORAL_GET_PATH_STRING( coralPathStr )
	add_custom_target( ${targetName}
		COMMAND ${CORAL_LAUNCHER} -p "${coralPathStr}" ${CORAL_LAUNCHER_FLAGS}
			${CORAL_COMPILER_ARGS} --dox -g ${moduleName} -o ${outDir} ${ARGN}
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Running the Coral Compiler to extract documentation..."
	)
endfunction()

################################################################################
# Macro to set common properties for all targets.
################################################################################
macro( CORAL_TARGET_PROPERTIES targetName )
	# Artifacts always get config-specific a suffix, except in release mode:
	set_target_properties( ${targetName} PROPERTIES
		DEBUG_POSTFIX "_debug"
		MINSIZEREL_POSTFIX ""
		RELWITHDEBINFO_POSTFIX ""
	)

	set_property( TARGET ${targetName} APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO "NDEBUG" )

	# Generate executables in /bin and libraries (but not modules) in /lib
	get_target_property( targetType ${targetName} TYPE )
	if( targetType STREQUAL "EXECUTABLE" )
		set_target_properties( ${targetName} PROPERTIES
			OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin"
		)
	elseif( NOT targetType STREQUAL "MODULE_LIBRARY" )
		set_target_properties( ${targetName} PROPERTIES
			OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib"
		)
	endif()

	if( MSVC )
		# Disable unavoidable MSVC warnings
		add_definitions( "/wd4251" ) # type needs to have dll-interface to be used by clients
		add_definitions( "/wd4275" ) # non dll-interface class used as base for dll-interface class
		add_definitions( "/wd4521" ) # multiple copy constructors specified
		add_definitions( "/wd4522" ) # multiple assignment operators specified
		add_definitions( "/wd4800" ) # casting integers to bool
		set_property( TARGET ${targetName} APPEND PROPERTY COMPILE_DEFINITIONS
				"_CRT_SECURE_NO_WARNINGS;_SCL_SECURE_NO_DEPRECATE" )
		add_definitions( -D_VARIADIC_MAX=10 ) # defaults to 5, GTest requires 10
	else()
		# Hide all DSO symbols by default (Ref: http://gcc.gnu.org/wiki/Visibility)
		set_property( TARGET ${targetName} APPEND PROPERTY COMPILE_FLAGS "-fvisibility=hidden" )
	endif()

	# Adjust settings on a per-config-type basis
	get_target_property( outputDir ${targetName} OUTPUT_DIR )
	foreach( suffix IN LISTS CORAL_CONFIG_SUFFIXES )
		# Targets built in "Rel" configs must be considered in 'release' mode
		if( suffix MATCHES "REL" )
			set_property( TARGET ${targetName} APPEND PROPERTY COMPILE_DEFINITIONS${suffix} "NDEBUG" )
		endif()
		# Enforce the same output dirs for all config types.
		set_target_properties( ${targetName} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY${suffix} "${outputDir}"
			LIBRARY_OUTPUT_DIRECTORY${suffix} "${outputDir}"
			ARCHIVE_OUTPUT_DIRECTORY${suffix} "${outputDir}"
		)
	endforeach()
endmacro( CORAL_TARGET_PROPERTIES )

################################################################################
# Macro to set common build options for targets that use Coral.
################################################################################
macro( CORAL_TARGET targetName )
	CORAL_TARGET_PROPERTIES( ${targetName} )
	target_link_libraries( ${targetName} ${CORAL_LIBRARIES} )
endmacro( CORAL_TARGET )

################################################################################
# Macro to set common build options for Coral Module targets.
################################################################################
macro( CORAL_MODULE_TARGET moduleName targetName )
	string( REPLACE "." "/" modulePath ${moduleName} )
	set_target_properties( ${targetName} PROPERTIES
		PREFIX ""
		OUTPUT_NAME "${moduleName}"
		OUTPUT_DIR "${CORAL_MODULE_OUTPUT_DIR}/${modulePath}"
	)
	CORAL_TARGET( ${targetName} )
endmacro( CORAL_MODULE_TARGET )

################################################################################
# Macro to build Coral modules comprised only of CSL types (no implementations).
################################################################################
macro( CORAL_BUILD_CSL_MODULE moduleName )
	CORAL_GENERATE_MODULE( _GENERATED_SOURCES ${moduleName} )
	include_directories( ${CORAL_INCLUDE_DIRS} "${CMAKE_CURRENT_BINARY_DIR}/generated" )
	add_library( ${moduleName} MODULE EXCLUDE_FROM_ALL ${_GENERATED_SOURCES} )
	CORAL_MODULE_TARGET( ${moduleName} ${moduleName} )
	source_group( "@Generated" FILES ${_GENERATED_SOURCES} )
endmacro( CORAL_BUILD_CSL_MODULE )

################################################################################
# Macro to set env vars for a test executable so it finds the Coral library.
################################################################################
macro( CORAL_TEST_ENVIRONMENT testName )
	set_property( TEST ${testName} APPEND PROPERTY ENVIRONMENT
		PATH=${CORAL_BIN_ROOT}/lib
		LD_LIBRARY_PATH=${CORAL_BIN_ROOT}/lib
		DYLD_LIBRARY_PATH=${CORAL_BIN_ROOT}/lib
	)
endmacro( CORAL_TEST_ENVIRONMENT )

################################################################################
# Macro to add a test target that invokes the Coral Launcher passing arguments.
################################################################################
macro( CORAL_ADD_TEST testName )
	CORAL_GET_PATH_STRING( coralPathStr )
	add_test( NAME ${testName} COMMAND ${CORAL_LAUNCHER} --mode $<CONFIGURATION> -p "${coralPathStr}" ${ARGN} )
	CORAL_TEST_ENVIRONMENT( ${testName} )
endmacro( CORAL_ADD_TEST )

################################################################################
# Macro to enable generation of 'test coverage' data for a target.
# Only works on UNIX, and only if the CMake var 'TEST_COVERAGE' is enabled.
################################################################################
macro( CORAL_ENABLE_TEST_COVERAGE targetName )
	if( UNIX AND TEST_COVERAGE )
		set_property( TARGET ${targetName} APPEND PROPERTY COMPILE_FLAGS "-fprofile-arcs;-ftest-coverage" )
		set_property( TARGET ${targetName} APPEND PROPERTY LINK_FLAGS "-fprofile-arcs;-ftest-coverage" )
	endif()
endmacro( CORAL_ENABLE_TEST_COVERAGE )

################################################################################
# Adds a file to the list of files to be cleaned in a directory
################################################################################
macro( CORAL_ADD_TO_MAKE_CLEAN filename )
	set_property( DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${filename}" )
endmacro( CORAL_ADD_TO_MAKE_CLEAN )

################################################################################
# Macro to create a custom target that generates documentation using Doxygen.
# The target will configure_file() "${CMAKE_CURRENT_SOURCE_DIR}/${doxyfileName}"
# then run Doxygen on the resulting file from within the current binary dir.
# All extra macro arguments (${ARGN}) will be passed along to Doxygen.
################################################################################
macro( CORAL_GENERATE_DOXYGEN targetName doxyfileName )
	find_package( Doxygen )
	if( DOXYGEN_FOUND )
		set( DOXYFILE_FOUND false )
		if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${doxyfileName}" )
			set( DOXYFILE_FOUND true )
		endif()
		if( DOXYFILE_FOUND )
			message( STATUS "Setting up Doxygen target '${targetName}'..." )
			configure_file( "${doxyfileName}" "${doxyfileName}.configured" )
			add_custom_target( ${targetName}
				COMMENT "Running Doxygen..."
				WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
				COMMAND ${DOXYGEN_EXECUTABLE} "${doxyfileName}.configured" ${ARGN}
			)
			set_target_properties( ${targetName} PROPERTIES PROJECT_LABEL "Doxygen" )
			# Read doxygen configuration file
			file( READ "${CMAKE_CURRENT_BINARY_DIR}/${doxyfileName}.configured" DOXYFILE_CONTENTS )
			string( REGEX REPLACE "\n" ";" DOXYFILE_LINES ${DOXYFILE_CONTENTS} )
			# Parse .tag filename and add to list of files to delete if it exists
			set( DOXYGEN_TAG_FILE )
			set( DOXYGEN_OUTPUT_DIRECTORY )
			set( DOXYGEN_HTML_OUTPUT )
			foreach( DOXYLINE ${DOXYFILE_LINES} )
				string( REGEX REPLACE ".*GENERATE_TAGFILE *= *([^^\n]+).*" "\\1" DOXYGEN_TAG_FILE "${DOXYLINE}" )
				string( REGEX REPLACE ".*OUTPUT_DIRECTORY *= *([^^\n]+).*" "\\1" DOXYGEN_OUTPUT_DIRECTORY "${DOXYLINE}" )
				string( REGEX REPLACE ".*HTML_OUTPUT *= *([^^\n]+).*" "\\1" DOXYGEN_HTML_OUTPUT "${DOXYLINE}" )
			endforeach()
			file( TO_CMAKE_PATH "${DOXYGEN_OUTPUT_DIRECTORY}" DOXYGEN_OUTPUT_DIRECTORY )
			file( TO_CMAKE_PATH "${DOXYGEN_HTML_OUTPUT}" DOXYGEN_HTML_OUTPUT )
			if( DOXYGEN_TAG_FILE )
				CORAL_ADD_TO_MAKE_CLEAN( "${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_TAG_FILE}" )
			endif()
			if( DOXYGEN_OUTPUT_DIRECTORY AND DOXYGEN_HTML_OUTPUT )
				CORAL_ADD_TO_MAKE_CLEAN( "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_HTML_OUTPUT}" )
			endif()
		else( DOXYFILE_FOUND )
			message( SEND_ERROR "Doxyfile not found - configuration error." )
		endif( DOXYFILE_FOUND )
	else( DOXYGEN_FOUND )
		message( "Doxygen not found - documentation will not be generated." )
	endif( DOXYGEN_FOUND )
endmacro( CORAL_GENERATE_DOXYGEN )

################################################################################
# Find the Coral Framework
################################################################################
function( _coral_find_program _name )
	find_program( ${_name}
		NAMES ${ARGN}
		HINTS ${CORAL_BIN_ROOT} ".."
		PATH_SUFFIXES "." "bin"
	)
	mark_as_advanced( ${_name} )
endfunction()

function( _coral_find_library _name )
	find_library( ${_name}
		NAMES ${ARGN}
		HINTS ${CORAL_BIN_ROOT} ".."
		PATH_SUFFIXES "lib"
	)
	mark_as_advanced( ${_name} )
endfunction()

find_path( CORAL_INCLUDE_DIRS "co/Coral.h"
	HINTS ${CORAL_ROOT} ".."
	PATH_SUFFIXES "include"
)

if( NOT CORAL_LAUNCHER )
	_coral_find_program( CORAL_LAUNCHER "coral.exe" "coral" )
endif()

_coral_find_library( CORAL_LIBRARY "coral" )
_coral_find_library( CORAL_LIBRARY_DEBUG "coral_debug" )

if( CORAL_LIBRARIES )
	# a custom CORAL_LIBRARIES was provided
elseif( CORAL_LIBRARY AND CORAL_LIBRARY_DEBUG )
	set( CORAL_LIBRARIES optimized ${CORAL_LIBRARY} debug ${CORAL_LIBRARY_DEBUG} )
elseif( CORAL_LIBRARY )
	set( CORAL_LIBRARIES optimized ${CORAL_LIBRARY} )
elseif( CORAL_LIBRARY_DEBUG )
	set( CORAL_LIBRARIES debug ${CORAL_LIBRARY_DEBUG} )
endif()

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Coral DEFAULT_MSG CORAL_INCLUDE_DIRS CORAL_LIBRARIES CORAL_LAUNCHER )
