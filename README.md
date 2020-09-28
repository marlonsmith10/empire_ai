Empire AI is an AI for OpenTTD written in C++.

Since OpenTTD does not officially support AIs written in C++, several files in the OpenTTD source need to be patched. This is handled automatically by patch_openttd.sh.

To build OpenTTD with Empire AI:

1. Clone the OpenTTD source: git clone https://github.com/OpenTTD/OpenTTD.git
2. Change into the OpenTTD AI directory: cd ./OpenTTD/src/ai
3. Clone the Empire AI source: git clone git@github.com:marlonsmith10/empire_ai.git
4. Change into the empire_ai directory: cd ./empire_ai
5. Run patch_openttd.sh to patch and build OpenTTD with Empire AI: ./patch_openttd.sh
6. Run OpenTTD: cd ../../../build && ./openttd
