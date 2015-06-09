#!/bin/bash

#Author : Louis-Paul CORDIER
#Date :   26/5/2015

echo -ne "\e[35m_____Upload files on the board\e[0m\n"
$(dirname $0)/upload.sh
echo -ne "\e[35m_____Launch code on DSP\e[0m\n"
$(dirname $0)/rundsp.sh
echo -ne "\e[35m_____Launch code on ARM\e[0m\n"
$(dirname $0)/runarm.sh
echo -ne "\e[35m_____Display the Gantt\e[0m\n"
$(dirname $0)/displayGantt.sh
