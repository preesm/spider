#ifndef EDMA_H
#define EDMA_H

int edma_init();
int edma_clean();

int edma_cpy(
		void* src, void* dst,
		int aCnt, int bCnt, int cCnt,
		int srcBIdx, int dstBIdx,
		int srcCIdx, int dstCIdx
	);

#endif
