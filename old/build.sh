#!/usr/bin/env sh

CFLAGS="-Wall -Wextra -Wpedantic -Wswitch-enum -Werror -std=c23 -I./thirdparty/raylib-5.5_linux_amd64/include/"
LDFLAGS="-L./thirdparty/raylib-5.5_linux_amd64/lib/ -lm -l:libraylib.a"
SRC="src/sv.c src/vec.c src/bf.c src/simulation.c src/main.c"
OUT="main.out"

BFLAGS="-O3"

set -xe

cc $CFLAGS $BFLAGS -o $OUT $SRC $LDFLAGS;
