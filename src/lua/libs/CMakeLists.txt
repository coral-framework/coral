###############################################################################
# Lua Libraries
###############################################################################

PROJECT( LUALIBS C )

FILE( GLOB LUA_SOURCES lua/*.c )
FILE( GLOB LFS_SOURCES lfs/*.c )
FILE( GLOB LUASOCKET_SOURCES socket/*.c )

IF( WIN32 )
	LIST( APPEND LUASOCKET_SOURCES socket/win32/wsocket.c )
ELSE()
	LIST( APPEND LUASOCKET_SOURCES socket/unix/usocket.c )
	ADD_DEFINITIONS( -fPIC -DLUA_USE_LINUX )
ENDIF()

INCLUDE_DIRECTORIES( lua )

ADD_LIBRARY( lualibs STATIC EXCLUDE_FROM_ALL ${LUA_SOURCES} ${LFS_SOURCES} ${LUASOCKET_SOURCES} )

SET_TARGET_PROPERTIES( lualibs PROPERTIES
	PROJECT_LABEL				"Lua Libraries"
	COMPILE_DEFINITIONS_DEBUG	"LUASOCKET_DEBUG"
)

IF( WIN32 )
	TARGET_LINK_LIBRARIES( lualibs ws2_32.lib )
ENDIF()

CORAL_TARGET( lualibs )

########################################################################
# Source Groups
########################################################################

SOURCE_GROUP( "Lua" FILES ${LUA_SOURCES} )
SOURCE_GROUP( "LFS" FILES ${LFS_SOURCES} )
SOURCE_GROUP( "LuaSocket" FILES ${LUASOCKET_SOURCES} )
