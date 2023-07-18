set (CURRENT_PACKAGE_NAME "QCalQual")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

find_package(Qt5Core NO_CMAKE_SYSTEM_PATH)	# In order to enable moc ...

add_executable (QtCalQual QtCalQualMainWindow.cpp QtCalQualMainWindow.h QQualCal.cpp QCalQual.qrc)
set_property (TARGET QtCalQual PROPERTY AUTOMOC ON)
set_property (TARGET QtCalQual PROPERTY AUTORCC ON)	# => QCalQual.qrc taken into account

target_link_libraries (QtCalQual PUBLIC QtQualif)
if (BUILD_GQLima)
	target_link_libraries (QtCalQual PUBLIC GQLima)
endif (BUILD_GQLima)
if (BUILD_GQGMDS)
	target_link_libraries (QtCalQual PUBLIC GQGMDS)
endif (BUILD_GQGMDS)
if (BUILD_GQVtk)
	target_link_libraries (QtCalQual PUBLIC GQVtk)
endif (BUILD_GQVtk)
target_include_directories (QtCalQual PRIVATE ../QtQualif/public ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (QtCalQual PUBLIC z)	# Pour Qt sous Ubuntu installé via l'installeur Qt

# INSTALL_RPATH modifie le rpath pour les libs internes au projet :
set_target_properties (QtCalQual PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH ${CMAKE_PACKAGE_RPATH_DIR})


# INSTALLATION :
include(CMakePackageConfigHelpers)
# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nos recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

install(TARGETS QtCalQual EXPORT QtCalQual DESTINATION ${CMAKE_INSTALL_BINDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Utilisation par d'autres modules de QtCalQual :
set (RUNTIME_INSTALL_DIR bin/)		# QtCalQual_RUNTIME_DIR avec QtCalQualConfig.cmake.in
configure_package_config_file(cmake/${CURRENT_PACKAGE_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${CURRENT_PACKAGE_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
        PATH_VARS RUNTIME_INSTALL_DIR
    )
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${CURRENT_PACKAGE_NAME}ConfigVersion.cmake
        VERSION ${QQUALIF_VERSION}
        COMPATIBILITY SameMajorVersion
    )
install (
   FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${CURRENT_PACKAGE_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PACKAGE_NAME}/${CURRENT_PACKAGE_NAME}ConfigVersion.cmake
   DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ
   )


