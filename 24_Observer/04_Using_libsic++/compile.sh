#!/bin/sh
c++ -std=c++2a -O3 -Wfatal-errors -Wall -Wextra -Wpedantic `pkg-config --cflags gtkmm-3.0` -o Observer *.cpp `pkg-config --libs gtkmm-3.0` && Observer
