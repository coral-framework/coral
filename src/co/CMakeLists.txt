###############################################################################
# Coral Library
###############################################################################

project( CO )

### Generate <co/Config.h> ####################################################

set( CORAL_POINTER_SIZE ${CMAKE_SIZEOF_VOID_P} )

include( TestBigEndian )
TEST_BIG_ENDIAN( IS_BIG_ENDIAN )

if( IS_BIG_ENDIAN )
  set( CORAL_BYTE_ORDER "CORAL_BIG_ENDIAN" )
else()
  set( CORAL_BYTE_ORDER "CORAL_LITTLE_ENDIAN" )
endif()

configure_file( Config.h.in "${CMAKE_CURRENT_BINARY_DIR}/Config.h" )

### Gather Source Files #######################################################

file( GLOB files_core       *.h *.cpp )
file( GLOB files_gen        generated/*.h generated/*.cpp )
file( GLOB files_gen_co     generated/co/*.h )
file( GLOB files_modules    modules/*.h modules/*.cpp )
file( GLOB files_private    private/*.h private/*.cpp )
file( GLOB files_types      types/*.h types/*.cpp )
file( GLOB files_types_csl  types/csl/*.h types/csl/*.cpp types/csl/*.cc types/csl/CSL.* )
file( GLOB files_types_desc types/descriptors/*.h types/descriptors/*.cpp )

set( CO_SOURCES ${files_core} ${files_modules} ${files_private}
  ${files_types} ${files_types_csl} ${files_types_desc} ${files_utils} )

set_property( DIRECTORY ${CO_SOURCE_DIR} PROPERTY SOURCES ${CO_SOURCES} )

### Build the Coral Library ###################################################

include_directories( generated )

add_definitions( -DBUILDING_CORAL )

add_library( co SHARED ${CO_SOURCES} ${files_gen} ${files_gen_co} )

set_target_properties( co PROPERTIES
  OUTPUT_NAME   "coral"
  PROJECT_LABEL "Coral Library"
)

CORAL_TARGET_PROPERTIES( co )

CORAL_TARGET_FOR_COVERAGE( co )

if( UNIX )
  target_link_libraries( co dl )
endif()

#### Source Groups ############################################################

source_group( "" FILES ${files_core} "CMakeLists.txt" )
source_group( "generated" FILES ${files_gen} )
source_group( "generated\\co" FILES ${files_gen_co} )
source_group( "modules" FILES ${files_modules} )
source_group( "private" FILES ${files_private} )
source_group( "types" FILES ${files_types} )
source_group( "types\\csl" FILES ${files_types_csl} )
source_group( "types\\descriptors" FILES ${files_types_desc} )

#### Install Rules ############################################################

# install shared library
install( TARGETS co DESTINATION lib COMPONENT Core )
install( FILES ${CMAKE_BINARY_DIR}/lib/coral_debug.pdb DESTINATION lib
  CONFIGURATIONS Debug COMPONENT Core OPTIONAL )

# install CMake package
install( FILES ${CMAKE_SOURCE_DIR}/cmake/FindCoral.cmake DESTINATION cmake
  CONFIGURATIONS Release RelWithDebInfo COMPONENT Core )

# install CSL files
install( DIRECTORY ${CMAKE_SOURCE_DIR}/modules/co DESTINATION modules
  CONFIGURATIONS Release RelWithDebInfo COMPONENT Core )

# install header files
install( FILES Any.h Common.h Coral.h Exception.h IService.h Log.h Platform.h
  RefPtr.h Slice.h TypeKind.h TypeTraits.h
  "${CMAKE_CURRENT_BINARY_DIR}/Config.h" DESTINATION include/co
  CONFIGURATIONS Release RelWithDebInfo COMPONENT Core )

install(DIRECTORY private DESTINATION include/co
  CONFIGURATIONS Release RelWithDebInfo COMPONENT Core
  FILES_MATCHING PATTERN "*.h" )
