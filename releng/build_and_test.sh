#!/bin/bash -eu

DIR=$(cd `dirname $0` && echo `git rev-parse --show-toplevel`)

echo "BUILD AND TEST"

LINUX64_BUILD_DIR="${DIR}"/master/build_linux64/
LINUX32_BUILD_DIR="${DIR}"/master/build_linux32/
WIN32_BUILD_DIR="${DIR}"/master/build_win32/
TMPDIR=$(mktemp -d)



rm -rf "${LINUX64_BUILD_DIR}"
mkdir -p "${LINUX64_BUILD_DIR}"
(cd "${LINUX64_BUILD_DIR}" && cmake .. -D64BITS=true -DSKIP_PAPI=true -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON && make)

if  [ $# == 1 ] && [ "$1" == "--cross" ]; then
  rm -rf "${LINUX32_BUILD_DIR}"
  mkdir -p "${LINUX32_BUILD_DIR}"
  (cd "${LINUX32_BUILD_DIR}" && cmake .. -D32BITS=true -DSKIP_PAPI=true -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON && make)


## Copy pthreadVC2.lib to /lib_spider/lib/pthread-2.10.0/lib
#  Copy pthreadVC2.dll to /lib_spider/lib/pthread-2.10.0/dll
#  Copy _ptw32.h to /lib_spider/lib/pthread-2.10.0/include
#  Copy pthread.h to /lib_spider/lib/pthread-2.10.0/include
#  Copy sched.h to /lib_spider/lib/pthread-2.10.0/include
#  Copy semaphore.h to /lib_spider/lib/pthread-2.10.0/include

# PThread for Windows

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

# STD::Thread for Windows

rm -rf "${DIR}"/master/lib/mingw-std-threads
mkdir -p "${DIR}"/master/lib/mingw-std-threads/include
wget -O "${TMPDIR}"/mingw-std-threads.zip https://github.com/meganz/mingw-std-threads/archive/master.zip
(cd "${TMPDIR}" && unzip mingw-std-threads.zip)
cp "${TMPDIR}"/mingw-std-threads-master/*.h "${DIR}"/master/lib/mingw-std-threads/include

rm -rf "${TMPDIR}"


  rm -rf "${WIN32_BUILD_DIR}"
  mkdir -p "${WIN32_BUILD_DIR}"
  (cd "${WIN32_BUILD_DIR}" && cmake .. -DCROSS_COMPILE_MINGW=true -DSKIP_PAPI=true -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON && make)

  RLSDIR=$(mktemp -d)
  VERSION=$(cat "${DIR}"/VERSION)
  SPIDERDIR="${RLSDIR}"/spider-${VERSION}

  mkdir -p "${SPIDERDIR}"/linux32
  mkdir -p "${SPIDERDIR}"/linux64
  mkdir -p "${SPIDERDIR}"/win32
  mkdir -p "${SPIDERDIR}"/include/spider-api/user/

  cp "${LINUX32_BUILD_DIR}"/libSpider.so "${SPIDERDIR}"/linux32/
  cp "${LINUX64_BUILD_DIR}"/libSpider.so "${SPIDERDIR}"/linux64/
  cp "${WIN32_BUILD_DIR}"/libSpider.dll.a "${SPIDERDIR}"/win32/
  cp "${WIN32_BUILD_DIR}"/Release/libSpider.dll "${SPIDERDIR}"/win32/
  cp "${DIR}"/master/libspider/spider/spider.h "${SPIDERDIR}"/include/
  cp "${DIR}"/master/libspider/spider-api/user/*.h "${SPIDERDIR}"/include/spider-api/user/

  (cd "${RLSDIR}" && zip -r spider-${VERSION}.zip spider-${VERSION}/)
  mv "${RLSDIR}"/spider-${VERSION}.zip "${DIR}"/
  rm -rf "${RLSDIR}"

fi
