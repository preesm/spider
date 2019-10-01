#K1 Makefile for build SPIDER

COMPILE_OPTI := -Os
EXTRA_FLAGS := -DMPPA_TRACE_ENABLE  -ffunction-sections -fdata-sections -fno-common -fno-PIC -Wall

release ?= 0

verbose ?= 0
verbose_stack ?= 0
verbose_time ?= 0
verbose_segment_id ?= 0

# target Spider IO
#io-system := bare
system-name := bare
io-lib := kspiderio
kspiderio-name := kspiderio

# target Spider CC
cluster-system := bare
cluster-lib := kspidercc
kspidercc-name := kspidercc

LIB_DIR := libspider

SPIDER_SRCS := $(wildcard $(LIB_DIR)/common/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/common/tools/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/lrt/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/lrt/specialActors/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/platforms/platform_MPPA/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/graphs/Archi/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/graphs/Bipartite/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/graphs/PiSDF/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/graphs/SRDAG/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/graphTransfo/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/launcher/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/monitor/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/parser/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/scheduling/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/scheduling/MemAlloc/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider/scheduling/Scheduler/*[.cpp])
SPIDER_SRCS += $(wildcard $(LIB_DIR)/spider-api/spider/*[.cpp])


ifeq ($(release), 1)
COMPILE_OPTI += -O3 -g -std=c++11
else
COMPILE_OPTI += -g -std=c++11

kspiderio-cppflags += -DDEBUG
kspidercc-cppflags += -DDEBUG
endif


kspiderio-srcs := $(SPIDER_SRCS)
kspiderio-cppflags := -mhypervisor $(COMPILE_OPTI) -I $(LIB_DIR) -I $(LIB_DIR)/spider -I $(LIB_DIR)/platforms/platform_MPPA -I $(LIB_DIR)/common -I $(LIB_DIR)/lrt -fpermissive $(EXTRA_FLAGS)

kspiderio-dep :=  -pthread -mhypervisor -lmppa_request_engine -lmppa_remote -lmppa_async -lmppapower -lmppanoc -lmpparouting -lpcie_queue 


kspidercc-srcs := $(SPIDER_SRCS)
kspidercc-cppflags := -mhypervisor $(COMPILE_OPTI) -I $(LIB_DIR) -I $(LIB_DIR)/spider -I $(LIB_DIR)/platforms/platform_MPPA -I $(LIB_DIR)/common -I $(LIB_DIR)/lrt -fpermissive $(EXTRA_FLAGS)

kspidercc-dep :=  -pthread -mhypervisor -lmppa_remote -lmppa_async -lmppa_request_engine -lmppapower -lmppanoc -lmpparouting


ifeq ($(verbose), 1)
kspiderio-cppflags += -DVERBOSE
kspidercc-cppflags += -DVERBOSE
endif

ifeq ($(verbose_stack), 1)
kspiderio-cppflags += -DVERBOSE_STACK
kspidercc-cppflags += -DVERBOSE_STACK
endif

ifeq ($(verbose_time), 1)
kspiderio-cppflags += -DVERBOSE_TIME
kspidercc-cppflags += -DVERBOSE_TIME
endif

ifeq ($(verbose_segment_id), 1)
kspiderio-cppflags += -DVERBOSE_SEGMENT_ID
kspidercc-cppflags += -DVERBOSE_SEGMENT_ID
endif


install: all
	cp ./output/lib/io/libkspiderio.a ./
	cp ./output/lib/cluster/libkspidercc.a ./

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray
