#!/bin/bash

#Author : Louis-Paul CORDIER
#Date :   26/5/2015

source ../env.sh

#Extract arm binary
REMOTE_ARM_BIN=$(basename $CURRENT_ARM_BIN_PATH)
echo "Remote path is $REMOTE_WORKING_PATH/$REMOTE_ARM_BIN"

echo -ne "Kill program on ARM if running...\n"
ssh $SSH_REMOTE_USER@$SSH_REMOTE_HOST "cd $REMOTE_WORKING_PATH; killall $REMOTE_ARM_BIN"

echo -ne "\nLaunch program on ARM... \e[92m \n"
ssh $SSH_REMOTE_USER@$SSH_REMOTE_HOST "cd $REMOTE_WORKING_PATH; ./$REMOTE_ARM_BIN"

echo -ne "\e[0m"
echo "Done!" 

