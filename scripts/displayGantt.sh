#!/bin/bash

#Author : Louis-Paul CORDIER
#Date :   26/5/2015

source $(dirname $0)/../env.sh

rm /tmp/$CURRENT_PGANTT_FILE
scp $SCP_OPTIONS $SSH_REMOTE_USER@$SSH_REMOTE_HOST:$REMOTE_WORKING_PATH/$CURRENT_PGANTT_FILE /tmp/
java -jar $PATH_TO_GANTT_TOOL -f /tmp/$CURRENT_PGANTT_FILE
