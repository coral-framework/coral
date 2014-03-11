########################################################################
# Lua Module (fully integrates Coral and Lua)
########################################################################

project( MODULE_LUA )

########################################################################
# Build the 'lua' module library
########################################################################

# Compile the embedded Lua libraries into a static library
add_subdirectory( libs )

include_directories( libs libs/lua generated ${CORE_SOURCE_DIR}/generated )

file( GLOB HEADER_FILES *.h )
FILE( GLOB SOURCE_FILES *.cpp )

add_library( lua MODULE ${HEADER_FILES} ${SOURCE_FILES} "generated/__AllInOne.cpp" )

CORAL_MODULE_TARGET( "lua" lua )

set_target_properties( lua PROPERTIES
	PROJECT_LABEL "Module Lua"
)

target_link_libraries( lua lualibs )

########################################################################
# Custom target to re-generate the Lua module files using the Coral compiler
########################################################################

add_custom_target( lua_regen
	COMMAND ${CORAL_LAUNCHER} -p "${CMAKE_SOURCE_DIR}/modules,${CMAKE_BINARY_DIR}/modules"
		--no-abi-checks co.compiler.Compile -g lua
	DEPENDS ${CORAL_LAUNCHER}
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	COMMENT "Forcing regeneration of the Lua module files..."
)

########################################################################
# Install Rules
########################################################################

# install shared library
install( TARGETS lua DESTINATION modules/lua COMPONENT "Lua Module" )
install( FILES ${CMAKE_BINARY_DIR}/modules/lua/lua_debug.pdb
	DESTINATION modules/lua CONFIGURATIONS Debug COMPONENT "Lua Module" OPTIONAL )

# install CSL files
install( DIRECTORY ${CMAKE_SOURCE_DIR}/modules/lua DESTINATION modules
	CONFIGURATIONS Release MinSizeRel RelWithDebInfo COMPONENT "Lua Module" )
