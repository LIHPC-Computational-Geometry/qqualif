set (CURRENT_PACKAGE_NAME "tests")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)


add_executable (hexagen hexagen.cpp)

target_link_libraries (hexagen PUBLIC GQLima)

# INSTALL_RPATH modifie le rpath pour les libs internes au projet :
set_target_properties (hexagen PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH ${CMAKE_PACKAGE_RPATH_DIR})

