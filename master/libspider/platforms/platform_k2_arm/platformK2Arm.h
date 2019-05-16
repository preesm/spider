/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#ifndef PLATFORM_K2_ARM_H
#define PLATFORM_K2_ARM_H

#include <platform.h>
#include <tools/Stack.h>
#include <graphs/Archi/ArchiOld.h>
#include <graphs/Archi/SharedMemArchi.h>
#include <lrt.h>

typedef enum {
    USE_MSMC = 1,
    USE_DDR = 0
} SharedMemMode;

class PlatformK2Arm : public Platform {
public:
    /** File Handling */
    virtual int fopen(const char *name);

    virtual void fprintf(int id, const char *fmt, ...);

    virtual void fclose(int id);

    /** Shared Memory Handling */
    virtual void *virt_to_phy(void *address);

    virtual int getMinAllocSize();

    virtual int getCacheLineSize();

    /** Time Handling */
    virtual void rstTime();

    virtual void rstTime(ClearTimeMsg *msg);

    virtual Time getTime();

    /** Platform Core Handling **/
    virtual void idleLrt(int i);

    virtual void wakeLrt(int i);

    virtual void idle();

    SharedMemArchi *getArchi();

    PlatformK2Arm(int nArm, int nDsp, SharedMemMode useMsmc, int shMemSize, Stack *stack, lrtFct *fcts, int nLrtFcts);

    virtual ~PlatformK2Arm();

private:
    Stack *stack_;
    SharedMemArchi *archi_;

    static Time mappingTime(int nActors);
};

#endif/*PLATFORM_K2_ARM_H*/
