set (CURRENT_PACKAGE_NAME "GQLima")
set (CURRENT_PACKAGE_MACRO_NAME "GQLIMA")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

find_package (Lima REQUIRED)

file (GLOB HEADERS public/${CURRENT_PACKAGE_NAME}/*.h)
file (GLOB CPP_SOURCES *.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (GQLima ${CPP_SOURCES} ${HEADERS})
set (ALL_TARGETS GQLima)
set_property (TARGET GQLima PROPERTY VERSION ${GQLIMA_VERSION})
set_property (TARGET GQLima PROPERTY SOVERSION ${GQLIMA_MAJOR_VERSION})
set (GQLIMA_PRIVATE_FLAGS -DGQLIMA_VERSION="${GQLIMA_VERSION}")

target_include_directories (GQLima PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions (GQLima PUBLIC ${GQLIMA_PUBLIC_FLAGS})
target_compile_definitions (GQLima PRIVATE ${GQLIMA_PRIVATE_FLAGS})
target_compile_options (GQLima PRIVATE ${SHARED_CFLAGS})	# Requested by Qt ...
target_link_libraries (GQLima PUBLIC Lima::Lima GQualif)
# Etre capable une fois installée de retrouver TkUtil, Qualif*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libGQLima.so.5.0.0" to "") ...
set_target_properties (GQLima PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
# Inefficace ici : CMAKE_AUTOMOC => dans le CMakeLists.txt racine
#set (CMAKE_AUTOMOC ON)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/GQLimaDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/GQLimaDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/GQLimaDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/GQLimaDistribution.h)

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
# Dont GQLimaDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GQLimaDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${QQUALIF_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de GQLima :
set (INCLUDE_INSTALL_DIR include/)		# GQLima_INCLUDE_DIR avec GQLimaConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# GQLima_LIB_DIR avec GQLimaConfig.cmake.in
#set (RUNTIME_INSTALL_DIR bin/)		# GQLima_RUNTIME_DIR avec GQLimaConfig.cmake.in
# Création du fichier GQLimaConfig.cmake qui définira GQLima_INCLUDE_DIR, GQLima_LIB_DIR et GQLima_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR} PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier GQLimaTargets.cmake dans prefix/lib/GQLima/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
# Copie des fichiers GQLimaConfig.cmake et GQLimaConfigVersion.cmake dans prefix/lib/GQLima/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
