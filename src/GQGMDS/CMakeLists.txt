set (CURRENT_PACKAGE_NAME "GQGMDS")
set (CURRENT_PACKAGE_MACRO_NAME "GQGMDS")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

find_package (gmds REQUIRED)

file (GLOB HEADERS public/${CURRENT_PACKAGE_NAME}/*.h)
file (GLOB CPP_SOURCES *.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (GQGMDS ${CPP_SOURCES} ${HEADERS})
set (ALL_TARGETS GQGMDS)
set_property (TARGET GQGMDS PROPERTY VERSION ${GQGMDS_VERSION})
set_property (TARGET GQGMDS PROPERTY SOVERSION ${GQGMDS_MAJOR_VERSION})
set (GQGMDS_PRIVATE_FLAGS -DGQGMDS_VERSION="${GQGMDS_VERSION}")

target_include_directories (GQGMDS PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions (GQGMDS PUBLIC ${GQGMDS_PUBLIC_FLAGS})
target_compile_definitions (GQGMDS PRIVATE ${GQGMDS_PRIVATE_FLAGS})
target_compile_options (GQGMDS PRIVATE ${SHARED_CFLAGS})	# Requested by Qt ...
target_link_libraries (GQGMDS PUBLIC gmds GQualif)
# Etre capable une fois installée de retrouver TkUtil, Qualif*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libGQGMDS.so.5.0.0" to "") ...
set_target_properties (GQGMDS PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
# Inefficace ici : CMAKE_AUTOMOC => dans le CMakeLists.txt racine
#set (CMAKE_AUTOMOC ON)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/GQGMDSDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/GQGMDSDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/GQGMDSDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/GQGMDSDistribution.h)

# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nos recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

# La génération des règles d'installation :
install (TARGETS ${ALL_TARGETS} EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Recopie des en-têtes dans le répertoire d'installation :
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})
# Dont GQGMDSDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GQGMDSDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${QQUALIF_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de GQGMDS :
set (INCLUDE_INSTALL_DIR include/)		# GQGMDS_INCLUDE_DIR avec GQGMDSConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# GQGMDS_LIB_DIR avec GQGMDSConfig.cmake.in
#set (RUNTIME_INSTALL_DIR bin/)		# GQGMDS_RUNTIME_DIR avec GQGMDSConfig.cmake.in
# Création du fichier GQGMDSConfig.cmake qui définira GQGMDS_INCLUDE_DIR, GQGMDS_LIB_DIR et GQGMDS_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR} PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier GQGMDSTargets.cmake dans prefix/lib/GQGMDS/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
# Copie des fichiers GQGMDSConfig.cmake et GQGMDSConfigVersion.cmake dans prefix/lib/GQGMDS/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
