set (CURRENT_PACKAGE_NAME "GQVtk")
set (CURRENT_PACKAGE_MACRO_NAME "GQVTK")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common_vtk.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

#find_package (VtkContrib REQUIRED)	# Uniquement pour la dépendance à VTK, à remplacer par la suite
if (VTK_7)
	set (VTK_REQUIRED_PACKAGES vtkIOExport vtkIOLegacy)
	find_package (VTK 7 REQUIRED COMPONENTS ${VTK_REQUIRED_PACKAGES})
	include(${VTK_USE_FILE})	# From Examples/GUI/Qt/ImageViewer/CMakeLists.txt
	set (VTK_LIB_DEPENDENCIES ${VTK_LIBRARIES})
	set (VTK_INC_DEPENDENCIES ${VTK_INCLUDE_DIRS})
elseif (VTK_8)
	set (VTK_REQUIRED_PACKAGES vtkIOExport vtkIOLegacy)
	find_package (VTK 8 REQUIRED COMPONENTS ${VTK_REQUIRED_PACKAGES})
	include(${VTK_USE_FILE})	# From Examples/GUI/Qt/ImageViewer/CMakeLists.txt
	set (VTK_LIB_DEPENDENCIES ${VTK_LIBRARIES})
	set (VTK_INC_DEPENDENCIES )	# Empty, not usefull with VTK 8
elseif (VTK_9)
	set (VTK_REQUIRED_PACKAGES IOExport IOLegacy)
	find_package (VTK 9 REQUIRED COMPONENTS ${VTK_REQUIRED_PACKAGES})
	set (VTK_LIB_DEPENDENCIES ${VTK_LIBRARIES})
	set (VTK_INC_DEPENDENCIES )	# Empty, not usefull with VTK 8
else ( )
	message (FATAL_ERROR "Version de VTK non supportée.")
endif ( )

file (GLOB HEADERS public/${CURRENT_PACKAGE_NAME}/*.h)
file (GLOB CPP_SOURCES *.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (GQVtk ${CPP_SOURCES} ${HEADERS})
set (ALL_TARGETS GQVtk)
set_property (TARGET GQVtk PROPERTY VERSION ${GQVTK_VERSION})
set_property (TARGET GQVtk PROPERTY SOVERSION ${GQVTK_MAJOR_VERSION})
set (GQVTK_PRIVATE_FLAGS -DGQVTK_VERSION="${GQVTK_VERSION}")

target_include_directories (GQVtk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
if (VTK_INC_DEPENDENCIES)
	target_include_directories (GQVtk PUBLIC ${VTK_INC_DEPENDENCIES})
endif (VTK_INC_DEPENDENCIES)
target_compile_definitions (GQVtk PUBLIC ${GQVTK_PUBLIC_FLAGS})
target_compile_definitions (GQVtk PRIVATE ${GQVTK_PRIVATE_FLAGS})
target_compile_options (GQVtk PRIVATE ${SHARED_CFLAGS})	# Requested by Qt ...
target_link_libraries (GQVtk PUBLIC GQualif)
target_link_libraries (GQVtk PUBLIC ${VTK_LIB_DEPENDENCIES})

# Etre capable une fois installée de retrouver TkUtil, Qualif*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libGQVtk.so.5.0.0" to "") ...
set_target_properties (GQVtk PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
# Inefficace ici : CMAKE_AUTOMOC => dans le CMakeLists.txt racine
#set (CMAKE_AUTOMOC ON)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/GQVtkDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/GQVtkDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/GQVtkDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/GQVtkDistribution.h)

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
# Dont GQVtkDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GQVtkDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${QQUALIF_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de GQVtk :
set (INCLUDE_INSTALL_DIR include/)		# GQVtk_INCLUDE_DIR avec GQVtkConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# GQVtk_LIB_DIR avec GQVtkConfig.cmake.in
#set (RUNTIME_INSTALL_DIR bin/)		# GQVtk_RUNTIME_DIR avec GQVtkConfig.cmake.in
# Création du fichier GQVtkConfig.cmake qui définira GQVtk_INCLUDE_DIR, GQVtk_LIB_DIR et GQVtk_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR} PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier GQVtkTargets.cmake dans prefix/lib/GQVtk/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
# Copie des fichiers GQVtkConfig.cmake et GQVtkConfigVersion.cmake dans prefix/lib/GQVtk/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
