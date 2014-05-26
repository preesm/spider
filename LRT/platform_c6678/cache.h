/*
 ============================================================================
 Name        : cache.h
 Author      : kdesnos
 Version     : 1.1
 Copyright   : CECILL-C
 Description : Parameter to configure the cache.
 ============================================================================
 */

#ifndef CACHE_H
#define CACHE_H

#include <ti/csl/csl_cacheAux.h>

/**
 * L1D Write-back invalidate operation.
 * This function implements the Advisory 7 from sprz334f to solve cache
 * coherency problem.
 * @see CACHE_wbInvL1D for more information.
 */
void cache_wbInvL1D(void* buffer, Uint32 size);

/**
 * L1D Write-back operation.
 * This function implements the Advisory 7 from sprz334f to solve cache
 * coherency problem.
 * @see CACHE_wbL1D for more information.
 */
void cache_wbL1D(void* buffer, Uint32 size);

/*!
 * L1D Invalidate operation.
 * This function implements the Advisory 7 from sprz334f to solve cache
 * coherency problem.
 * @see CACHE_invL1D for more information.
 */
void cache_invL1D(void* buffer, Uint32 size);

/**
 * L2 Write-back invalidate operation.
 * This function implements the Advisory 7 from sprz334f to solve cache
 * coherency problem.
 * @see CACHE_wbInvL2 for more information.
 */
void cache_wbInvL2(void* buffer, Uint32 size);

/**
 * L2 Write-back operation.
 * This function implements the Advisory 7 from sprz334f to solve cache
 * coherency problem.
 * @see CACHE_wbL2 for more information.
 */
void cache_wbL2(void* buffer, Uint32 size);

/**
 * L2 invalidate operation.
 * This function implements the Advisory 7 from sprz334f to solve cache
 * coherency problem.
 * @see CACHE_invL2 for more information.
 */
void cache_invL2(void* buffer, Uint32 size);

#endif
