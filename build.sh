#!/usr/bin/env sh

CFLAGS="-Wall -Wextra -Wpedantic -Wswitch-enum -Werror -std=c23 -ggdb"
LDFLAGS="-lm -lraylib"
SRC="src/sv.c src/vec.c src/main.c"
OUT="main.out"

set -xe

cc $CFLAGS -o $OUT $SRC $LDFLAGS;
