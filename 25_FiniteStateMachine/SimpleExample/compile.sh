#!/bin/sh
c++ -std=c++2a -O3 -Wfatal-errors -Wall -Wextra -Wpedantic *.cpp -o FSM && strip FSM && FSM
