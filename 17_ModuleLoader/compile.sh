#!/bin/bash

# Compile the Module library:
c++ -std=c++23 -O3 -Wfatal-errors -Wall -Wextra -Wpedantic -c -fpic -o Module.o Module.cpp && \

# Create the Module library:
gcc -shared -o libModule.so Module.o && \

# Remove the object:
rm Module.o && \

# strip the library:
strip libModule.so && \

# change the library permitions:
chmod 644 libModule.so && \

# Create the Program that uses the Module:
c++ -std=c++2a -O3 -Wfatal-errors -Wall -Wextra -Wpedantic -o ModuleLoader ModuleLoader.cpp -ldl && \

# strip the program:
strip ModuleLoader && \

# Execute the program:
./ModuleLoader ./libModule.so

#================================================================================ END
