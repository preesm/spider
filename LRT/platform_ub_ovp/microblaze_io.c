#include <stdio.h>

//
// This file defines primitives expected when linking executables for Xilinx Microblaze
//
// These primitives are normally provided by the driver for a peripheral configured
// in the Xilinx EDK to handle stdout/stdin.
//
// When using the microblaze toolchain's newlib library in a virtual platform the functions
// that call these primitives (read/write) are normally semihosted so these functions never
// get executed, but the linker still requires they exist so we define this dummy file
// and add it to the link command when creating microblaze executables that use newlib.
//
// If you are not using newlib and your application uses these functions then you must
// implement them, perhaps reading writing directly from a device defined in your platform,
// and link them to your executable.
//

int outbyte(char c) {
    return 0;
}

int inbyte(char c) {
    return 0;
}

