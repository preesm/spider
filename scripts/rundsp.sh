#!/bin/bash

#Author : Louis-Paul CORDIER
#Date :   26/5/2015

source $(dirname $0)/../env.sh

function check_error {
  if [ $1 -ne 0 ]; then
    echo -e "\e[1m\e[31mFAILED\e[0m"
    echo -e "\e[32m$2"
    echo -e "\e[0m"
    echo 
    exit -1
  fi
}


#Extract filename from the path:
DSP_REMOTE_BIN=$(basename $CURRENT_DSP_BIN_PATH)
echo "remote path is $REMOTE_WORKING_PATH/$DSP_REMOTE_BIN"

#Reset, load and run DSP program.

for i in $(seq 0 $NB_DSP) ; do
	echo -ne "\nLoading on DSP$i... "
	OUTPUT=$(ssh $SSH_OPTIONS $SSH_REMOTE_USER@$SSH_REMOTE_HOST "mpmcl reset dsp$i")
	check_error $? "$OUTPUT"

	OUTPUT=$(ssh $SSH_OPTIONS $SSH_REMOTE_USER@$SSH_REMOTE_HOST "mpmcl load dsp$i $REMOTE_WORKING_PATH/$DSP_REMOTE_BIN")
        check_error $? "$OUTPUT"

	OUTPUT=$(ssh $SSH_OPTIONS $SSH_REMOTE_USER@$SSH_REMOTE_HOST "mpmcl run dsp$i")
        check_error $? "$OUTPUT"

    	echo -ne "\e[92mOK\e[0m"
done
echo 

