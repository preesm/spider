#!/bin/bash

#Author : Louis-Paul CORDIER
#Date :   26/5/2015

source ../env.sh

function checkmd5 {
  LOCALFILE=$1
  REMOTEFILE=$2
  
  LOCALMD5=$(md5sum $LOCALFILE | cut -d' ' -f1)
  REMOTEMD5=$(ssh $SSH_REMOTE_USER@$SSH_REMOTE_HOST -- md5sum $REMOTEFILE | cut -d' ' -f1)

  if [ "$LOCALMD5" == "$REMOTEMD5" ]; then
	echo -e "\e[33m Warning: \e[0m MD5SUM of $(basename $LOCALFILE) are both identical on local and remote target. Skip downloading."
	return 1
  fi

  return 0
}

#Create remote dir if doesnt exist
ssh $SSH_REMOTE_USER@$SSH_REMOTE_HOST "mkdir -p $REMOTE_WORKING_PATH"

#Check if files have changed. Warn the user if not (Could be a compilation error)

#Uploading the ARM library
checkmd5 "$SPIDER_K2ARMLIB_OUTPATH/$CURRENT_ARMLIB_FILENAME" "/usr/lib/$CURRENT_ARMLIB_FILENAME"
if [ $? -eq 0 ]; then
  scp $SPIDER_K2ARMLIB_OUTPATH/$CURRENT_ARMLIB_FILENAME $SSH_REMOTE_USER@$SSH_REMOTE_HOST:/usr/lib/$CURRENT_ARMLIB_FILENAME
fi

#Uploading the ARM binary
checkmd5 "$CURRENT_ARM_BIN_PATH" "$REMOTE_WORKING_PATH/$(basename $CURRENT_ARM_BIN_PATH)"
if [ $? -eq 0 ]; then
  scp $CURRENT_ARM_BIN_PATH $SSH_REMOTE_USER@$SSH_REMOTE_HOST:$REMOTE_WORKING_PATH
fi

#Uploading the DSP binary
checkmd5 "$CURRENT_DSP_BIN_PATH" "$REMOTE_WORKING_PATH/$(basename $CURRENT_DSP_BIN_PATH)"
if [ $? -eq 0 ]; then
  scp $CURRENT_DSP_BIN_PATH $SSH_REMOTE_USER@$SSH_REMOTE_HOST:$REMOTE_WORKING_PATH
fi

