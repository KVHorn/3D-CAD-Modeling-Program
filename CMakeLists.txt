cmake_minimum_required(VERSION 3.21)
project(CADModeler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt — installed via the Qt online installer; path comes from CMakePresets.json
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# OpenCASCADE — installed via vcpkg; CONFIG forces use of OpenCASCADEConfig.cmake
find_package(OpenCASCADE CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${OpenCASCADE_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    ${OpenCASCADE_LIBRARIES}
)

# Deploy Qt runtime DLLs next to the executable (Windows)
if (WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMENT "Deploying Qt runtime with windeployqt"
    )
endif()

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    resources/app.rc            # Windows .exe icon (Explorer)
    resources/resources.qrc     # Qt resource -> window/taskbar icon
)