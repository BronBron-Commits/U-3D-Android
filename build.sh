#!/bin/sh
set -e

PREFIX=/data/data/com.termux/files/usr

export PATH="$PREFIX/bin:$PATH"
export LD_LIBRARY_PATH="$PREFIX/lib"

unset CFLAGS CXXFLAGS LDFLAGS

clang -g main.c -o u3d-x11 \
  -I"$PREFIX/include" \
  -L"$PREFIX/lib" \
  -lX11 -lEGL -lGLESv2 -lm -ldl
