set (CURRENT_PACKAGE_NAME "QtQualif")
set (CURRENT_PACKAGE_MACRO_NAME "QT_QUALIF")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

find_package(Qt5Core NO_CMAKE_SYSTEM_PATH)	# In order to enable moc ...
find_package (QwtCharts 5 REQUIRED)

file (GLOB HEADERS public/${CURRENT_PACKAGE_NAME}/*.h)
file (GLOB CPP_SOURCES *.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (QtQualif ${CPP_SOURCES} ${HEADERS})
set_property (TARGET QtQualif PROPERTY AUTOMOC ON)

set (ALL_TARGETS QtQualif)
set_property (TARGET QtQualif PROPERTY VERSION ${QT_QUALIF_VERSION})
set_property (TARGET QtQualif PROPERTY SOVERSION ${QT_QUALIF_MAJOR_VERSION})
# Rem : en Qt v 4.* on utilise -DUSE_QT_WEBKIT
set (QT_QUALIF_PUBLIC_FLAGS "${MANDATORY_CXX_OPTIONS}")
set (QT_QUALIF_PRIVATE_FLAGS -DQT_QUALIF_VERSION="${QQUALIF_VERSION}")

target_include_directories (QtQualif PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_include_directories (QtQualif PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../GQualif/public>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions (QtQualif PUBLIC ${QT_QUALIF_PUBLIC_FLAGS})
target_compile_definitions (QtQualif PRIVATE ${QT_QUALIF_PRIVATE_FLAGS})
target_compile_options (QtQualif PRIVATE ${SHARED_CFLAGS})	# Requested by Qt ...
target_link_libraries (QtQualif PUBLIC QwtCharts::QwtCharts GQualif)

# Etre capable une fois installée de retrouver vtk*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libQtQualif.so.5.0.0" to "") ...
set_target_properties (QtQualif PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
set_property (TARGET QtQualif PROPERTY AUTOMOC ON)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/QtQualifDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/QtQualifDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/QtQualifDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/QtQualifDistribution.h)

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
# Dont QtQualifDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QtQualifDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${QQUALIF_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de QtQualif :
set (INCLUDE_INSTALL_DIR include/)		# QtQualif_INCLUDE_DIR avec QtQualifConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# QtQualif_LIB_DIR avec QtQualifConfig.cmake.in
#set (RUNTIME_INSTALL_DIR bin/)		# QtQualif_RUNTIME_DIR avec QtQualifConfig.cmake.in
# Création du fichier QtQualifConfig.cmake qui définira QtQualif_INCLUDE_DIR, QtQualif_LIB_DIR et QtQualif_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier QtQualifTargets.cmake dans prefix/lib/QtQualif/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
# Copie des fichiers QtQualifConfig.cmake et QtQualifConfigVersion.cmake dans prefix/lib/QtQualif/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)

