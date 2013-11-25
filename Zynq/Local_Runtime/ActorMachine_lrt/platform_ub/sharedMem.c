/*
 * sharedMem.c
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */


#include <xaxicdma.h>
#include <stdlib.h>

#include "types.h"
#include "platform.h"
#include "print.h"


static XAxiCdma xcdma;

void OS_ShMemInit() {
    XAxiCdma_Config CdmaCfgPtr;
    int Status;

    CdmaCfgPtr.DeviceId 	= 0;
    CdmaCfgPtr.BaseAddress 	= CDMA_ADDR;
    CdmaCfgPtr.BurstLen 	= 16;
    CdmaCfgPtr.DataWidth 	= 64;
    CdmaCfgPtr.HasDRE 		= 1;
    CdmaCfgPtr.IsLite 		= 0;

//    CdmaCfgPtr = XAxiCdma_LookupConfig(cpuId-1);
//	if (!CdmaCfgPtr) {
//		zynq_puts("Error in XAxiCdma_LookupConfig (");
//		zynq_puthex(CdmaCfgPtr);
//		zynq_puts(")\r\n");
//		exit(-1);
//	}

	Status = XAxiCdma_CfgInitialize(&xcdma , &CdmaCfgPtr, CdmaCfgPtr.BaseAddress);
	if (Status != XST_SUCCESS) {
		zynq_puts("Error in XAxiCdma_CfgInitialize (");
		zynq_puthex(Status);
		zynq_puts(")\r\n");
		exit(-1);
	}

	XAxiCdma_IntrDisable(&xcdma, XAXICDMA_XR_IRQ_ALL_MASK);
}

UINT32 OS_ShMemRead(UINT32 address, void* data, UINT32 size) {
	int res = 0;
	int Status, CDMA_Status;

//	zynq_puts("Read @");zynq_puthex(address);zynq_puts(" L");zynq_putdec(size);zynq_puts(" D");zynq_puthex(data);zynq_puts("\n");
//	zynq_puts("CDMA Ctrl: ");zynq_puthex(*(volatile u32 *)(0x42000000));zynq_puts("\n");
//	zynq_puts("CDMA Status: ");zynq_puthex(*(volatile u32 *)(0x42000004));zynq_puts("\n");
//	zynq_puts("CDMA addr: ");zynq_puthex(xcdma.BaseAddr);zynq_puts("\n");

	if (DDR_OFFSET <= address
			&& DDR_OFFSET + DDR_SIZE > address
			&& DDR_OFFSET <= address + size
			&& DDR_OFFSET + DDR_SIZE > address + size) {

		while (XAxiCdma_IsBusy(&xcdma)){
			zynq_puts("xdma Still busy !\n"); // Wait
			zynq_puts("CDMA addr: ");zynq_puthex(xcdma.BaseAddr);zynq_puts("\n");
			zynq_puts("CDMA Ctrl: ");zynq_puthex(*(volatile u32 *)(0x42000000));zynq_puts("\n");
			zynq_puts("CDMA Status: ");zynq_puthex(*(volatile u32 *)(0x42000004));zynq_puts("\n");
			zynq_puts("CDMA Cur Desc ptr: ");zynq_puthex(*(volatile u32 *)(0x42000008));zynq_puts("\n");
			zynq_puts("CDMA Tail Desc ptr: ");zynq_puthex(*(volatile u32 *)(0x42000010));zynq_puts("\n");
			zynq_puts("CDMA Source Add: ");zynq_puthex(*(volatile u32 *)(0x42000018));zynq_puts("\n");
			zynq_puts("CDMA Dest Add: ");zynq_puthex(*(volatile u32 *)(0x42000020));zynq_puts("\n");
			zynq_puts("CDMA Bytes to transfert: ");zynq_puthex(*(volatile u32 *)(0x42000020));zynq_puts("\n");
		}

		Status = XAxiCdma_SimpleTransfer(&xcdma, address, (UINT32)data, size, NULL, NULL);
		if (Status != XST_SUCCESS) {
			CDMA_Status = XAxiCdma_GetError(&xcdma);
			if (CDMA_Status != 0x0) {
				XAxiCdma_Reset(&xcdma);
				zynq_puts("Error in XAxiCdma_SimpleTransfer (");
				zynq_puthex(Status);
				zynq_puts(")\r\n");
			}
			exit(XST_FAILURE);
		}

		while (XAxiCdma_IsBusy(&xcdma)); // Wait
		CDMA_Status = XAxiCdma_GetError(&xcdma);
		if (CDMA_Status != 0x0) {
			XAxiCdma_Reset(&xcdma);
			zynq_puts("Error in XAxiCdma_IsBusy (");
			zynq_puthex(CDMA_Status);
			zynq_puts(")\r\n");
			volatile int i;
			for(i=0; i<20000000; i++);
			exit(XST_FAILURE);
		}

		return size;

	}else{
		zynq_puts("Memory not found (");
		zynq_puthex(address);
		zynq_puts(")\r\n");
		return res;
	}
}

UINT32 OS_ShMemWrite(UINT32 address, void* data, UINT32 size) {
	int res = 0;
	int Status, CDMA_Status;

//	zynq_puts("Write @");zynq_puthex(address);zynq_puts(" L");zynq_putdec(size);zynq_puts(" D");zynq_puthex(data);zynq_puts("\n");
//	zynq_puts("CDMA Ctrl: ");zynq_puthex(*(volatile u32 *)(0x42000000));zynq_puts("\n");
//	zynq_puts("CDMA Status: ");zynq_puthex(*(volatile u32 *)(0x42000004));zynq_puts("\n");
	if (DDR_OFFSET <= address
			&& DDR_OFFSET + DDR_SIZE > address
			&& DDR_OFFSET <= address + size
			&& DDR_OFFSET + DDR_SIZE > address + size) {

		while (XAxiCdma_IsBusy(&xcdma)){
			zynq_puts("xdma Still busy !\n"); // Wait
			zynq_puts("CDMA addr: ");zynq_puthex(xcdma.BaseAddr);zynq_puts("\n");
			zynq_puts("CDMA Ctrl: ");zynq_puthex(*(volatile u32 *)(0x42000000));zynq_puts("\n");
			zynq_puts("CDMA Status: ");zynq_puthex(*(volatile u32 *)(0x42000004));zynq_puts("\n");
			zynq_puts("CDMA Cur Desc ptr: ");zynq_puthex(*(volatile u32 *)(0x42000008));zynq_puts("\n");
			zynq_puts("CDMA Tail Desc ptr: ");zynq_puthex(*(volatile u32 *)(0x42000010));zynq_puts("\n");
			zynq_puts("CDMA Source Add: ");zynq_puthex(*(volatile u32 *)(0x42000018));zynq_puts("\n");
			zynq_puts("CDMA Dest Add: ");zynq_puthex(*(volatile u32 *)(0x42000020));zynq_puts("\n");
			zynq_puts("CDMA Bytes to transfert: ");zynq_puthex(*(volatile u32 *)(0x42000020));zynq_puts("\n");
		}

		Status = XAxiCdma_SimpleTransfer(&xcdma, (UINT32)data, address, size, NULL, NULL);
		if (Status != XST_SUCCESS) {
			CDMA_Status = XAxiCdma_GetError(&xcdma);
			if (CDMA_Status != 0x0) {
				XAxiCdma_Reset(&xcdma);
				zynq_puts("Error in XAxiCdma_SimpleTransfer (");
				zynq_puthex(CDMA_Status);
				zynq_puts(")\r\n");
				volatile int i;
				for(i=0; i<20000000; i++);
			}
			exit(XST_FAILURE);
		}

		while (XAxiCdma_IsBusy(&xcdma)); // Wait

		CDMA_Status = XAxiCdma_GetError(&xcdma);
		if (CDMA_Status != 0x0) {
			XAxiCdma_Reset(&xcdma);
			zynq_puts("Error in XAxiCdma_IsBusy (");
			zynq_puthex(CDMA_Status);
			zynq_puts(")\r\n");
			exit(XST_FAILURE);
		}

		return size;

	}else{
		zynq_puts("Memory not found (");
		zynq_puthex(address);
		zynq_puts(")\r\n");
		return res;
	}
}
