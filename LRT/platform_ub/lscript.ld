/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

_STACK_SIZE = DEFINED(_STACK_SIZE) ? _STACK_SIZE : 0x400;
_HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x400;

/* Define Memories in the system */

MEMORY
{
   microblaze_2_local_memory_ilmb_ctrl_local_bram_microblaze_2_local_memory_dlmb_ctrl_local_bram : ORIGIN = 0x00000050, LENGTH = 0x00007FB0
   microblaze_2_local_memory_dlmb_ctrl_fifo_bram : ORIGIN = 0x00010000, LENGTH = 0x00010000
}

/* Specify the default entry point to the program */

ENTRY(_start)

/* Define the sections, and where they are mapped in memory */

SECTIONS
{
.vectors.reset 0x00000000 : {
   KEEP (*(.vectors.reset))
} 

.vectors.sw_exception 0x00000008 : {
   KEEP (*(.vectors.sw_exception))
} 

.vectors.interrupt 0x00000010 : {
   KEEP (*(.vectors.interrupt))
} 

.vectors.hw_exception 0x00000020 : {
   KEEP (*(.vectors.hw_exception))
} 

.text : {
   *(.text)
   *(.text.*)
   *(.gnu.linkonce.t.*)
} > microblaze_2_local_memory_ilmb_ctrl_local_bram_microblaze_2_local_memory_dlmb_ctrl_local_bram

.init : {
   KEEP (*(.init))
} > microblaze_2_local_memory_ilmb_ctrl_local_bram_microblaze_2_local_memory_dlmb_ctrl_local_bram

.fini : {
   KEEP (*(.fini))
} > microblaze_2_local_memory_ilmb_ctrl_local_bram_microblaze_2_local_memory_dlmb_ctrl_local_bram

.ctors : {
   __CTOR_LIST__ = .;
   ___CTORS_LIST___ = .;
   KEEP (*crtbegin.o(.ctors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .ctors))
   KEEP (*(SORT(.ctors.*)))
   KEEP (*(.ctors))
   __CTOR_END__ = .;
   ___CTORS_END___ = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.dtors : {
   __DTOR_LIST__ = .;
   ___DTORS_LIST___ = .;
   KEEP (*crtbegin.o(.dtors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .dtors))
   KEEP (*(SORT(.dtors.*)))
   KEEP (*(.dtors))
   PROVIDE(__DTOR_END__ = .);
   PROVIDE(___DTORS_END___ = .);
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.rodata : {
   __rodata_start = .;
   *(.rodata)
   *(.rodata.*)
   *(.gnu.linkonce.r.*)
   __rodata_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.sdata2 : {
   . = ALIGN(8);
   __sdata2_start = .;
   *(.sdata2)
   *(.sdata2.*)
   *(.gnu.linkonce.s2.*)
   . = ALIGN(8);
   __sdata2_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.sbss2 : {
   __sbss2_start = .;
   *(.sbss2)
   *(.sbss2.*)
   *(.gnu.linkonce.sb2.*)
   __sbss2_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.data : {
   . = ALIGN(4);
   __data_start = .;
   *(.data)
   *(.data.*)
   *(.gnu.linkonce.d.*)
   __data_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.got : {
   *(.got)
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.got1 : {
   *(.got1)
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.got2 : {
   *(.got2)
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.eh_frame : {
   *(.eh_frame)
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.jcr : {
   *(.jcr)
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.gcc_except_table : {
   *(.gcc_except_table)
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.sdata : {
   . = ALIGN(8);
   __sdata_start = .;
   *(.sdata)
   *(.sdata.*)
   *(.gnu.linkonce.s.*)
   __sdata_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.sbss (NOLOAD) : {
   . = ALIGN(4);
   __sbss_start = .;
   *(.sbss)
   *(.sbss.*)
   *(.gnu.linkonce.sb.*)
   . = ALIGN(8);
   __sbss_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.tdata : {
   __tdata_start = .;
   *(.tdata)
   *(.tdata.*)
   *(.gnu.linkonce.td.*)
   __tdata_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.tbss : {
   __tbss_start = .;
   *(.tbss)
   *(.tbss.*)
   *(.gnu.linkonce.tb.*)
   __tbss_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.bss (NOLOAD) : {
   . = ALIGN(4);
   __bss_start = .;
   *(.bss)
   *(.bss.*)
   *(.gnu.linkonce.b.*)
   *(COMMON)
   . = ALIGN(4);
   __bss_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

_SDA_BASE_ = __sdata_start + ((__sbss_end - __sdata_start) / 2 );

_SDA2_BASE_ = __sdata2_start + ((__sbss2_end - __sdata2_start) / 2 );

/* Generate Stack and Heap definitions */

.heap (NOLOAD) : {
   . = ALIGN(8);
   _heap = .;
   _heap_start = .;
   . += _HEAP_SIZE;
   _heap_end = .;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

.stack (NOLOAD) : {
   _stack_end = .;
   . += _STACK_SIZE;
   . = ALIGN(8);
   _stack = .;
   __stack = _stack;
} > microblaze_2_local_memory_dlmb_ctrl_fifo_bram

_end = .;
}

