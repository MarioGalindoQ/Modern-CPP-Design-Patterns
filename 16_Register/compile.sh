#!/bin/bash
c++ -std=c++23 -O3 -Wfatal-errors -Wall -Wextra -Wpedantic *.cpp -o Register && strip Register && Register
