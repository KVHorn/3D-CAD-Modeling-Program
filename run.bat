@echo off
rem Configure (first run only does real work), build Debug, and launch.
rem Visual Studio generator: config is chosen at build time, exe lands in build\Debug.

cmake --preset windows-default
if errorlevel 1 (
    echo CMake configure failed.
    exit /b 1
)

cmake --build build --config Debug
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

start "" "build\Debug\InfinityCreator.exe"
