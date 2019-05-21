/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
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
#include <Logger.h>
#include <cstdarg>
#if defined _WIN32 && !defined _MSC_VER
#include <mingw-std-threads/include/mingw.mutex.h>
#else
#include <mutex>
#endif

static bool loggersValue[kNLogger];

static const char *loggersLitteral[kNLogger] = {
        "JOB",
        "TIME",
        "GENERAL",
};

static std::mutex lock;

void Logger::initializeLogger() {
    loggersValue[LOG_JOB] = false;
    loggersValue[LOG_TIME] = false;
    loggersValue[LOG_GENERAL] = true;
}

void Logger::enable(LoggerType type) {
    loggersValue[type] = true;
}

void Logger::disable(LoggerType type) {
    loggersValue[type] = false;
}

inline bool Logger::isLoggerEnabled(LoggerType type) {
    return loggersValue[type];
}


void Logger::print(LoggerType type, LoggerLevel level, const char *fmt, ...) {
    if (Logger::isLoggerEnabled(type)) {
        std::lock_guard<std::mutex> locker(lock);
        va_list l;
        va_start(l, fmt);
        fprintf(stderr, "%s-", loggersLitteral[type]);
        switch (level) {
            case LOG_INFO:
                fprintf(stderr, "INFO: ");
                break;
            case LOG_WARNING:
                fprintf(stderr, "WARNING: ");
                break;
            case LOG_ERROR:
                fprintf(stderr, "ERROR: ");
                break;
            default:
                fprintf(stderr, "UNDEFINED: ");
                break;
        }
        vfprintf(stderr, fmt, l);
        va_end(l);
    }
}
