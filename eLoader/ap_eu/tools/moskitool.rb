#!/usr/bin/ruby

# crappy ruby code by wololo
# given a list of librarys + memdump.bin file, outputs nids + their position

dump_file = File.new("memdump.bin", "r");
dump_file.binmode;
dump = dump_file.read();


libraries = [
"InterruptManager",
 "ModuleMgrForUser",
 "UtilsForUser",
"ThreadManForUser",
"LoadExecForUser",
"SysMemUserForUser",
"sceDisplay",
"sceCtrl",
"sceUmdUser",
"sceAudio",
"sceRtc",
"scePower",
"sceUtility",
"sceGe_user",
"IoFileMgrForUser",
 ];


class Integer
    def to_hex(bytes)
        return to_s(16).rjust(bytes, "0").upcase
    end
end   

# N -> V ?
#n -> v ?
class StubEntry

  def initialize(dump, position)
	@libraryName = dump[position..position+3].unpack("V")[0]
	@importFlags = dump[position+4..position+5].unpack("v")[0]
	@libraryVersion = dump[position+6..position+7].unpack("v")[0]
	@importStubs = dump[position+8..position+9].unpack("v")[0] 
	@stubSize = dump[position+10..position+11] .unpack("v")[0]
	@nidPointer = dump[position+12..position+15].unpack("V")[0]
	@jumpPointer = dump[position+16..position+20].unpack("V")[0]
  end
  
  def to_s
    "Stub Entry:" +
	"\n\tLibrary name pointer:" + @libraryName.to_hex(8) +
	"\n\tImport flags:" + @importFlags.to_hex(4) +
	"\n\tLibrary version: " + @libraryVersion.to_hex(4) + 
	"\n\tImport stubs: " + @importStubs.to_hex(4) +
	"\n\tNumber of imports: " + @stubSize.to_hex(4) +
	"\n\tPointer to NIDs:" + @nidPointer.to_hex(8) +
	"\n\tPointer to stubs:" + @jumpPointer.to_hex(8) + 
    "\n"
  end
  attr_accessor :libraryName, :importFlags, :libraryVersion, :importStubs, :stubSize, :nidPointer, :jumpPointer
    
end


start_address = 0x08800000;

libraries.each { |library|
    pos = dump.index(library);
    if pos then
        word = [start_address + pos].pack("V");
        puts library;
        posword = dump.index(word);
        stubEntry = StubEntry.new(dump, posword);
        puts (stubEntry);
        offset = stubEntry.nidPointer - start_address;
        for i in 0..stubEntry.stubSize - 1 do
            pos2 = offset + (i * 4)
            pos3 = stubEntry.jumpPointer + (i * 8)
            a = "0x" + dump[pos2+3].to_hex(2) + dump[pos2+2].to_hex(2) + dump[pos2+1].to_hex(2) + dump[pos2].to_hex(2) + " - " + pos3.to_hex(8)
            puts a
        end
    else
        puts library + " not found"
    end

}
