set (CURRENT_PACKAGE_NAME "tests")

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/workarounds.cmake)


add_executable (hexagen hexagen.cpp)

target_link_libraries (hexagen PUBLIC GQLima)

# INSTALL_RPATH modifie le rpath pour les libs internes au projet :
set_target_properties (hexagen PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH ${CMAKE_PACKAGE_RPATH_DIR})

