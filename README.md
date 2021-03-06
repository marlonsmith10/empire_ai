Empire AI is an AI for OpenTTD written in C++.

Since OpenTTD does not officially support AIs written in C++, several files in the OpenTTD source need to be patched. This is handled automatically by patch_openttd.sh.

To build OpenTTD with Empire AI:

1. Clone the OpenTTD source: git clone https://github.com/OpenTTD/OpenTTD.git
2. Check out a known compatible commit of OpenTTD: git checkout d9b0e6c550c25b4d02c254f3fb5d85ce39e55aa0
3. Change into the OpenTTD AI directory: cd ./OpenTTD/src/ai
4. Clone the Empire AI source: git clone git@github.com:marlonsmith10/empire_ai.git
5. Change into the empire_ai directory: cd ./empire_ai
6. Run patch_openttd.sh to patch and build OpenTTD with Empire AI: ./patch_openttd.sh
7. Run OpenTTD: cd ../../../build && ./openttd
