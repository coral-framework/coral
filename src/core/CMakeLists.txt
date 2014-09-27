########################################################################
# Coral Core Library
########################################################################

project( CORE )

#########################################################################
# Generate <co/Config.h>
#########################################################################

set( CORAL_POINTER_SIZE ${CMAKE_SIZEOF_VOID_P} )
configure_file( Config.h.in "${CMAKE_BINARY_DIR}/include/co/Config.h" )

#########################################################################
# Copy public headers to ${CMAKE_BINARY_DIR}/include/co
#########################################################################

file( GLOB CORAL_PUBLIC_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} reserved/*.h )

list( APPEND CORAL_PUBLIC_HEADERS
	Any.h
	Common.h
	Coral.h
	Exception.h
	IService.h
	Log.h
	Platform.h
	RefPtr.h
	Slice.h
	TypeKind.h
	TypeTraits.h
)

foreach( PUBLIC_HEADER ${CORAL_PUBLIC_HEADERS} )
	configure_file( ${PUBLIC_HEADER} "${CMAKE_BINARY_DIR}/include/co/${PUBLIC_HEADER}" COPYONLY )
endforeach()

########################################################################
# Gather Files From Subdirs
########################################################################

file( GLOB files_core		*.h *.cpp )
file( GLOB files_gen		generated/*.h generated/*.cpp )
file( GLOB files_gen_co		generated/co/*.h )
file( GLOB files_modules	modules/*.h modules/*.cpp )
file( GLOB files_reserved	reserved/*.h reserved/*.cpp )
file( GLOB files_types		types/*.h types/*.cpp )
file( GLOB files_types_csl	types/csl/*.h types/csl/*.cpp types/csl/*.cc types/csl/CSL.* )
file( GLOB files_types_desc	types/descriptors/*.h types/descriptors/*.cpp )
file( GLOB files_utils		utils/*.h utils/*.cpp )

set( CORE_SOURCES ${files_core} ${files_modules} ${files_reserved} ${files_types}
		${files_types_csl} ${files_types_desc} ${files_utils} )

set_property( DIRECTORY ${CORE_SOURCE_DIR} PROPERTY SOURCES ${CORE_SOURCES} )

########################################################################
# Build Library
########################################################################

include_directories( generated )

add_definitions( -DBUILDING_CORAL_CORE )

add_library( core SHARED ${CORE_SOURCES} ${files_gen} ${files_gen_co} )

if( NOT MSVC )
	# Disable warnings for auto-generated code
	set_property( SOURCE "types/csl/scanner.cc" APPEND PROPERTY COMPILE_FLAGS "-Wno-unused-parameter" )
endif()

set_target_properties( core PROPERTIES
	OUTPUT_NAME   "coral"
	PROJECT_LABEL "Coral Library"
)

CORAL_TARGET_PROPERTIES( core )

CORAL_TARGET_FOR_COVERAGE( core )

if( UNIX )
    target_link_libraries( core dl )
endif()

########################################################################
# Source Groups
########################################################################

source_group( "" FILES ${files_core} "CMakeLists.txt" )
source_group( "generated" FILES ${files_gen} )
source_group( "generated\\co" FILES ${files_gen_co} )
source_group( "modules" FILES ${files_modules} )
source_group( "reserved" FILES ${files_reserved} )
source_group( "types" FILES ${files_types} )
source_group( "types\\csl" FILES ${files_types_csl} )
source_group( "types\\descriptors" FILES ${files_types_desc} )
source_group( "utils" FILES ${files_utils} )

########################################################################
# Install Rules
########################################################################

# install shared library
install( TARGETS core DESTINATION lib COMPONENT Core )
install( FILES ${CMAKE_BINARY_DIR}/lib/coral_debug.pdb
	DESTINATION lib CONFIGURATIONS Debug COMPONENT Core OPTIONAL )

# install CMake package
install( FILES ${CMAKE_SOURCE_DIR}/cmake/FindCoral.cmake
	DESTINATION cmake CONFIGURATIONS Release RelWithDebInfo COMPONENT Core )

# install public headers
install( DIRECTORY ${CMAKE_BINARY_DIR}/include
	DESTINATION . CONFIGURATIONS Release RelWithDebInfo COMPONENT Core )

# install CSL files
install( DIRECTORY ${CMAKE_SOURCE_DIR}/modules/co
	DESTINATION modules CONFIGURATIONS Release RelWithDebInfo COMPONENT Core )
