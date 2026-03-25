#!/bin/bash
# -Wl,-rpath=... Avoid the necesity of puting the library path into LD_LIBRARY_PATH
c++ -std=c++23 -O3 -Wfatal-errors -Wall -Wextra -Wpedantic -Wl,-rpath=../Framework -L../Framework -lFramework -o up up.cpp && strip up
