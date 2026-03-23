#!/bin/bash
c++ -std=c++2a -O3 -Wfatal-errors -Wall -Wextra -Wpedantic *.cpp -o Register && strip Register && Register
