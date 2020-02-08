#!/bin/bash

make && st-flash write build/templcd.bin 0x08000000

