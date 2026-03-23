#!/bin/bash
c++ -std=c++2a -O3 -Wfatal-errors -Wall -Wextra -Wpedantic -c -fpic -o Framework.o Framework.cpp && \
gcc -shared -o libFramework.so Framework.o && \
rm Framework.o && \
chmod 644 libFramework.so && \
strip libFramework.so
