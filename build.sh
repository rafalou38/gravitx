#!/bin/bash

# Setup Variables
export NAME_PART="gravitx"
export FILES="./src/gravitx.cpp ./lib/tinyxml2.cpp"

# Setup required Environment
export USB_ROOT=../..
export RAYLIB_PATH="$USB_ROOT/raylib/raylib"
export COMPILER_PATH="$USB_ROOT/raylib/w64devkit/bin"
export PATH=$COMPILER_PATH:$PATH
export CFLAGS="$RAYLIB_PATH/src/raylib.rc.data -g --std=c++20 -static -Os -Wall -I$RAYLIB_PATH/src -Iexternal -DPLATFORM_DESKTOP"
export LDFLAGS="-lraylib -lopengl32 -lgdi32 -lwinmm"

# Change to the current directory
cd "$(pwd)"

# Clean latest build
if [ -f "$NAME_PART.exe" ]; then
    rm -f "$NAME_PART.exe"
fi

# Compile program
echo  g++ -o "$NAME_PART.exe" $FILES $CFLAGS $LDFLAGS
../../raylib/w64devkit/bin/g++ -o "$NAME_PART.exe" $FILES $CFLAGS $LDFLAGS

# Reset Environment
unset PATH

# Execute program
if [ -f "$NAME_PART.exe" ]; then
    "./$NAME_PART.exe"
fi
