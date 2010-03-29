buildsdk.rb creates sdk.S
functions.txt : list of imports retrieved by prxtool, necessary for buildsdk.rb

main.c+makefile => sample h.bin (memdumper)

################
binLoader
################
script binLoader.sh should take care of compiling the loader + injecting it in a saveplain file.
A file "SOURCE_SDDATA.bin"is necessary for it to inject into. That file is a standard valentine save obtained from sgdeemer
output file is SDDATA.bin, unencrypted savefile with binary loader
h.bin needs to be put on the root of the memory stick for the time being