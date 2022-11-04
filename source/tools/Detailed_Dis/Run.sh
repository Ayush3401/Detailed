#!/bin/bash
#cd /home/amit/Desktop/Tool/pin-3.20/source/tools/Dis_Mem/
#make obj-intel64/I_M.so TARGET=intel64
#ake obj-intel64/I_M_Client.so TARGET=intel64

pkill -9 -f MulT
../../../pin -t obj-intel64/Detailed.so -- ./MulT

exit
