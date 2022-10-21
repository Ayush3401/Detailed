#!/bin/bash
#cd /home/amit/Desktop/Tool/pin-3.20/source/tools/Dis_Mem/
#make obj-intel64/I_M.so TARGET=intel64
#ake obj-intel64/I_M_Client.so TARGET=intel64

#../../../pin -t obj-intel64/I_M.so -N 1 -- /home/amit/Desktop/Splash-3/codes/apps/fmm/FMM < inputs/input.template
#../../../pin -t obj-intel64/I_M.so -N 3 -- /home/amit/Desktop/Splash-3/codes/apps/fmm/FMM < inputs/input.template &
../../../pin -t obj-intel64/I_M.so -N 4 -- /home/amit/Desktop/Splash-3/codes/apps/fmm/FMM < inputs/input.template &
../../../pin -t obj-intel64/I_M.so -N 5 -- /home/amit/Desktop/Splash-3/codes/apps/fmm/FMM < inputs/input.template &
#../../../pin -t obj-intel64/I_M.so -N 6 -- /home/amit/Desktop/Splash-3/codes/apps/fmm/FMM < inputs/input.template 
#../../../pin -t obj-intel64/I_M.so -N 7 -- /home/amit/Desktop/Splash-3/codes/apps/fmm/FMM < inputs/input.template
#../../../pin -t obj-intel64/I_M.so -N 5 -- /home/amit/Desktop/Splash-3/codes/apps/ocean/non_contiguous_partitions/OCEAN -n4098 -p16 -e1e-07 -r10000 -t14400 &
#../../../pin -t obj-intel64/I_M.so -N 3 -- /home/amit/Desktop/Splash-3/codes/apps/ocean/non_contiguous_partitions/OCEAN -n4098 -p16 -e1e-07 -r10000 -t14400 &
#../../../pin -t obj-intel64/I_M.so -N 4 -- /home/amit/Desktop/Splash-3/codes/apps/ocean/non_contiguous_partitions/OCEAN -n4098 -p16 -e1e-07 -r10000 -t14400 &
exit
