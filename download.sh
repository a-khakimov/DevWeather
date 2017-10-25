#!/bin/bash

make 
~/stlink/stlink/build/Release/st-flash \
write \
~/sourcefiles/Microcontrollers/templcd/build/templcd.bin \
0x08000000

