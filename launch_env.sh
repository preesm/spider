#!/bin/bash

CCS_PATH="/home/x0231061/ti/ccsv6/eclipse/ccstudio"

#Set the compilation environment
export ROOTFS_PATH="/data/wisap_tools_link/ti/mcsdk/3_01_02_05/mcsdk_linux_3_01_02_05/linux-devkit.patched4/sysroots/cortexa15hf-vfp-neon-3.8-oe-linux-gnueabi"
export PDK_PATH="/data/wisap_tools_link/ti/mcsdk/3_01_02_05/pdk_keystone2_3_01_02_05.patched4"
export DSPLIB_PATH="/data/wisap_tools_link/ti/mcsdk/3_01_02_05/dsplib_c66x_3_4_0_0"
export GCC_LINARO="/data/wisap_tools_link/linaro/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin"
export SPIDER_PATH="/home/x0231061/project/clean_spider_git/master"
export SPIDER_DEVICE="DEVICE_K2L"
export PDK_DEVICE_SUBFOLDER="k2l"
export SPIDER_K2ARMLIB_OUTPATH="/home/x0231061/project/clean_spider_git/master/projects/Spider_k2_Arm"
export SPIDER_K2DSPLIB_OUTPATH="/home/x0231061/project/clean_spider_git/master/projects/Spider_k2_dsp_lrt"

export PATH=$PATH:$GCC_LINARO

#Execute CCS
$CCS_PATH

