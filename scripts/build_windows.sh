#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd "$SCRIPT_DIR"


# Ensure the build directory exists
if [ ! -d "../build" ]; then
    echo "Creating build directory..."
    mkdir -p ../build
fi

# Compile using MinGW
# -lgdi32 and -luser32 are required for Windows UI functions

echo "Compiling for Windows using MinGW..."

x86_64-w64-mingw32-g++ ../src/main.cpp ../src/GhostUI.cpp \
    -I ../include \
    -o ../build/GhostUI.exe \
    -static-libgcc -static-libstdc++ \
    -lgdi32 -luser32 -mwindows

if [ $? -eq 0 ]; then
    echo "Compilation successful! Executable created at ../build/GhostUI.exe"
else
    echo "Compilation failed!"
fi