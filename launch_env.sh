#!/bin/bash
source $(dirname $0)/env.sh
export PATH=$PATH:$GCC_LINARO

#Execute CCS
$CCS_PATH

