#!/bin/bash
../../../pin -t obj-intel64/I_M.so -N 1 -- /home/amit/Desktop/MulT &
../../../pin -t obj-intel64/I_M_Client.so -P 2 -N 1 -- /home/amit/Desktop/MulT &
../../../pin -t obj-intel64/I_M_Client.so -P 3 -N 1 -- /home/amit/Desktop/MulT &
../../../pin -t obj-intel64/I_M_Client.so -P 4 -N 1 -- /home/amit/Desktop/MulT &

../../../pin -t obj-intel64/I_M.so -N 2 -- /home/amit/Desktop/MulT &
../../../pin -t obj-intel64/I_M_Client.so -P 2 -N 2 -- /home/amit/Desktop/MulT &
../../../pin -t obj-intel64/I_M_Client.so -P 3 -N 2 -- /home/amit/Desktop/MulT &
../../../pin -t obj-intel64/I_M_Client.so -P 4 -N 2 -- /home/amit/Desktop/MulT &
