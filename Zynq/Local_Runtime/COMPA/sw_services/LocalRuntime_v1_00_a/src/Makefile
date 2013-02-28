
COMPILER=
ARCHIVER=
CP=cp
COMPILER_FLAGS=-O2 -c -g
EXTRA_COMPILER_FLAGS=-O0
LIB=libxil.a

RELEASEDIR=../../../lib
INCLUDEDIR=../../../include
INCLUDES=-I./. -I${INCLUDEDIR}
#INCLUDES=-I${INCLUDEDIR}

LIBSOURCES=lrt_core.c lrt_debug.c lrt_ext_msg_mngr.c lrt_task_mngr.c
LIBOBJS=$(LIBSOURCES:%.c=%.o)
INCLUDEFILES= lrt_definitions.h lrt_prototypes.h lrt.h lrt_cfg.h
#INCLUDEFILES=lrt.h

libs:
	echo "Compiling Local Runtime"
	$(COMPILER) $(COMPILER_FLAGS) $(EXTRA_COMPILER_FLAGS) $(INCLUDES) $(LIBSOURCES)
	$(ARCHIVER) -r ${RELEASEDIR}/${LIB} $(LIBOBJS) 
	make clean
	
#libs:
#	@echo "Compiling" 
#	${RELEASEDIR}/${LIB}(${LIBOBJS})

(%.o):%.c
	$(COMPILER) $(COMPILER_FLAGS) $(EXTRA_COMPILER_FLAGS) $(INCLUDES) $< -o $*.o
	$(ARCHIVER) -r ${RELEASEDIR}/${LIB} $*.o
	$(RM) $*.o

include:
	${CP} $(INCLUDEFILES) $(INCLUDEDIR)

clean:
	rm -rf *.o
