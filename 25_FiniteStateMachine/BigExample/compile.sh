#!/bin/bash
c++ -std=c++23 -O3 -Wfatal-errors -Wall -Wextra -Wpedantic *.cpp -o FSM && strip FSM && FSM
