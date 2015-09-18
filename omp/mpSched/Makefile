# Makefile to build OpenMP applications


default: all

include Makefile.config
include Makefile.libomp

#
# Compiler option configuration
#
LNK_CMD   = $(CFGDIR)/linker.cmd
OPT_CMD   = $(CFGDIR)/compiler.opt
CL_OPTS   = -@ $(OPT_CMD) -mv6600 --omp -I $(OMP_DIR)/ti/runtime/openmp -I $(DSP_DIR)
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
 
C_FILES := $(wildcard src/*.c) omp_config_api.c
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.obj)))

all: omp_mpSched.out

obj:
	@mkdir obj

obj/omp_config_api.obj: omp_config_api.c
	@echo compiling $<
	@$(CL) $(CL_OPTS) --output_file=$@  -c $<  -z  -m $*.map $(LNK_CMD) $(RTS_LIB) 

obj/%.obj: src/%.c libomp_config obj
	@echo compiling $<
	@$(CL) $(CL_OPTS) --output_file=$@  -c $<  -z  -m $*.map $(LNK_CMD) $(RTS_LIB) 
 
omp_mpSched.out: $(OBJ_FILES) 
	@$(CL) $(CL_OPTS) -z $(LNK_OPTS) --output_file=$@ -c $^ omp_config/linker.cmd $(RTS_LIB)	-I "$(DSP_DIR)/ti/dsplib/lib" -ldsplib.ae66

clean: libomp_clean 
	@rm -rf obj *.out *.obj *.mak 