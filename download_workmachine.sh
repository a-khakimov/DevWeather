#!/bin/bash

make 
~/sources/stlink/stlink/build/Release/st-flash \
write \
~/sources/templcd/build/templcd.bin \
0x08000000

