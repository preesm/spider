/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013)
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
#include "specialActors.h"

void saFork(void *inputFIFOs[], void *outputFIFOs[], Param inParams[], Param /*outParams*/[]) {
#if VERBOSE
    fprintf(stderr, "INFO: Entering Fork...\n");
#endif

    auto nbFifoIn = (int) inParams[0];
    auto nbFifoOut = (int) inParams[1];
    auto nbTknIn = (int) inParams[2];
    int index = 0;

    // 0. Check the number of input FIFOs
    if (nbFifoIn == 1) {
        /* Fork */
        for (int i = 0; i < nbFifoOut; i++) {
            auto nbTknOut = (int) inParams[i + 3];
            if (nbTknOut && outputFIFOs[i] != ((char *) inputFIFOs[0]) + index) {
                memcpy(outputFIFOs[i], ((char *) inputFIFOs[0]) + index, (size_t) nbTknOut);
            }
            index += nbTknOut;
        }
    } else {
        throwSpiderException("Fork should have exactly one input FIFO --> nInputFIFOs: %d.", nbFifoIn);
    }

    // 1. Check that nbTknIn == Sum(nbTknOut)
    if (index != nbTknIn) {
        throwSpiderException("Fork has remaining tokens --> nTokensIN: %d | nTokensOUT: %d.", nbTknIn, index);
    }

#if VERBOSE
    fprintf(stderr, "INFO: Exiting Fork...\n");
#endif
}

