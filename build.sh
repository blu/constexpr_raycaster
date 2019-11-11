#!/bin/bash

g++ -o raycaster main.cpp -Ofast -fno-exceptions -fno-rtti -fstrict-overflow -fstrict-aliasing -funroll-loops -fwhole-program -fno-fp-int-builtin-inexact
g++ -o bin2png bin2png.cpp -Ofast -fno-exceptions -fno-rtti -lpng
