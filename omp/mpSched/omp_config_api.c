#include "omp.h"

#if defined (TI_C6636)
#define DEVICE_K2H  1
#endif

#include <ti/csl/csl_cache.h>
#include <ti/csl/csl_cacheAux.h>
#include <ti/csl/csl_msmc.h>
#include <ti/csl/csl_msmcAux.h>
#include <ti/drv/qmss/qmss_qm.h>

extern cregister volatile unsigned int DNUM;

/** \defgroup omp_config OMP Runtime Configuration Hooks */
/* @{ */

#define MSMCSRAM_START_ADDR     (0x0C000000)
#define MSMCSRAM_NC_SIZE        (0x00020000)    /* 128 KB */

#if defined (TI_C6678)
#define MSMCSRAM_SIZE           (0x00400000)    /* 4 MB */
#define MSMCSRAM_NC_START_ADDR  (0xA0000000)
#define DDR_START_ADDR          (0x80000000)
#define DDR_SIZE                (0x20000000)    /* 512 MB */
#define OMP_NUM_CORES           (8)
#define CLOCK_RATE              (1000)          /* 1000 MHz */
#elif defined (TI_C6670)
#define MSMCSRAM_SIZE           (0x00200000)    /* 2 MB */
#define MSMCSRAM_NC_START_ADDR  (0xA0000000)
#define DDR_START_ADDR          (0x80000000)
#define DDR_SIZE                (0x20000000)    /* 512 MB */
#define OMP_NUM_CORES           (4)
#define CLOCK_RATE              (983)           /* 983 MHz */
#elif defined (TI_C6657)
#define MSMCSRAM_SIZE           (0x00100000)    /* 1 MB */
#define MSMCSRAM_NC_START_ADDR  (0xA0000000)
#define DDR_START_ADDR          (0x80000000)
#define DDR_SIZE                (0x20000000)    /* 512 MB */
#define OMP_NUM_CORES           (2)
#define CLOCK_RATE              (1000)          /* 1000 MHz */
#elif defined (TI_C6636)
#define MSMCSRAM_SIZE           (0x00600000)    /* 6 MB */
#define MSMCSRAM_NC_START_ADDR  (0xE0000000)
#define DDR_START_ADDR          (0xA0000000)
#define DDR_SIZE                (0x20000000)    /* 512 MB */
#define OMP_NUM_CORES           (8)
#define CLOCK_RATE              (1000)          /* 1000 MHz */
#else
#error "Device not supported"
#endif

#define OMP_MASTER_CORE_IDX (0)

/**
 * Default reset routine. Invoked by all cores during boot, before cinit.
 *
 * Invoked before C initialization is performed - C init run addresses
 * can be in regions mapped by MPAX.
 * Annotated weak, can be overridden by a definition in application source
 * @see c_int00
 *
 * Typically performs the following operations:
 * - Sets up caches
 * - Initializes the MPAX registers for mapping memory regions
 * - Initializes the MAR registers to set attributes for memory regions
 *
 * NOTE: The addresses and sizes used here must correspond to those specified 
 * in the Platform or linker command file!
 */
void __TI_omp_reset(void)
{
    /* Configure caches */
    CACHE_setL1DSize(CACHE_L1_32KCACHE);
    CACHE_setL1PSize(CACHE_L1_32KCACHE);
    CACHE_setL2Size(CACHE_32KCACHE);

    /* OMP runtime requires a portion of MSMCSRAM to be non-cached. Since it is 
     * not possible to disable caching in the MSMCSRAM address range, we need a 
     * 2 step process:
     * 1. Map a portion of MSMCSRAM into a range that can be marked as
     *    non-cached. This is done using the MPAX register
     * 2. Annotate the mapped section as non-cached using the appropriate
     *    MAR register for that memory range
     * All accesses to MSMCSRAM through the mapped address range will not
     * be cached.
     */
    __TI_setMPAX(3, MSMCSRAM_NC_START_ADDR, 
                    MSMCSRAM_START_ADDR, 0x13 /* 1 MB */);
    __TI_omp_disable_caching(MSMCSRAM_NC_START_ADDR, MSMCSRAM_NC_SIZE);

    /* Annotate MSMCSRAM and DDR as cached + prefetch + write through */
    __TI_omp_enable_caching(MSMCSRAM_START_ADDR, MSMCSRAM_SIZE);
    __TI_omp_enable_caching(DDR_START_ADDR, DDR_SIZE);
}


/**
 * Default runtime configuration function. Called by the master core.
 *
 * It is annotated weak and can be overridden by a user provided function 
 * with the same name.
 * @see __TI_omp_config_thread_stack
 */
void __TI_omp_configure(void)
{
    __TI_omp_config_cores (OMP_MASTER_CORE_IDX, OMP_NUM_CORES);

    __TI_omp_config_hw_semaphores(/*hw_sem_base_idx=*/3);

    __TI_omp_config_clock_freq_in_mhz(CLOCK_RATE);

#ifdef TI_C6636
    __TI_omp_config_hw_queues (/* init_qmss =*/                       1,
                               /* hw_queue_base_idx=*/             7332,
                               /* first_desc_idx_in_linking_ram=*/ 8000,
                               /* first_memory_region_idx=*/         32);
#else
    __TI_omp_config_hw_queues (/* init_qmss =*/                       1,
                               QMSS_PARAM_NOT_SPECIFIED,
                               /* first_desc_idx_in_linking_ram=*/0,
                               Qmss_MemRegion_MEMORY_REGION_NOT_SPECIFIED);
#endif

        /* Thread stacks allocated from the heap */
    __TI_omp_config_thread_stack(1, 0x20000);
}

