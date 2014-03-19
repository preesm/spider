// Generated from "fr.irisa.example.Top_Simple"

#include "orcc_types.h"
#include "lrt_orcc_fifo.h"

#define SIZE 8192
// #define PRINT_FIRINGS

/////////////////////////////////////////////////
// FIFO allocation
DECLARE_FIFO(u8, 8192, 0, 1)
DECLARE_FIFO(u8, 8192, 1, 1)

/////////////////////////////////////////////////
// FIFO pointer assignments
fifo_u8_t *Source_O = &fifo_0;
fifo_u8_t *Process_I = &fifo_0;

fifo_u8_t *Process_O = &fifo_1;
fifo_u8_t *Target_I = &fifo_1;


