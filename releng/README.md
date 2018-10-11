Release Engineering for Spider Library
======================================

The release scripts of Spider build the library for Linux 32 / 64 and Win 32.

This requires to have mingw and a cross compiler tool chain installed.

On Ubuntu 16.04, this means installing ```gcc-multilib g++-multilib mingw-w64```.

For Windows PThread, the script automaticaly fetches the pre compiled windows version.
