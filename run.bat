@echo off
echo =======================================
echo Compiling ICT SBA Project (Raylib C++)...
echo =======================================

rem 1. Remove old executable to prevent running an unmodified version
if exist main.exe del main.exe

rem 2. Force the internal compiler to build the static package
.\compiler\bin\g++.exe -g .\*.cpp -o .\main.exe -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm -static -static-libgcc -static-libstdc++

rem 3. Check if compilation was successful
if exist main.exe (
    echo [SUCCESS] Compilation completed. Launching game...
    echo =======================================
    .\main.exe
) else (
    echo.
    echo [ERROR] Compilation failed! 
    echo Please check your source code for syntax errors, 
    echo and ensure your folder is correctly named "compiler".
    echo.
    pause
)
