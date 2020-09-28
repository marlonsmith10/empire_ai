#!/bin/sh

# Apply the patch files in ./patch
patch --directory='..' < ./patch/ai_core.cpp.patch
patch --directory='..' < ./patch/CMakeLists.txt.patch
patch --directory='../../script/api/' < ./patch/script_object.hpp.patch

# Build openTTD
pwd
cd ../../..
mkdir build
cd build
cmake ..
make -j8

# Copy the squirrel dummy AI to the build directory
cp -r ../src/ai/empire_ai/squirrel_dummy/* ./ai
