#!/bin/bash

#Author : Louis-Paul CORDIER
#Date :   26/5/2015

source ../env.sh

rm /tmp/$CURRENT_PGANTT_FILE
scp $SSH_REMOTE_USER@$SSH_REMOTE_HOST:$REMOTE_WORKING_PATH/$CURRENT_PGANTT_FILE /tmp/
java -jar $PATH_TO_GANTT_TOOL -f /tmp/$CURRENT_PGANTT_FILE
