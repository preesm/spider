/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2017) :
 *
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2016)
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
#ifndef PLATFORM_H
#define PLATFORM_H

#include "spider.h"
#include <monitor/StackMonitor.h>
#include <stdio.h>
#include <stdexcept>

class LRT;

class LrtCommunicator;

class SpiderCommunicator;

struct ClearTimeMsg;

class Platform {
public:
    /** File Handling */
    virtual FILE *fopen(const char *name) = 0;

    virtual void fprintf(FILE *id, const char *fmt, ...) = 0;

    virtual void fclose(FILE *id) = 0;

    /** Memory Handling */
    virtual void *virt_to_phy(void *address) = 0;

    virtual int getMinAllocSize() = 0;

    virtual int getCacheLineSize() = 0;

    /** Time Handling */
    virtual void rstTime(struct ClearTimeMsg *msg) = 0;

    virtual void rstTime() = 0;

    virtual Time getTime() = 0;

    virtual void rstJobIx() = 0;

    /** Platform getter/setter */
    static inline Platform *get();

    virtual LRT *getLrt() = 0;

    virtual int getLrtIx() = 0;

    virtual int getNLrt() = 0;

    virtual LrtCommunicator *getLrtCommunicator() = 0;

    virtual SpiderCommunicator *getSpiderCommunicator() = 0;

    virtual void setStack(SpiderStack id, Stack *stack) = 0;

    virtual Stack *getStack(SpiderStack id) = 0;

    virtual inline int getMaxActorAllocSize(int pe);

protected:
    Platform();

    virtual ~Platform();

    static Platform *platform_;
};

inline Platform *Platform::get() {
    if (platform_)
        return platform_;
    else
        throw std::runtime_error("Error undefined platform\n");
}

// If unimplemented in child
inline int Platform::getMaxActorAllocSize(int pe) {
    return 1024 * 1024 * 1024;
}

#endif/*PLATFORM_H*/
