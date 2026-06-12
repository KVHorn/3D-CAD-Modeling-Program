@echo off
cmake --build build --config Debug
if %errorlevel%==0 build\Debug\CADModeler.exe

