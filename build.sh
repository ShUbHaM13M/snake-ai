#!/usr/bin/bash

#timestamp=$(date +%s)

libs="`pkg-config --libs --cflags sdl2` -lSDL2_ttf"
warnings="-Wall -Wextra"
includes=""

clang $includes -g ./src/snake.c -ogame $libs $warnings
