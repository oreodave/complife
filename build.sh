#!/usr/bin/env sh

CFLAGS="-Wall -Wextra -Wpedantic -Wswitch-enum -Werror -std=c23 -ggdb"
SRC="main.c"
OUT="main.out"

set -xe

cc $CFLAGS -o $OUT $SRC;
