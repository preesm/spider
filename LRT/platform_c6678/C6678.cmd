/****************************************************************************/
/*  C6678.cmd                                                               */
/*  Copyright (c) 2011 Texas Instruments Incorporated                       */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on an C6678              */
/*                 device.  Use it as a guideline.  You will want to        */
/*                 change the memory layout to match your specific C6xxx    */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/*    Usage:       The map below divides the external memory in segments    */
/*                 Use the linker option --define=COREn=1                   */
/*                 Where n is the core number.                              */
/*                                                                          */
/****************************************************************************/

-stack 	0x8000
-heap 	0x800

MEMORY
{
    LOCAL_L1P_SRAM: o = 0x00E00000 l = 0x00008000   /* 32kB LOCAL L1P/SRAM */
    LOCAL_L1D_SRAM: o = 0x00F00000 l = 0x00008000   /* 32kB LOCAL L1D/SRAM */
    LOCAL_L2_SRAM:  o = 0x00800000 l = 0x00080000   /* 512kB LOCAL L2/SRAM */
  
    EMIF16_CS2:      o = 0x70000000 l = 0x04000000   /* 64MB EMIF16 CS2 Data Memory */
    EMIF16_CS3:      o = 0x74000000 l = 0x04000000   /* 64MB EMIF16 CS3 Data Memory */
    EMIF16_CS4:      o = 0x78000000 l = 0x04000000   /* 64MB EMIF16 CS4 Data Memory */
    EMIF16_CS5:      o = 0x7C000000 l = 0x04000000   /* 64MB EMIF16 CS5 Data Memory */
  
    CORE0_DDR3:      o = 0x80000000 l = 0x10000000   /* 256MB DDR3 SDRAM for CORE0 */
    CORE1_DDR3:      o = 0x90000000 l = 0x10000000   /* 256MB DDR3 SDRAM for CORE1 */  
    CORE2_DDR3:      o = 0xA0000000 l = 0x10000000   /* 256MB DDR3 SDRAM for CORE2 */  
    CORE3_DDR3:      o = 0xB0000000 l = 0x10000000   /* 256MB DDR3 SDRAM for CORE3 */  
    CORE4_DDR3:      o = 0xC0000000 l = 0x10000000   /* 256MB DDR3 SDRAM for CORE4 */  
    CORE5_DDR3:      o = 0xD0000000 l = 0x10000000   /* 256MB DDR3 SDRAM for CORE5 */  
    CORE6_DDR3:      o = 0xE0000000 l = 0x10000000   /* 256MB DDR3 SDRAM for CORE6 */  
    CORE7_DDR3:      o = 0xF0000000 l = 0x10000000   /* 256MB DDR3 SDRAM for CORE7 */  
}
 
SECTIONS
{


    .text          >  LOCAL_L2_SRAM
    .stack         >  LOCAL_L2_SRAM
    .bss           >  LOCAL_L2_SRAM
    .cio           >  LOCAL_L2_SRAM
    .const         >  LOCAL_L2_SRAM
    .data          >  LOCAL_L2_SRAM
    .switch        >  LOCAL_L2_SRAM
    .sysmem        >  LOCAL_L2_SRAM
    .far           >  LOCAL_L2_SRAM
    .args          >  LOCAL_L2_SRAM
    .ppinfo        >  LOCAL_L2_SRAM
    .ppdata        >  LOCAL_L2_SRAM
  
    /* COFF sections */
    .pinit         >  LOCAL_L2_SRAM
    .cinit         >  LOCAL_L2_SRAM
  
    /* EABI sections */
    .binit         >  LOCAL_L2_SRAM
    .init_array    >  LOCAL_L2_SRAM
    .neardata      >  LOCAL_L2_SRAM
    .fardata       >  LOCAL_L2_SRAM
    .rodata        >  LOCAL_L2_SRAM
    .c6xabi.exidx  >  LOCAL_L2_SRAM
    .c6xabi.extab  >  LOCAL_L2_SRAM

    platform_lib   >  LOCAL_L2_SRAM

    .image			> LOCAL_L2_SRAM
}
