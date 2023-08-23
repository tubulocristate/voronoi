#!/bin/sh

set -xe

g++ main.cpp glad.c -Wall -Wextra -lglfw3 -lGL -o main
