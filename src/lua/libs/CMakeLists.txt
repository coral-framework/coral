###############################################################################
# Lua Libraries
###############################################################################

project( LUALIBS C )

file( GLOB LUA_SOURCES lua/*.c )
file( GLOB LFS_SOURCES lfs/*.c )
file( GLOB LPEG_SOURCES lpeg/*.c )
file( GLOB LUASOCKET_SOURCES socket/*.c )

if( WIN32 )
	list( APPEND LUASOCKET_SOURCES socket/win32/wsocket.c )
else()
	list( APPEND LUASOCKET_SOURCES socket/unix/usocket.c )
	add_definitions( -fPIC -DLUA_USE_LINUX )
endif()

include_directories( lua )

add_library( lualibs STATIC EXCLUDE_FROM_ALL ${LUA_SOURCES}
  ${LFS_SOURCES} ${LPEG_SOURCES} ${LUASOCKET_SOURCES} )

set_target_properties( lualibs PROPERTIES
	PROJECT_LABEL				"Lua Libraries"
	COMPILE_DEFINITIONS	$<$<CONFIG:Debug>:LUASOCKET_DEBUG>
)

if( WIN32 )
	target_link_libraries( lualibs ws2_32.lib )
endif()

CORAL_TARGET( lualibs )

########################################################################
# Source Groups
########################################################################

source_group( "Lua" FILES ${LUA_SOURCES} )
source_group( "LFS" FILES ${LFS_SOURCES} )
source_group( "LPeg" FILES ${LPEG_SOURCES} )
source_group( "LuaSocket" FILES ${LUASOCKET_SOURCES} )
