cmake_minimum_required(VERSION 3.21)
project(InfinityCreator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt - installed via the Qt online installer; path comes from CMakePresets.json
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# OpenCASCADE - installed via vcpkg; CONFIG forces use of OpenCASCADEConfig.cmake
find_package(OpenCASCADE CONFIG REQUIRED)

# WIN32 keyword => Windows GUI subsystem (no console window).
# All diagnostics go to debug.log via the qInstallMessageHandler in main.cpp.
add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    src/app/MainWindow.cpp
    src/app/MainWindow.h
    src/app/Document.cpp
    src/app/Document.h
    src/app/FeatureTreeDock.cpp
    src/app/FeatureTreeDock.h
    src/app/MenuBuilder.cpp
    src/app/MenuBuilder.h
    src/app/UndoStack.cpp
    src/app/UndoStack.h
    src/app/AppSettings.cpp
    src/app/AppSettings.h
    src/app/RecentFiles.cpp
    src/app/RecentFiles.h
    src/workspaces/WorkspaceType.h
    src/workspaces/Workspace.cpp
    src/workspaces/Workspace.h
    src/workspaces/PartDesignWorkspace.cpp
    src/workspaces/PartDesignWorkspace.h
    src/workspaces/SketcherWorkspace.cpp
    src/workspaces/SketcherWorkspace.h
    src/viewport/OcctViewport.cpp
    src/viewport/OcctViewport.h
    src/model/Feature.cpp
    src/model/Feature.h
    src/model/FeatureParameter.h
    src/model/BooleanOp.h
    src/model/PrimitiveFeatures.cpp
    src/model/PrimitiveFeatures.h
    src/model/PartModel.cpp
    src/model/PartModel.h
    src/model/FeatureFactory.cpp
    src/model/FeatureFactory.h
    src/model/DressUpFeatures.cpp
    src/model/DressUpFeatures.h
    src/model/DocumentSettings.cpp
    src/model/DocumentSettings.h
    src/model/Units.h
    src/ui/ParameterDialog.cpp
    src/ui/ParameterDialog.h
    src/ui/OptionsDialog.cpp
    src/ui/OptionsDialog.h
    src/ui/DocumentPropertiesDialog.cpp
    src/ui/DocumentPropertiesDialog.h
    src/ui/FirstRunDialog.cpp
    src/ui/FirstRunDialog.h
    src/io/InfinityFormat.cpp
    src/io/InfinityFormat.h
    src/io/MeshUtils.cpp
    src/io/MeshUtils.h
    src/io/StlExporter.cpp
    src/io/StlExporter.h
    src/io/ThreeMfExporter.cpp
    src/io/ThreeMfExporter.h
)

# So includes read as "model/Feature.h", "app/MainWindow.h", etc.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${OpenCASCADE_INCLUDE_DIR}
)

# NOMINMAX: stop <windows.h> (pulled in by WNT_Window.hxx) from defining
# min/max macros that break std::min/std::max and some OCCT templates.
target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)

# Linking the whole config-provided variable rather than individual toolkits
# because OCCT merged/renamed toolkits in 7.8+.
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    ${OpenCASCADE_LIBRARIES}
)

# Deploy Qt runtime DLLs next to the executable (Windows).
# OCCT DLLs are copied automatically by the vcpkg toolchain.
if (WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMENT "Deploying Qt runtime with windeployqt"
    )
endif()
