# Makefile to build OpenMP applications


default: all

include Makefile.config
include Makefile.libomp

PROG_NAME := omp_hclm

#
# Compiler option configuration
#
LNK_CMD   = $(CFGDIR)/linker.cmd
OPT_CMD   = $(CFGDIR)/compiler.opt
CL_OPTS   = -@ $(OPT_CMD) -mv6600 -DDEVICE_K2H --omp -I $(OMP_DIR)/ti/runtime/openmp -I $(DSP_DIR)
CLPP_OPTS = $(CL_OPTS) --exceptions 
LNK_OPTS  = -x -c --priority -w
CL        = $(XDCCGROOT)/bin/cl6x
RTS_LIB   = $(XDCCGROOT)/lib/libc.a

#
# Linux Compil
#
LINUX_CC = g++-4.9
LINUX_LD = g++-4.9
LINUX_C_FLAGS = -g -O0 -fopenmp
LINUX_LNK_FLAGS = -g -fopenmp

ifeq ($(BUILD_TYPE),debug)
    CL_OPTS += -g --optimize_with_debug=on
else
    CL_OPTS += -o3
endif


ifeq ($(OMP_TARGET),C6636)
CL_OPTS += -DTI_C6636 -DDEVICE_K2H -DDSP
else ifeq ($(OMP_TARGET),C6678)
CL_OPTS += -DTI_C6678 
else ifeq ($(OMP_TARGET),C6670)
CL_OPTS += -DTI_C6670 
else ifeq ($(OMP_TARGET),C6657)
CL_OPTS += -DTI_C6657 
else
$(call error,ERROR - INVALID OMP_TARGET - SET TO ONE OF C6657/C6670/C6678/C6636)
endif

COMMON_SRC_FILES = src/actors.cpp src/hclm_sched.cpp src/main.cpp src/ompMonitor.cpp src/platform.cpp
LINUX_SRC_FILES = src/platformLinux.cpp $(COMMON_SRC_FILES)
DSP_SRC_FILES = src/platformK2Dsp.cpp $(COMMON_SRC_FILES)
	

LINUX_OBJ_FILES := $(addprefix obj_linux/,$(notdir $(LINUX_SRC_FILES:.cpp=.o)))
DSP_OBJ_FILES := $(addprefix obj_dsp/,$(notdir $(DSP_SRC_FILES:.cpp=.obj)))  

.PHONY: linux dsp all clean clean_linux clean_dsp

all: linux dsp

linux: $(PROG_NAME)_linux
dsp: $(PROG_NAME)_dsp.out

# Clean Options
clean: libomp_clean clean_linux clean_dsp
	
clean_linux:
	@rm -rf $(PROG_NAME)_linux obj_linux

clean_dsp:
	@rm -rf $(PROG_NAME)_dsp.out obj_dsp *.mak
	
# Directory Creation
obj_linux:
	mkdir obj_linux
	
obj_dsp:
	mkdir obj_dsp
	
# Compiling Obj Files 
obj_linux/%.o: src/%.cpp obj_linux
	@echo compiling $<
	@${LINUX_CC} ${LINUX_C_FLAGS} -c $< -o $@ 
	
obj_dsp/%.obj: src/%.cpp libomp_config obj_dsp
	@echo compiling $<
	@$(CL) $(CLPP_OPTS) --output_file=$@  -c $<  -z  -m $*.map $(LNK_CMD) $(RTS_LIB) 
	
obj_dsp/omp_config_api.obj: omp_config_api.c obj_dsp
	@echo compiling $<
	@$(CL) $(CL_OPTS) --output_file=$@  -c $<  -z  -m $*.map $(LNK_CMD) $(RTS_LIB) 
	
# Linking Exe
$(PROG_NAME)_dsp.out: $(DSP_OBJ_FILES) obj_dsp/omp_config_api.obj
	@echo Linking $@
	@$(CL) $(CL_OPTS) -z $(LNK_OPTS) --output_file=$@ -c $^ omp_config/linker.cmd $(RTS_LIB)	-I "$(DSP_DIR)/ti/dsplib/lib" -ldsplib.ae66

$(PROG_NAME)_linux: $(LINUX_OBJ_FILES)
	@echo Linking $@
	@$(LINUX_LD) $(LINUX_LNK_FLAGS)  $^ -o $@
	