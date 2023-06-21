set (CURRENT_PACKAGE_NAME "GQualif")
set (CURRENT_PACKAGE_MACRO_NAME "GQUALIF")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

find_package (TkUtil 6 REQUIRED)
if (BUILD_GQLima)
	find_package (Lima REQUIRED)
endif (BUILD_GQLima)
find_package (qualifinternal REQUIRED)

file (GLOB HEADERS public/${CURRENT_PACKAGE_NAME}/*.h)
file (GLOB CPP_SOURCES *.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (GQualif ${CPP_SOURCES} ${HEADERS})
set (ALL_TARGETS GQualif)
set_property (TARGET GQualif PROPERTY VERSION ${GQUALIF_VERSION})
set_property (TARGET GQualif PROPERTY SOVERSION ${GQUALIF_MAJOR_VERSION})
set (GQUALIF_PRIVATE_FLAGS -DGQUALIF_VERSION="${GQUALIF_VERSION}" -DQQUALIF_VERSION="${QQUALIF_VERSION}")

target_include_directories (GQualif PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions (GQualif PUBLIC ${GQUALIF_PUBLIC_FLAGS})
target_compile_definitions (GQualif PRIVATE ${GQUALIF_PRIVATE_FLAGS})
target_compile_options (GQualif PRIVATE ${SHARED_CFLAGS})	# Requested by Qt ...
target_link_libraries (GQualif PUBLIC TkUtil::TkUtil qualifinternal::qualifinternal)
# Etre capable une fois installée de retrouver TkUtil, Qualif*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libGQualif.so.5.0.0" to "") ...
set_target_properties (GQualif PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
# Inefficace ici : CMAKE_AUTOMOC => dans le CMakeLists.txt racine
#set (CMAKE_AUTOMOC ON)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/GQualifDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/GQualifDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/GQualifDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/GQualifDistribution.h)

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
# Dont GQualifDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GQualifDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${QQUALIF_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de GQualif :
set (INCLUDE_INSTALL_DIR include/)		# GQualif_INCLUDE_DIR avec GQualifConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# GQualif_LIB_DIR avec GQualifConfig.cmake.in
#set (RUNTIME_INSTALL_DIR bin/)		# GQualif_RUNTIME_DIR avec GQualifConfig.cmake.in
# Création du fichier GQualifConfig.cmake qui définira GQualif_INCLUDE_DIR, GQualif_LIB_DIR et GQualif_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR} PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier GQualifTargets.cmake dans prefix/lib/GQualif/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
# Copie des fichiers GQualifConfig.cmake et GQualifConfigVersion.cmake dans prefix/lib/GQualif/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
