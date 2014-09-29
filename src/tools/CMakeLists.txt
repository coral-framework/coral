########################################################################
# Deployed Executables
########################################################################

project( TOOLS )

# Add a coral.properties file to the ${CMAKE_BINARY_DIR}/bin dir to indicate
# where the coral executable can find the coral library.
configure_file( coral.properties.in "${CORAL_BINARY_DIR}/bin/coral.properties" )

########################################################################
# Common Settings
########################################################################

include_directories( ${CMAKE_SOURCE_DIR}/src ${CO_SOURCE_DIR}/generated )

set( EXTRA_SOURCES
	Utils.h
	${CO_SOURCE_DIR}/private/OS.h
	${CO_SOURCE_DIR}/private/OS.cpp
  ${CO_SOURCE_DIR}/private/Properties.h
  ${CO_SOURCE_DIR}/private/Properties.cpp
	${CO_SOURCE_DIR}/private/StringTokenizer.h
)

########################################################################
# Build the Launcher Back End
########################################################################

add_executable( launcher launcher.cpp Utils.h )
add_dependencies( launcher lua )

CORAL_TARGET( launcher )

set_target_properties( launcher PROPERTIES
	PROJECT_LABEL "Executable launcher"
)

########################################################################
# Build the Launcher Front End
########################################################################

add_executable( coral coral.cpp ${EXTRA_SOURCES} )
add_dependencies( coral launcher )

CORAL_TARGET_PROPERTIES( coral )
set_property( TARGET coral APPEND PROPERTY COMPILE_DEFINITIONS "CORAL_NO_EXPORT" )
set_target_properties( coral PROPERTIES	PROJECT_LABEL "Executable coral" )

########################################################################
# Build the Compiler Front End
########################################################################

add_executable( coralc coralc.cpp ${EXTRA_SOURCES} )
add_dependencies( coralc launcher )

CORAL_TARGET_PROPERTIES( coralc )
set_property( TARGET coralc APPEND PROPERTY COMPILE_DEFINITIONS "CORAL_NO_EXPORT" )
set_target_properties( coralc PROPERTIES PROJECT_LABEL "Executable coralc" )

########################################################################
# Install Rules
########################################################################

# install the front-end executables (only in release mode)
install( TARGETS coral coralc DESTINATION .
  CONFIGURATIONS Release RelWithDebInfo COMPONENT Core )

# install the launcher executable
install( TARGETS launcher DESTINATION bin COMPONENT Core )
install( FILES ${CMAKE_BINARY_DIR}/bin/launcher_debug.pdb DESTINATION bin
  CONFIGURATIONS Debug COMPONENT Core OPTIONAL )
