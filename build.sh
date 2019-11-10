#!/bin/bash

g++ -o raycaster main.cpp -O1 -fno-exceptions -fno-rtti
g++ -o bin2png bin2png.cpp -Ofast -fno-exceptions -fno-rtti -lpng
