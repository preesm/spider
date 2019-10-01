/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet,            *
 * Hugo Miomandre                                                           *
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

#include <unistd.h>

#ifdef __k1__
#include <HAL/hal/hal_ext.h>
#include <mppa_power.h>
#include <mppa_rpc.h>
#include <utask.h>

#define CHIP_FREQ ((float)(__bsp_frequency))
#endif
#include <pthread.h>

#include <cstring>
#include <cstdio>

#include <platformMPPA.h>

#include <fcntl.h>
#include <stdlib.h>

#include <stdarg.h>
#include <sched.h>
#include <errno.h>

#include <tools/Stack.h>
#include <graphs/Archi/SharedMemArchi.h>
#include <monitor/StackMonitor.h>

#include <lrt.h>
#include <spider.h>
#include <MPPALrtCommunicator.h>
#include <MPPASpiderCommunicator.h>

#ifdef __k1io__
#define MALLOC_DDR(size) malloc(size)
#define MALLOC_SMEM(size) malloc(size)

#define FREE_DDR(size) free(size)
#define FREE_SMEM(size) free(size)	
#else
#define MALLOC_DDR(size) malloc(size)
#define MALLOC_SMEM(size) malloc(size)

#define FREE_DDR(size) free(size)
#define FREE_SMEM(size) free(size)
#endif

#define PLATFORM_FPRINTF_BUFFERSIZE (500)

#define MAX_MSG_SIZE_IO (10*1024)
#define MAX_MSG_SIZE_CC (1*1024)

#define DATAMEM_SEGMENT_ID (600)
#define JOBTAB_SEGMENT_ID (601)

#define ACTIVE_MESSAGE_SEGMENT_BASE_ID (620)

#define TRACE_SEGMENT_BASE_ID (580)

#define TRACE_MSG_SIZE (32)
#define TRACE_BUFFERSIZE (1000*1000* TRACE_MSG_SIZE) // jusqu'à 1 million d'acteur par LRT en une execution

#ifdef __k1io__
#define ARCHI_SIZE_LRT 					(64*1024)
#define ARCHI_BASE_SIZE_SPIDER 			(92*1024)
#define ARCHI_COMPLEMETARY_SIZE_SPIDER	(4*1024)
#endif

#define NPE_TYPE (2)

#define IO_TYPE	(0)
#define CC_TYPE	(1)


unsigned char **buffer_fifoLRTtoSpider;
unsigned char **buffer_fifoSpidertoLRT;

static mppa_async_segment_t fifoLRTtoSpider_temp, fifoSpidertoLRT_temp;

static char buffer[PLATFORM_FPRINTF_BUFFERSIZE];
static struct timespec start;

static void* notif_tab;
static void* dataMem;
static void* traceBuffer;

static SharedMemArchi* archi_;

#if __k1io__
static Notif_t notifTab[4];
#endif

static void setAffinity(int cpuId){
	// not supported
}

PlatformMPPA::PlatformMPPA(int nLrtIo, int nLrtCc, int nbCc, int shMemSize, lrtFct* fcts, int nLrtFcts, SpiderStackConfig &stackConfig){

	if (platform_) throw "Try to create 2 platforms";
	platform_ = this;

	nLrtIo_ = nLrtIo;
	nLrtCc_ = nLrtCc;
	nbCc_ = nbCc;
	nLrt_ = nLrtIo_ + nLrtCc_ * nbCc_;


	#ifdef __k1io__
	
	#ifdef VERBOSE
	printf("Starting Spider with %d LRT on IOs, %d LRT in each cluster, on %d cluster(s), for a total of %d LRTs\n",nLrtIo_,nLrtCc_,nbCc_,nLrt_);
	#endif

	/* used only for master */
	stackPisdf = 0;
	stackSrdag = 0;
	stackTransfo = 0;
	#endif


	#ifdef __k1io__
	nLrtLocal_ = nLrtIo_;
	#else
	nLrtLocal_ = nLrtCc_;
	#endif

	
	//Allocating buffer for every fifoLRTtoSpider queues
	#ifdef __k1io__
	buffer_fifoLRTtoSpider = (unsigned char**) MALLOC_SMEM(nLrt_ * sizeof(char*));
	assert(buffer_fifoLRTtoSpider != NULL && "buffer_fifoLRTtoSpider alloc failed");

	for (int i = 0; i < nLrt_; i++) {
		buffer_fifoLRTtoSpider[i] = (unsigned char*) MALLOC_SMEM(QUEUE_LRT_TO_SPIDER_SIZE * sizeof(char));

		assert(buffer_fifoLRTtoSpider[i] != NULL && "buffer_fifoLRTtoSpider[i] alloc failed");
	}
	#endif


	buffer_fifoSpidertoLRT = (unsigned char**) MALLOC_SMEM(nLrtLocal_ * sizeof(char*));
	assert(buffer_fifoSpidertoLRT != NULL && "buffer_fifoLRTtoSpider alloc failed");

	for (int i = 0; i < nLrtLocal_; i++) {
		//printf("%d/%d\n",i,nLrtLocal_);
		buffer_fifoSpidertoLRT[i] = (unsigned char*) MALLOC_SMEM(QUEUE_SPIDER_TO_LRT_SIZE * sizeof(char));
		assert(buffer_fifoSpidertoLRT[i] != NULL && "buffer_fifoLRTtoSpider[i] alloc failed");
	}


	/* alloc for nodes */
	stackArchi = (Stack**) MALLOC_DDR(nLrtLocal_*sizeof(Stack*));
	assert(stackArchi != NULL && "stackArchi alloc failed");

	stackLrt = (Stack**) MALLOC_DDR(nLrtLocal_*sizeof(Stack*));
	assert(stackLrt != NULL && "stackLrt alloc failed");

	lrt_ = (LRT**) MALLOC_SMEM(nLrtLocal_*sizeof(LRT*));
	assert(lrt_ != NULL && "lrt_ alloc failed");

	lrtCom_ = (LrtCommunicator**) MALLOC_SMEM(nLrtLocal_*sizeof(MPPALrtCommunicator*));
	assert(lrtCom_ != NULL && "lrtCom_ alloc failed");


	thread_ID_tab_ = (int*) MALLOC_SMEM(nLrt_*sizeof(int));
	assert(thread_ID_tab_ != NULL && "thread_ID_tab_ alloc failed");


	//Global stacks initialisation
	#ifdef __k1io__
	StackMonitor::initStack(PISDF_STACK, pisdfStack);
	StackMonitor::initStack(SRDAG_STACK, srdagStack);
	StackMonitor::initStack(TRANSFO_STACK, transfoStack);
	#endif

	//allocation des segments
	#ifdef __k1io__
	int nLrtQueues = nLrtIo_ + nbCc*nLrtCc_;
	#else
	int nLrtQueues = nLrtCc_;
	#endif

	fifoLRTtoSpider = (mppa_async_segment_t*) MALLOC_SMEM(nLrtQueues * sizeof(mppa_async_segment_t));
	assert(fifoLRTtoSpider != NULL && "fifoLRTtoSpider alloc failed");

	fifoSpidertoLRT = (mppa_async_segment_t*) MALLOC_SMEM(nLrtQueues * sizeof(mppa_async_segment_t));
	assert(fifoSpidertoLRT != NULL && "fifoSpidertoLRT alloc failed");


	#ifdef __k1io__
	// 1 segment per cluster
	trace_rdma_segment_ = (mppa_async_segment_t*) MALLOC_SMEM(nbCc_ * sizeof(mppa_async_segment_t));
	#else
	// 1 segment inside the cluster
	trace_rdma_segment_ = (mppa_async_segment_t*) MALLOC_SMEM(sizeof(mppa_async_segment_t));
	#endif
	assert(trace_rdma_segment_ != NULL && "IO : trace_rdma_segment_ alloc failed");
	

	//declaration des threads et structures de passage de paramètre
	#ifdef VERBOSE
	printf("Allocation des threads et structures de passage de paramètre\n");
	#endif

	pthread_t* thread_lrt = (pthread_t*) MALLOC_SMEM((nLrtLocal_ - 1)*sizeof(pthread_t));
	assert(thread_lrt != NULL && "thread_lrt alloc failed");

	Arg_lrt* arg_lrt = (Arg_lrt*) MALLOC_SMEM((nLrtLocal_)*sizeof(Arg_lrt));
	assert(arg_lrt != NULL && "arg_lrt alloc failed");


	// local synchronization array
	#ifdef __k1io__

	notif_tab = &notifTab;
	memset(notif_tab, 0, sizeof(notifTab));
	#else
	posix_memalign(&notif_tab,8,(nLrtLocal_) * sizeof(Notif_t));
	memset(notif_tab,0,nLrtLocal_ * sizeof(Notif_t));
	#endif


	__builtin_k1_wpurge();
	__builtin_k1_fence();

	assert(notif_tab != NULL && "notif_tab failed to alloc");


	// allocating synchronization segments
	notif_segment_ = (mppa_async_segment_t*) MALLOC_SMEM((nbCc_+1) * sizeof(mppa_async_segment_t));
	assert(notif_segment_ != NULL && "notif_segment_ failed to alloc");


	// creating and cloning every cluster/IO synchronization semgents
#ifdef __k1io__
	
	#ifdef VERBOSE_SEGMENT_ID
	printf("IO%d : Creating notif_segment_[%d] @%p with ID %d\n",__k1_get_cpu_id(), nbCc_, &notif_segment_[nbCc_], ACTIVE_MESSAGE_SEGMENT_BASE_ID+nbCc_);
	#endif

	if(mppa_async_segment_create(&notif_segment_[nbCc_],ACTIVE_MESSAGE_SEGMENT_BASE_ID+nbCc_,notif_tab,(nLrtLocal_+1)*sizeof(Notif_t),0,0,NULL) != 0)
	{
		printf("IO%d segment %d failed to create\n", __k1_get_cluster_id(), ACTIVE_MESSAGE_SEGMENT_BASE_ID+nbCc_);
		assert(0);
	}
	
	for(int i = 0; i < nbCc_; i++)
	{
		#ifdef VERBOSE_SEGMENT_ID
		printf("IO%d : Cloning notif_segment_[%d] @%p with ID %d\n",__k1_get_cpu_id(), i, &notif_segment_[i], ACTIVE_MESSAGE_SEGMENT_BASE_ID+i);
		#endif
		if(mppa_async_segment_clone(&notif_segment_[i],ACTIVE_MESSAGE_SEGMENT_BASE_ID+i,NULL, 0, NULL) != 0)
		{
			printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), ACTIVE_MESSAGE_SEGMENT_BASE_ID+i);
			assert(0);
		}
	}
	#ifdef VERBOSE_SEGMENT_ID
	printf("IO%d Segment creation for active messages ok\n", __k1_get_cluster_id());
	#endif

#else

	#ifdef VERBOSE_SEGMENT_ID
	printf("CC%d : Creating notif_segment_[%d] @%p with ID %d\n",__k1_get_cluster_id(), __k1_get_cluster_id(), &notif_segment_[__k1_get_cluster_id()], ACTIVE_MESSAGE_SEGMENT_BASE_ID+__k1_get_cluster_id());
	#endif
	if(mppa_async_segment_create(&notif_segment_[__k1_get_cluster_id()],ACTIVE_MESSAGE_SEGMENT_BASE_ID+__k1_get_cluster_id(),notif_tab,nLrtLocal_*sizeof(Notif_t),0,0,NULL) != 0)
	{
		printf("CC%d segment %d failed to create\n", __k1_get_cluster_id(), ACTIVE_MESSAGE_SEGMENT_BASE_ID+__k1_get_cluster_id());
		assert(0);
	}

	for(int i = 0; i < nbCc_+1; i++){
		if(i == __k1_get_cluster_id()) continue;
		
		#ifdef VERBOSE_SEGMENT_ID
		printf("CC%d : Cloning notif_segment_[%d] @%p with ID %d\n",__k1_get_cluster_id(), i, &notif_segment_[i], ACTIVE_MESSAGE_SEGMENT_BASE_ID+i);
		#endif
		if(mppa_async_segment_clone(&notif_segment_[i],ACTIVE_MESSAGE_SEGMENT_BASE_ID+i,NULL, 0, NULL) != 0)
		{
			printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), TRACE_SEGMENT_BASE_ID);
			assert(0);
		}
	}
	#ifdef VERBOSE_SEGMENT_ID
	printf("CC%d Segment creation for active messages ok\n", __k1_get_cluster_id());
	#endif
#endif

		#ifdef __k1io__

		utask_smem_posix_memalign((void**) &jobTab_, 64, nLrt_*sizeof(long long));
		memset(jobTab_, 0, 	sizeof(long long)*nLrt_);

		assert(jobTab_ != NULL && "jobTab_ failed to alloc");

		
		// Allocating working buffer in DDR
		dataMem = MALLOC_DDR(shMemSize);
		assert(dataMem != NULL && "dataMem failed to alloc");
		memset(dataMem, 0, shMemSize);


		//allocating 1 trace buffer for each cluster + 1 buffer for IOs + 1 buffer for spider
		traceBuffer = (void*) MALLOC_DDR(TRACE_BUFFERSIZE * (nbCc_+1+1));
		memset(traceBuffer, -1, TRACE_BUFFERSIZE * (nbCc_+1+1));
		
		__builtin_k1_wpurge(); // write memory barrier
		__builtin_k1_fence();

		if(mppa_async_segment_create(&dataMem_segment_,DATAMEM_SEGMENT_ID,dataMem,shMemSize - sizeof(unsigned int)*nLrt_,0,0,NULL) != 0)
		{
			printf("CC%d segment %d failed to create\n", __k1_get_cluster_id(), DATAMEM_SEGMENT_ID);
			assert(0);
		}
		if(mppa_async_segment_create(&jobTab_segment_,JOBTAB_SEGMENT_ID,jobTab_,sizeof(long long)*nLrt_,0,0,NULL) != 0)
		{
			printf("CC%d segment %d failed to create\n", __k1_get_cluster_id(), JOBTAB_SEGMENT_ID);
			assert(0);
		}

		//trace IO side
		for (int i = 0; i < nbCc_; i++) //traceBuffer[nbCc_+1+1] is untouched for IOs and spider
		{
			if(mppa_async_segment_create(&trace_rdma_segment_[i],TRACE_SEGMENT_BASE_ID+i, (void*) ((char*)traceBuffer + (TRACE_BUFFERSIZE*i)),TRACE_BUFFERSIZE,0,0,NULL) != 0)
			{
				printf("CC%d segment %d failed to create\n", __k1_get_cluster_id(), TRACE_SEGMENT_BASE_ID+i);
				assert(0);
			}
		}

		#else

	if(mppa_async_segment_clone(&dataMem_segment_,DATAMEM_SEGMENT_ID,NULL, 0, NULL) != 0)
	{
		printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), DATAMEM_SEGMENT_ID);
		assert(0);
	}
	if(mppa_async_segment_clone(&jobTab_segment_,JOBTAB_SEGMENT_ID,NULL, 0, NULL) != 0)
	{
		printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), JOBTAB_SEGMENT_ID);
		assert(0);
	}

	//trace cluster side
	if(mppa_async_segment_clone(trace_rdma_segment_,TRACE_SEGMENT_BASE_ID + __k1_get_cluster_id(),NULL, 0, NULL) != 0)
	{
		printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), TRACE_SEGMENT_BASE_ID);
		assert(0);
	}

	#endif


	//remplissage sturcture de passage de parametre dans les threads
	#ifdef VERBOSE
	printf("[Cluster %d] Remplissage sturcture de passage de parametre dans les threads\n",__k1_get_cluster_id());
	#endif
	for (int i = 0; i<nLrtLocal_; i++)
	{
		arg_lrt[i].fcts = fcts;
		arg_lrt[i].nLrtFcts = nLrtFcts;
		arg_lrt[i].indice = i;
		arg_lrt[i].instance = this;
		arg_lrt[i].archiStack = archiStack;
		arg_lrt[i].lrtStack = lrtStack;
	}


	//tableau de correspondance entre lrtId et cpuId
	for (int i = 0; i<nLrtIo_; i++){
		thread_ID_tab_[i] = IO_CPUID_OFFSET + i;
	}
	

	for (int i = 0; i<nbCc_; i++){
		for (int j = 0; j<nLrtCc_; j++){
			thread_ID_tab_[i*nLrtCc_+j + nLrtIo_] = i * 16 + j; 
		}
	}

	#ifdef __k1dp__
	// init barrier used in lrts in cc
	pthread_barrier_init(&lrtCcBarrier_, NULL, nLrtCc_);
	#endif

	// Because rpc server is on IO1 so LRT1 is on IO2 and LRT2 on IO3
	for(int i = 1; i < nLrtIo_; i++) thread_ID_tab_[i]++;


	#ifdef VERBOSE
	printf("Lancement des threads\n");
	#endif
	//Lancement des threads
	for (int i = 1; i<nLrtLocal_; i++){
		#ifdef __k1io__
			#ifdef VERBOSE
			printf("IO%d create thread %d\n", __k1_get_cluster_id()/192, i);
			#endif
			pthread_create(&thread_lrt[i - 1], NULL, &lrtPThreads_helper, &arg_lrt[i - 1]);
		#else
			#ifdef VERBOSE
			printf("[Cluster %d] create thread %d\n", __k1_get_cluster_id(), i);
			#endif
			pthread_create(&thread_lrt[i-1], NULL, &lrtPThreads_helper, &arg_lrt[i]);
		#endif
	}
	#ifdef __k1dp__
		lrtPThreads(&arg_lrt[0]); // for loop on pthread create when several Lrt per cluster
		#ifdef VERBOSE
		printf("[Cluster %d] lrtPThreads Done !!\n", __k1_get_cluster_id());
		#endif
	#endif

	/* END OF CLUSTER SIDE*/

#ifdef __k1io__

	#ifdef VERBOSE
	printf("IO%d : Declaration des stacks spécific au thread\n",__k1_get_cpu_id());
	#endif


	//Declaration des stacks spécific au thread
	archiStack.size = ARCHI_BASE_SIZE_SPIDER + (ARCHI_COMPLEMETARY_SIZE_SPIDER*nLrt_);
	StackMonitor::initStack(ARCHI_STACK, archiStack);

	lrtStack.size /= nLrtIo_;
	StackMonitor::initStack(LRT_STACK, lrtStack);


	// CREATING SEGMENT
	// IO SIDE

	//Creating lrt to spider queues for slaves
	for(int i = 0; i<nLrtIo_; i++){
		#ifdef VERBOSE_SEGMENT_ID
		printf("IO%d : Creating fifoLRTtoSpider[%d] @%p with ID %d\n",__k1_get_cpu_id(), i, &fifoLRTtoSpider[i], getLrtId(thread_ID_tab_[i])*2+1);
		#endif
		if(mppa_async_segment_create(&fifoLRTtoSpider[i],getLrtId(thread_ID_tab_[i])*2 + 1, buffer_fifoLRTtoSpider[i], QUEUE_LRT_TO_SPIDER_SIZE, MPPA_ASYNC_SEGMENT_FLAG_QUEUE0, 0, NULL) != 0)
		{
			printf("IO%d : failed creating segment %d\n",__k1_get_cpu_id(), getLrtId(thread_ID_tab_[i])*2 + 1);
			assert(0);
		}
	}
	//creating spider to LRT Master queue
	#ifdef VERBOSE_SEGMENT_ID
	printf("IO%d : Creating fifoSpidertoLRT_temp @%p with ID %d \n",__k1_get_cpu_id(), &fifoSpidertoLRT_temp, getLrtId(IO_CPUID_OFFSET));
	#endif
	if(mppa_async_segment_create(&fifoSpidertoLRT_temp, getLrtId(IO_CPUID_OFFSET), buffer_fifoSpidertoLRT[0], QUEUE_SPIDER_TO_LRT_SIZE, MPPA_ASYNC_SEGMENT_FLAG_QUEUE0, 0, NULL) != 0)
	{
		printf("IO%d : failed creating segment %d\n",__k1_get_cpu_id(), getLrtId(IO_CPUID_OFFSET));
		assert(0);
	}


	// CLUSTER SIDE
	int k = nLrtIo_;

	#define NB_CC_COLUMN (4)
	int indirection_table[NB_CC_COLUMN] = { MPPA_ASYNC_SEGMENT_FLAG_QUEUE0, MPPA_ASYNC_SEGMENT_FLAG_QUEUE1, MPPA_ASYNC_SEGMENT_FLAG_QUEUE2, MPPA_ASYNC_SEGMENT_FLAG_QUEUE3 };

	for(int i = 0; i<nbCc_; i++){
		for(int j = 0; j<nLrtCc_; j++){
			#ifdef VERBOSE_SEGMENT_ID
			printf("IO%d : Creating fifoLRTtoSpider[%d] @%p with ID %d\n",__k1_get_cpu_id(), k+(i*nLrtCc_+j), &fifoLRTtoSpider[k+(i*nLrtCc_+j)], getLrtId((16*i+j))*2 + 1);
			#endif
			if(mppa_async_segment_create(&fifoLRTtoSpider[k+(i*nLrtCc_+j)], getLrtId((16*i+j))*2 + 1, buffer_fifoLRTtoSpider[k+(i*nLrtCc_+j)], QUEUE_LRT_TO_SPIDER_SIZE, indirection_table[i%NB_CC_COLUMN], 0, NULL) != 0)
			{
				printf("IO%d : failed creating segment %d\n",__k1_get_cpu_id(), getLrtId((16*i+j))*2 + 1);
				assert(0);
			}
		}
	}


	// CLONING SEGMENT
	// IO SIDE

	//cloning queue Spider to LRT queues
	#ifdef VERBOSE_SEGMENT_ID
	printf("IO%d : Cloning IO queues\n",__k1_get_cpu_id());
	#endif
	for(int i = 0; i<nLrtIo_; i++){
		#ifdef VERBOSE_SEGMENT_ID
		printf("IO%d : Cloning fifoSpidertoLRT[%d] @%p with ID %d \n",__k1_get_cpu_id(), i, &fifoSpidertoLRT[i], getLrtId(thread_ID_tab_[i])*2);
		#endif
		if(mppa_async_segment_clone(&fifoSpidertoLRT[i], getLrtId(thread_ID_tab_[i])*2, NULL, 0, NULL) != 0)
		{
			printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), TRACE_SEGMENT_BASE_ID);
			assert(0);
		}
	}
	////cloning LRT Master to spider queue
	#ifdef VERBOSE_SEGMENT_ID
	printf("IO%d : Cloning fifoLRTtoSpider_temp @%p with ID %d \n",__k1_get_cpu_id(), &fifoLRTtoSpider_temp, getLrtId(IO_CPUID_OFFSET) + 1);
	#endif
	if(mppa_async_segment_clone(&fifoLRTtoSpider_temp, getLrtId(IO_CPUID_OFFSET) + 1, NULL, 0, NULL) != 0)
	{
		printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), getLrtId(IO_CPUID_OFFSET) + 1);
		assert(0);
	}

	// CLUSTER SIDE
	k = nLrtIo_;
	#ifdef VERBOSE_SEGMENT_ID
	printf("IO%d : Cloning CC queues\n",__k1_get_cpu_id());
	#endif
	for(int i = 0; i<nbCc_; i++){
		for(int j = 0; j<nLrtCc_; j++){
			#ifdef VERBOSE_SEGMENT_ID
			printf("IO%d : Cloning fifoSpidertoLRT[%d] @%p with ID %d \n",__k1_get_cpu_id(), k+(i*nLrtCc_+j), &fifoSpidertoLRT[k+(i*nLrtCc_+j)], getLrtId((16*i+j))*2);
			#endif
			if(mppa_async_segment_clone(&fifoSpidertoLRT[k+(i*nLrtCc_+j)], getLrtId((16*i+j))*2, NULL, 0, NULL) != 0)
			{
				printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), getLrtId((16*i+j))*2);
				assert(0);
			}
		}
	}


	#ifdef VERBOSE
	printf("IO%d : Initialize Spider Communicator\n",__k1_get_cpu_id());
	#endif
	spiderCom_ = CREATE(ARCHI_STACK, MPPASpiderCommunicator)(
		MAX_MSG_SIZE_IO,
		nLrt_,
		(void*)(((char*)traceBuffer)+((nbCc_+1)*TRACE_BUFFERSIZE)),
		traceBuffer
		);

	for (int i = 0; i<nLrt_; i++){
		((MPPASpiderCommunicator*)spiderCom_)->setLrtCom(i, &fifoLRTtoSpider[i], &fifoSpidertoLRT[i]);
	}

	#ifdef VERBOSE
	printf("IO%d : Initialize LRT Communicator\n",__k1_get_cpu_id());
	#endif
	lrtCom_[0] = CREATE(ARCHI_STACK, MPPALrtCommunicator)(
		MAX_MSG_SIZE_IO,
		&fifoSpidertoLRT_temp,
		&fifoLRTtoSpider_temp,
		notif_tab,
		(void*)(((char*)traceBuffer)+(nbCc_*TRACE_BUFFERSIZE)),
		notif_segment_,
		&jobTab_segment_,
		&dataMem_segment_,
		NULL,
		this
		);

	#ifdef VERBOSE
	printf("IO%d : Initialize LRT\n",__k1_get_cpu_id());
	#endif

	lrt_[0] = CREATE(ARCHI_STACK, LRT)(0);


	setAffinity(0);
	lrt_[0]->setFctTbl(fcts, nLrtFcts);

	#ifdef VERBOSE
	printf("IO%d : Create Archi\n",__k1_get_cpu_id());
	#endif
	
	char name[50];
	
	/** Create Archi */
	archi_ = CREATE(ARCHI_STACK, SharedMemArchi)(
		/* Nb PE */		nLrt_,
		/* Nb PE Type*/ NPE_TYPE,
		/* Spider Pe */ 0,
		/*MappingTime*/ this->mappingTime);

	sprintf(name, "TID %ld (LRT %d)", thread_ID_tab_[0], 0);

	archi_->setPETypeRecvSpeed(0, 1, 10);
	archi_->setPETypeSendSpeed(0, 1, 10);
	archi_->setPEType(0, IO_TYPE);
	archi_->setName(0, name);
	archi_->activatePE(0);

	//printf("IO%d : Create Archi name\n",__k1_get_cpu_id());

	for (int i = 1; i<nLrtIo_; i++){
		sprintf(name, "TID %ld (LRT %d)", thread_ID_tab_[i], i);
		archi_->setPEType(i, IO_TYPE);
		archi_->setName(i, name);
		archi_->activatePE(i);
	}


	for (int i = nLrtIo_; i<nLrt_; i++){
		sprintf(name, "TID %ld (LRT %d)", thread_ID_tab_[i], i);
		archi_->setPEType(i, CC_TYPE);
		archi_->setName(i, name);
		archi_->activatePE(i);
	}
	
	
	Spider::setArchi(archi_);

	FREE_SMEM(thread_lrt);
	FREE_SMEM(arg_lrt);

	this->rstTime();
	#ifdef VERBOSE
	printf("Platform Creation end\n");
	#endif
#endif
}

PlatformMPPA::~PlatformMPPA(){

#ifdef __k1io__


	for (int lrt = 1; lrt<archi_->getNPE(); lrt++){
		int size = sizeof(StopLrtMsg);
		StopLrtMsg* msg = (StopLrtMsg*)getSpiderCommunicator()->ctrl_start_send(lrt, size);
		msg->msgIx = MSG_STOP_LRT;

		#ifdef VERBOSE
		printf("Sending MSG_STOP_LRT to LRT%d\n",lrt);
		#endif

		getSpiderCommunicator()->ctrl_end_send(lrt, size);
	}

	// Waiting for each Thread to finish
	int *temp;

	for (int i = 1; i < nLrt_; i++) {

		mppa_async_dequeue(&fifoLRTtoSpider[i], sizeof(int), (void**) &temp, NULL);
		
		if (*temp != 0xDEADBEEF){
			printf("Error when shutting down LRT %d\n",i);
			while(1);
		}

		*temp = 0;
		__builtin_k1_wpurge();
	}
	__builtin_k1_fence();

	#ifdef VERBOSE
	printf("IO%d : Received all dead beefs\n",__k1_get_cpu_id());
	#endif

	/* wait compute cluster exit */
	int status = 0;
	for (int cluster = 0; cluster < nbCc_; cluster++){	
		int ret;
        if (mppa_power_base_waitpid(cluster, &ret, 0) < 0) {
                printf("# [IODDR0] Waitpid failed on cluster %d\n", cluster);
        }
        status += ret;
	}
	assert(status == 0 && "Compute cluster status wrong value\n");

	#ifdef VERBOSE
	printf("IO%d : All threads done\n",__k1_get_cpu_id());
	#endif
#endif

	// Destroying LRTs
	for (int i = 0; i < nLrtLocal_; i++)
	{
		lrt_[i]->~LRT();
	}

	#ifdef __k1io__
	//Destroying Communicators
	((MPPASpiderCommunicator*)spiderCom_)->~MPPASpiderCommunicator();
	for (int i = 0; i < nLrtIo_; i++)
	{
		((MPPALrtCommunicator*)lrtCom_[i])->~MPPALrtCommunicator();
	}

	archi_->~SharedMemArchi();

	StackMonitor::free(ARCHI_STACK, lrt_[0]);
	StackMonitor::free(ARCHI_STACK, lrtCom_[0]);
	StackMonitor::free(ARCHI_STACK, spiderCom_);
	StackMonitor::free(ARCHI_STACK, archi_);

	//WARNING : Thread specific stacks have to be cleaned BEFORE exiting threads
	StackMonitor::cleanAllStack();

	//Destroying segment
	for(int i = 0; i<nLrt_; i++)
	{
		if(mppa_async_segment_destroy(&fifoLRTtoSpider[i]) != 0)
		{
			printf("CC%d : failed destroy segment %s %d\n",__k1_get_cluster_id(), __FILE__, __LINE__);
			assert(0);
		}
	}
	if(mppa_async_segment_destroy(&fifoSpidertoLRT_temp) != 0)
	{
		printf("CC%d : failed destroy segment %s %d\n",__k1_get_cluster_id(), __FILE__, __LINE__);
		assert(0);
	}

	for (int i = 0; i < nbCc_; i++)
	{
		if(mppa_async_segment_destroy(&trace_rdma_segment_[i]) != 0)
		{
			printf("CC%d : failed destroy segment %s %d\n",__k1_get_cluster_id(), __FILE__, __LINE__);
			assert(0);
		}
	}

	#endif


	//Desallocation des tableaux dynamiques
	FREE_SMEM(lrt_);
	FREE_SMEM(lrtCom_);
	FREE_DDR(stackArchi);
	FREE_DDR(stackLrt);


	#ifdef __k1io__
	for (int i = 0; i < nLrt_; i++) FREE_SMEM(buffer_fifoLRTtoSpider[i]);
	FREE_SMEM(buffer_fifoLRTtoSpider);
	#endif


	for (int i = 0; i < nLrtLocal_; i++) FREE_SMEM(buffer_fifoSpidertoLRT[i]);
	FREE_SMEM(buffer_fifoSpidertoLRT);




	#ifdef __k1io__
	FREE_DDR(traceBuffer);
	FREE_DDR(dataMem);

	utask_smem_free(jobTab_);
	#else
	#endif

	FREE_SMEM(fifoSpidertoLRT);
	FREE_SMEM(fifoLRTtoSpider);

	FREE_SMEM(thread_ID_tab_);
	FREE_SMEM(trace_rdma_segment_);

	FREE_SMEM(notif_segment_);

}

/** File Handling */
FILE* PlatformMPPA::fopen(const char* name){

	return std::fopen(name, "w+");
}

void PlatformMPPA::fprintf(FILE* id, const char* fmt, ...){
#if 1

	va_list ap;
	va_start(ap, fmt);

	int n = vsnprintf(buffer, PLATFORM_FPRINTF_BUFFERSIZE, fmt, ap);

	if (n >= PLATFORM_FPRINTF_BUFFERSIZE){
		printf("PLATFORM_FPRINTF_BUFFERSIZE too small\n");
	}

	for (int i = 0; i < n; i++) fputc(buffer[i], id);

#endif
}

void PlatformMPPA::fclose(FILE* id){
	if (id != NULL){
		std::fclose(id);
		id = NULL;
	}
}

void* PlatformMPPA::virt_to_phy(void* address){
	return (void*)((long)dataMem + (long)address);
}

int PlatformMPPA::getCacheLineSize(){
	return 0;
}

int PlatformMPPA::getMinAllocSize(){
	#ifdef __k1io__
	return 8;
	#else
	return 4;
	#endif
}


void PlatformMPPA::rstJobIx(){

	//Sending a msg to all slave LRTs, end of graph iteration
	for (int i = 1;i < nLrt_;i++){
		EndIterMsg* msg = (EndIterMsg*) getSpiderCommunicator()->ctrl_start_send(i, sizeof(EndIterMsg));
		msg->msgIx = MSG_END_ITER;
		getSpiderCommunicator()->ctrl_end_send(i, sizeof(EndIterMsg));
	}

	//Waiting for slave LRTs to finish their job queue
	for (int i = 1;i < nLrt_;i++){
		void* msg = NULL;

		do{
			while(getSpiderCommunicator()->ctrl_start_recv(i,&msg) == 0); //lrt_[0]->runOneJob();
		}
		while (((UndefinedMsg*) msg)->msgIx != MSG_END_ITER);
		getSpiderCommunicator()->ctrl_end_recv(i);
	}

	//Sending a msg to all slave LRTs, reset jobIx counter
	for (int i = 1;i < nLrt_;i++){
		ResetLrtMsg* msg = (ResetLrtMsg*) getSpiderCommunicator()->ctrl_start_send(i, sizeof(ResetLrtMsg));
		msg->msgIx = MSG_RESET_LRT;
		getSpiderCommunicator()->ctrl_end_send(i, sizeof(ResetLrtMsg));
	}

	//reseting master LRT jobIx counter
	Platform::get()->getLrt()->rstJobIx();

	//reseting local copy of jobIx in jobTab_
	rstLrtJobIx();	

	//Waiting for slave LRTs to reset their jobIx counter
	for (int i = 1;i < nLrt_;i++){
		void* msg = NULL;
		do{
			while(getSpiderCommunicator()->ctrl_start_recv(i,&msg) == 0);
		}
		while (((UndefinedMsg*) msg)->msgIx != MSG_RESET_LRT);
		getSpiderCommunicator()->ctrl_end_recv(i);
	}
}


/** Time Handling */
void PlatformMPPA::rstTime(struct ClearTimeMsg* msg){
	struct timespec* ts = (struct timespec*)(msg + 1);
	start = *ts;
}

void PlatformMPPA::rstTime(){

	long long val_steady = __k1_read_dsu_timestamp() / (CHIP_FREQ * 1000);

	start.tv_sec = val_steady / (1000*1000*1000);
	start.tv_nsec = val_steady - start.tv_sec;

	for (int lrt = 1; lrt < nLrt_; lrt++){
		int size = sizeof(ClearTimeMsg)+sizeof(struct timespec);
		ClearTimeMsg* msg = (ClearTimeMsg*)getSpiderCommunicator()->ctrl_start_send(lrt, size);
		struct timespec* ts = (struct timespec*)(msg + 1);

		msg->msgIx = MSG_CLEAR_TIME;
		*ts = start;
	
		
		getSpiderCommunicator()->ctrl_end_send(lrt, size);
	}
}

Time PlatformMPPA::getTime(){
	Time val_steady = __k1_read_dsu_timestamp();// / CHIP_FREQ;

	if (val_steady == 0){
		printf("TIME ISSUE\n");
		return 1;
	}
	else
		return val_steady;
}

void PlatformMPPA::idleLrt(int lrt){
	//Can't be done easily
	//lrt_[lrt]->setIdle(true);
}

void PlatformMPPA::wakeLrt(int lrt){
	//Can't be done easily
	//lrt_[lrt]->setIdle(false);
}

void PlatformMPPA::idle(){
	while (lrt_[0]->isIdle()){
		sleep((unsigned)-1);
	}
}

Time PlatformMPPA::mappingTime(int nActors, int nPE){

	return 10 * nActors * nPE;
}

void PlatformMPPA::lrtPThreads(Arg_lrt *argument_lrt){

	int indice = argument_lrt->indice;

	#ifdef __k1io__
		indice++;		// rustine de merde parce que dans les IO, l'indice doit commencer à 1, 0 = master
	#endif
	
	mppa_async_segment_t fifoSpidertoLRT, fifoLRTtoSpider;


#ifdef __k1io__
	//Each thread create its incoming queue
	#ifdef VERBOSE_SEGMENT_ID
	printf("IO%d : Creating fifoSpidertoLRT @%p with ID %d \n", __k1_get_cpu_id(), &fifoSpidertoLRT, getLrtId(IO_CPUID_OFFSET + __k1_get_cpu_id())*2);
	#endif
	if(mppa_async_segment_create(&fifoSpidertoLRT, getLrtId(IO_CPUID_OFFSET + __k1_get_cpu_id())*2, buffer_fifoSpidertoLRT[indice], QUEUE_SPIDER_TO_LRT_SIZE, MPPA_ASYNC_SEGMENT_FLAG_QUEUE0, 0, NULL) != 0)
	{
		printf("CC%d : failed creating segment %d\n",__k1_get_cluster_id(), getLrtId(IO_CPUID_OFFSET + __k1_get_cpu_id())*2);
		assert(0);
	}

	
	//clone lrt to spider queue
	#ifdef VERBOSE_SEGMENT_ID
	printf("IO%d : Cloning fifoLRTtoSpider @%p with ID %d \n", __k1_get_cpu_id(), &fifoLRTtoSpider, getLrtId(IO_CPUID_OFFSET + __k1_get_cpu_id())*2+1);
	#endif
	if(mppa_async_segment_clone(&fifoLRTtoSpider, getLrtId(IO_CPUID_OFFSET + __k1_get_cpu_id())*2+1, NULL, 0, NULL) != 0)
	{
		printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), getLrtId(IO_CPUID_OFFSET + __k1_get_cpu_id())*2+1);
		assert(0);
	}
#else
	#ifdef VERBOSE_SEGMENT_ID
	printf("[Cluster %d] PE%d : Creating fifoSpidertoLRT @%p with ID %d \n",__k1_get_cluster_id(),__k1_get_cpu_id(), &fifoSpidertoLRT, getLrtId((__k1_get_cluster_id()*16+__k1_get_cpu_id()))*2);
	#endif
	if(mppa_async_segment_create(&fifoSpidertoLRT, getLrtId((__k1_get_cluster_id()*16+__k1_get_cpu_id()))*2, buffer_fifoSpidertoLRT[indice], QUEUE_SPIDER_TO_LRT_SIZE, MPPA_ASYNC_SEGMENT_FLAG_QUEUE0, 0, NULL) != 0)
	{
		printf("CC%d : failed creating segment %d\n",__k1_get_cluster_id(), getLrtId((__k1_get_cluster_id()*16+__k1_get_cpu_id()))*2);
		assert(0);
	}

	#ifdef VERBOSE_SEGMENT_ID
	printf("[Cluster %d] PE%d : Cloning fifoLRTtoSpider @%p with ID %d \n",__k1_get_cluster_id(),__k1_get_cpu_id(), &fifoLRTtoSpider,getLrtId((__k1_get_cluster_id()*16+__k1_get_cpu_id()))*2+1);
	#endif
	if(mppa_async_segment_clone(&fifoLRTtoSpider, getLrtId((__k1_get_cluster_id()*16+__k1_get_cpu_id()))*2+1, NULL, 0, NULL) != 0)
	{
		printf("CC%d segment %d failed to clone\n", __k1_get_cluster_id(), getLrtId((__k1_get_cluster_id()*16+__k1_get_cpu_id()))*2+1);
		assert(0);
	}
#endif

	#ifdef VERBOSE
	printf("[Cluster %d] PE%d : Declaration des stacks spécific au thread\n",__k1_get_cluster_id(),__k1_get_cpu_id());
	#endif


	//Declaration des stacks spécific au thread
	#ifdef __k1io__
	
	argument_lrt->archiStack.size = ARCHI_SIZE_LRT/(nLrtLocal_ - 1);
	argument_lrt->archiStack.start = (char*)argument_lrt->archiStack.start + ARCHI_BASE_SIZE_SPIDER + (ARCHI_COMPLEMETARY_SIZE_SPIDER*nLrt_) + argument_lrt->archiStack.size * (indice-1) * sizeof(char);
	StackMonitor::initStack(ARCHI_STACK, argument_lrt->archiStack);
	#else
	
	argument_lrt->archiStack.size /= nLrtLocal_;
	argument_lrt->archiStack.start = (char*)argument_lrt->archiStack.start + argument_lrt->archiStack.size * indice * sizeof(char);
	StackMonitor::initStack(ARCHI_STACK, argument_lrt->archiStack);
	#endif

	argument_lrt->lrtStack.size /= nLrtLocal_;
	argument_lrt->lrtStack.start = (char*) argument_lrt->lrtStack.start + argument_lrt->lrtStack.size * indice * sizeof(char);
	StackMonitor::initStack(LRT_STACK, argument_lrt->lrtStack);


	/** Create LRT */
	#ifdef VERBOSE
	printf("[Cluster %d] PE%d : Initialize LRT Communicator\n",__k1_get_cluster_id(),__k1_get_cpu_id());
	#endif

#ifdef __k1io__
	lrtCom_[indice] = CREATE(ARCHI_STACK, MPPALrtCommunicator)(
		MAX_MSG_SIZE_IO,
		&fifoSpidertoLRT,
		&fifoLRTtoSpider,
		notif_tab,
		(void*)(((char*)traceBuffer)+(nbCc_*TRACE_BUFFERSIZE)),
		notif_segment_,
		&jobTab_segment_,
		&dataMem_segment_,
		NULL,
		this
		);
#else
	lrtCom_[indice] = CREATE(ARCHI_STACK, MPPALrtCommunicator)(
		MAX_MSG_SIZE_CC,
		&fifoSpidertoLRT,
		&fifoLRTtoSpider,
		notif_tab,
		NULL,
		notif_segment_,
		&jobTab_segment_,
		&dataMem_segment_,
		trace_rdma_segment_,
		this
		);
#endif

	int temp = -1;
	#if __k1io__
	temp = getLrtId(IO_CPUID_OFFSET + __k1_get_cpu_id());
	#else
	temp = getLrtId(__k1_get_cluster_id()*16+__k1_get_cpu_id());
	#endif

	#ifdef VERBOSE
	printf("[Cluster %d] PE%d : Initialize LRT with Ix %d\n",__k1_get_cluster_id(),__k1_get_cpu_id(),temp);
	#endif
	lrt_[indice] = CREATE(ARCHI_STACK, LRT)(temp);

	//setAffinity(indice);
	lrt_[indice]->setFctTbl(argument_lrt->fcts, argument_lrt->nLrtFcts);

	/** launch LRT */
	lrt_[indice]->runInfinitly();

	//freeing thread specific stacks
	StackMonitor::free(ARCHI_STACK, lrt_[indice]);
	StackMonitor::free(ARCHI_STACK, lrtCom_[indice]);

	//cleaning thread specific stacks
	StackMonitor::clean(ARCHI_STACK);
	StackMonitor::clean(LRT_STACK);

	//Each thread destroy its queue
	if(mppa_async_segment_destroy(&fifoSpidertoLRT) != 0)
	{
		printf("CC%d : failed destroy segment %s %d\n",__k1_get_cluster_id(), __FILE__, __LINE__);
		assert(0);
	}

	//Signaling to master thread that this slave thread is ending
	temp = 0xDEADBEEF;

	#ifdef VERBOSE
	printf("[Cluster %d] PE%d : Sending dead beef\n",__k1_get_cluster_id(),__k1_get_cpu_id());
	#endif

	#ifdef __k1dp__
	mppa_async_fence(MPPA_ASYNC_DDR_0, NULL);
	#endif

	mppa_async_enqueue(&fifoLRTtoSpider, &temp, sizeof(int), 0, NULL);

	#ifdef __k1dp__
	pthread_barrier_wait(&lrtCcBarrier_);
	#endif
}

void* lrtPThreads_helper(void *voidArgs) {
	Arg_lrt *args = (Arg_lrt*)voidArgs;
	args->instance->lrtPThreads(args);
	return 0;
}
