TI_FOLDER = /opt/ti
XDCTOOLS_JAVA_HOME = $(TI_FOLDER)/ccsv5/eclipse/jre
BIOS = $(TI_FOLDER)/bios_6_33_06_50
CGT = $(TI_FOLDER)/TI_CGT_C6000_7.4.0
DSP = $(TI_FOLDER)/dsplib_c66x_3_1_0_0
IPC = $(TI_FOLDER)/ipc_1_24_03_32
PDK = $(TI_FOLDER)/pdk_C6678_1_1_2_6
OMP = $(TI_FOLDER)/omp_1_01_03_02
XDC = $(TI_FOLDER)/xdctools_3_23_04_60

CC = $(CGTOOLS)/bin/cl6x
LNK = $(CGTOOLS)/bin/cl6x
RTS = $(CGTOOLS)/lib/rts6600_elf.lib
CONFIG = omp_config
XDCTARGET = ti.targets.elf.C66
XDCPLATFORM = ti.omp.examples.platforms.evm6678

CFLAGS =-mv6600 --abi=eabi -g -O3 --include_path="$(CGTOOLS)/include" \
		--display_error_number --diag_warning=225 --openmp --obj_directory=./obj \
		-i"$(DSP)/packages"
LDFLAGS=-mv6600 --abi=eabi -g --display_error_number --diag_warning=225 --openmp -z \
		-m"omp_mpSched.map" -i"$(CGT)/lib" \
		-i"$(CGT)/include" --reread_libs --warn_sections \
		--display_error_number --xml_link_info="omp_mpSched_linkInfo.xml" --rom_model \
		-i"$(DSP)/lib" -ldsplib.ae66

XDC_PATH:=$(BIOS)/packages;$(IPC)/packages;$(PDK)/packages;$(OMP)/packages;/opt/ti/ccsv5/ccs_base;

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.obj)))

CONFIGURO = XDCTOOLS_JAVA_HOME="$(XDCTOOLS_JAVA_HOME)" \
        $(XDC)/xs --xdcpath="$(XDC_PATH)" xdc.tools.configuro
        
.PRECIOUS: %/compiler.opt %/linker.cmd
 
all: omp_mpSched.out

configPkg/compiler.opt: configPkg/linker.cmd ;
configPkg/linker.cmd : $(CONFIG).cfg
	$(CONFIGURO) -o configPkg -c $(CGT) -t $(XDCTARGET) -p $(XDCPLATFORM) $<
	
obj:
	mkdir obj
	
obj/%.obj: src/%.c configPkg/compiler.opt obj
	$(CC) $(CFLAGS) -@configPkg/compiler.opt -c $<
 
omp_mpSched.out : $(OBJ_FILES) configPkg/linker.cmd
	$(LNK) $(LDFLAGS) -o $@ -c $^ $(RTS)
	
clean: 
	rm -r configPkg obj omp_mpSched.map omp_mpSched_linkInfo.xml omp_mpSched.out
 