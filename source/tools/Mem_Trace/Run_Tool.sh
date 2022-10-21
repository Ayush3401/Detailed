#!/bin/bash
#cd /home/amit/Desktop/Tool/pin-3.20/source/tools/Dis_Mem/
#make obj-intel64/I_M.so TARGET=intel64
#ake obj-intel64/I_M_Client.so TARGET=intel64

../../../pin -t obj-intel64/I_M.so -N 1 -- /home/amit/Desktop/XSBench -s large -t 16 &
../../../pin -t obj-intel64/I_M.so -N 2 -- /home/amit/Desktop/XSBench -s large -t 16 &
exit
