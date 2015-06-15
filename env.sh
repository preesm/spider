#!/bin/bash

#Author : Louis-Paul CORDIER
#Date :   26/5/2015

CCS_PATH="/home/x0231061/ti/ccsv6/eclipse/ccstudio"

#__Set the compilation environment
export ROOTFS_PATH="/data/wisap_tools_link/ti/mcsdk/3_01_02_05/mcsdk_linux_3_01_02_05/linux-devkit.patched4/sysroots/cortexa15hf-vfp-neon-3.8-oe-linux-gnueabi"
export PDK_PATH="/data/wisap_tools_link/ti/mcsdk/3_01_02_05/pdk_keystone2_3_01_02_05.patched4"
export DSPLIB_PATH="/data/wisap_tools_link/ti/mcsdk/3_01_02_05/dsplib_c66x_3_4_0_0"
export GCC_LINARO="/data/wisap_tools_link/linaro/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin"
export SPIDER_PATH="/home/x0231061/git/compa_spider/master"
export SPIDER_DEVICE="DEVICE_K2L"
export PDK_DEVICE_SUBFOLDER="k2l"
export SPIDER_K2ARMLIB_OUTPATH="/home/x0231061/git/compa_spider/master/projects/Spider_k2_Arm"
export SPIDER_K2DSPLIB_OUTPATH="/home/x0231061/git/compa_spider/master/projects/Spider_k2_dsp_lrt"

#__Board configuration
export SSH_REMOTE_USER="root"
export SSH_REMOTE_HOST="lav1n2"
export SSH_OPTIONS=""
export SCP_OPTIONS=""
export PATH_TO_GANTT_TOOL="/home/x0231061/project/spider_integration/GanttDisplay.jar"
export REMOTE_WORKING_PATH="/home/root/spider"

#__DSP configuration
#Number of DSP cores MINUS 1
export NB_DSP="3"

#__Current project configuration
export CURRENT_ARMLIB_FILENAME="libSpider_k2_Arm_release.so"
export CURRENT_ARM_BIN_PATH="/home/x0231061/git/compa_spider/apps/radix2_fft_fx/projects/k2_arm/Release/radix2_fft_fx_k2_arm_release"
export CURRENT_DSP_BIN_PATH="/home/x0231061/git/compa_spider/apps/radix2_fft_fx/projects/k2_dsp/Release/radix2_fft_fx_k2_dsp_release.out"
export CURRENT_PGANTT_FILE="radixFFT_2_fx.pgantt"


