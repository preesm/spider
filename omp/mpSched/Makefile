CGTOOLS = /opt/ti_v5_2/TI_CGT_C6000_7.4.0
XDC_INSTALL_DIR = /opt/ti_v5_2/xdctools_3_23_04_60
CC = $(CGTOOLS)/bin/cl6x
LNK = $(CGTOOLS)/bin/cl6x
RTS = $(CGTOOLS)/lib/rts6600_elf.lib
CONFIG = omp_config
XDCTARGET = ti.targets.elf.C66
XDCPLATFORM = ti.omp.examples.platforms.evm6678
XDCTOOLS_JAVA_HOME = /opt/ti_v5_2/ccsv5/eclipse/jre

CFLAGS =-mv6600 --abi=eabi -g -O3 --include_path="$(CGTOOLS)/include" \
		--display_error_number --diag_warning=225 --openmp --obj_directory=./obj \
		-i"/opt/ti_v5_2/dsplib_c66x_3_1_0_0/packages"
LDFLAGS=-mv6600 --abi=eabi -g --display_error_number --diag_warning=225 --openmp -z \
		-m"omp_mpSched.map" -i"$(CGTOOLS)/lib" \
		-i"$(CGTOOLS)/include" --reread_libs --warn_sections \
		--display_error_number --xml_link_info="omp_mpSched_linkInfo.xml" --rom_model \
		-i"/opt/ti_v5_2/dsplib_c66x_3_1_0_0/lib" -ldsplib.ae66

XDC_PATH:=/opt/ti_v5_2/bios_6_33_05_46/packages;/opt/ti_v5_2/ipc_1_24_03_32/packages;/opt/ti_v5_2/pdk_C6678_1_1_2_6/packages;/opt/ti_v5_2/omp_1_01_03_02/packages;/opt/ti_v5_2/ccsv5/ccs_base;

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.obj)))

CONFIGURO = XDCTOOLS_JAVA_HOME="$(XDCTOOLS_JAVA_HOME)" \
        $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDC_PATH)" xdc.tools.configuro
        
.PRECIOUS: %/compiler.opt %/linker.cmd
 
all: omp_mpSched.out

configPkg/compiler.opt: configPkg/linker.cmd ;
configPkg/linker.cmd : $(CONFIG).cfg
	$(CONFIGURO) -o configPkg -c $(CGTOOLS) -t $(XDCTARGET) -p $(XDCPLATFORM) $<
	
obj:
	mkdir obj
	
obj/%.obj: src/%.c configPkg/compiler.opt obj
	$(CC) $(CFLAGS) -@configPkg/compiler.opt -c $<
 
omp_mpSched.out : $(OBJ_FILES) configPkg/linker.cmd
	$(LNK) $(LDFLAGS) -o $@ -c $^ $(RTS)
	
clean: 
	rm -r configPkg obj omp_mpSched.map omp_mpSched_linkInfo.xml omp_mpSched.out
 