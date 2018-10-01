#!/bin/bash -eu

DIR=$(cd `dirname $0` && echo `git rev-parse --show-toplevel`)

echo "BUILD AND TEST"

LINUX_BUILD_DIR="${DIR}"/master/build_linux/
WIN32_BUILD_DIR="${DIR}"/master/build_win32/
TMPDIR=$(mktemp -d)


## Copy pthreadVC2.lib to /lib_spider/lib/pthread-2.10.0/lib
#  Copy pthreadVC2.dll to /lib_spider/lib/pthread-2.10.0/dll
#  Copy _ptw32.h to /lib_spider/lib/pthread-2.10.0/include
#  Copy pthread.h to /lib_spider/lib/pthread-2.10.0/include
#  Copy sched.h to /lib_spider/lib/pthread-2.10.0/include
#  Copy semaphore.h to /lib_spider/lib/pthread-2.10.0/include


rm -rf "${DIR}"/master/lib/pthread-2.10
mkdir -p "${DIR}"/master/lib/pthread-2.10/lib
mkdir -p "${DIR}"/master/lib/pthread-2.10/dll
mkdir -p "${DIR}"/master/lib/pthread-2.10/include

wget -O "${TMPDIR}"/pthread-2.10.0.zip https://preesm.github.io/assets/downloads/pthread-2.10.0.zip
(cd "${TMPDIR}" && unzip pthread-2.10.0.zip)


cp "${TMPDIR}"/pthread-2.10.0/lib/pthreadVC2.lib "${DIR}"/master/lib/pthread-2.10/lib
cp "${TMPDIR}"/pthread-2.10.0/lib/*.a "${DIR}"/master/lib/pthread-2.10/lib

cp "${TMPDIR}"/pthread-2.10.0/lib/*.dll "${DIR}"/master/lib/pthread-2.10/dll
cp "${TMPDIR}"/pthread-2.10.0/lib/*.a "${DIR}"/master/lib/pthread-2.10/dll

cp "${TMPDIR}"/pthread-2.10.0/include/*.h "${DIR}"/master/lib/pthread-2.10/include



rm -rf "${TMPDIR}"


rm -rf "${WIN32_BUILD_DIR}"
mkdir -p "${WIN32_BUILD_DIR}"
exit 0
(cd "${WIN32_BUILD_DIR}" && cmake .. -DCROSS_COMPILE_MINGW=true && make)


rm -rf "${LINUX_BUILD_DIR}"
mkdir -p "${LINUX_BUILD_DIR}"
(cd "${LINUX_BUILD_DIR}" && cmake .. && make)
