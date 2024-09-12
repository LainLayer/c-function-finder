#!/usr/bin/env bash

set -xe

gcc -o main -O3 -march=native -flto main.c -Wall -Wextra -Werror -pedantic -std=c11 -D__USE_POSIX -D_XOPEN_SOURCE=600
