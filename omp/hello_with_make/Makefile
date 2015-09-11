# Makefile to build OpenMP applications


default: all

include Makefile.config
include Makefile.libomp

#
# Compiler option configuration
#
LNK_CMD   = $(CFGDIR)/linker.cmd
OPT_CMD   = $(CFGDIR)/compiler.opt
CL_OPTS   = -@ $(OPT_CMD) -mv6600 --omp -I $(OMP_DIR)/ti/runtime/openmp
LNK_OPTS  = -x -c --priority -w
CL        = $(XDCCGROOT)/bin/cl6x
RTS_LIB   = $(XDCCGROOT)/lib/libc.a


ifeq ($(BUILD_TYPE),debug)
    CL_OPTS += -g --optimize_with_debug=on
else
    CL_OPTS += -o3
endif


ifeq ($(OMP_TARGET),C6636)
CL_OPTS += -DTI_C6636
else ifeq ($(OMP_TARGET),C6678)
CL_OPTS += -DTI_C6678 
else ifeq ($(OMP_TARGET),C6670)
CL_OPTS += -DTI_C6670 
else ifeq ($(OMP_TARGET),C6657)
CL_OPTS += -DTI_C6657 
else
$(call error,ERROR - INVALID OMP_TARGET - SET TO ONE OF C6657/C6670/C6678/C6636)
endif
 
testfiles = hello.c

outfiles = $(patsubst %.c,%.out,$(testfiles))

all: $(outfiles)


%.out: %.c  libomp_config
	echo compiling $<
	$(CL) $(CL_OPTS) $< omp_config_api.c -z $(LNK_OPTS) -o $@ -m $*.map $(LNK_CMD) $(RTS_LIB)

clean: libomp_clean
	@rm -rf *.map *.out *.obj *.mak 

#
# Cleans libomp artifacts
#
realclean: libomp_clean
	@rm -rf *.map *.out *.obj *.mak 
