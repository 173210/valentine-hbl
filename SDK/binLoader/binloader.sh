#!/usr/bin/sh

psp-as loader.s
psp-objcopy -O binary a.out a.bin
rm a.out
mv a.bin saveplain/code.bin
cd saveplain/
./inject.rb

