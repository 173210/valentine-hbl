#!/usr/bin/ruby

# crappy ruby code by wololo
# given a list of librarys + memdump.bin file, outputs stubs

dump_file = File.new("memdump.bin", "r");
dump_file.binmode;
dump = dump_file.read();


libraries = [
"main",
"sceNet_Library",
"sceAudiocodec_Driver",
"sceATRAC3plus_Library",
"sceSAScore",
"sceMemab",
"sceNetAdhocAuth_Service",
"sceKernelLibrary",
"sceNetAdhoc_Library",
"sceNetAdhocctl_Library",
"sceNetAdhocDownload_Library",
"sceNetAdhocMatching_Library",
"sceVideocodec_Driver",
"sceMpegbase_Driver",
"sceMpeg_library", ];


class Integer
    def to_hex
        return to_s(16).rjust(2, "0").upcase
    end
end   



result = [];

libraries.each { |library|
    pos = dump.index(library);
    if pos then
        pos = pos + 40; 
        a = "0x" + dump[pos+3].to_hex + dump[pos+2].to_hex + dump[pos+1].to_hex + dump[pos].to_hex + ", #" + library
        result << a
        puts  a 
    else
        puts library + " not found"
    end

}
puts "===="

result.each { |res|
puts res
}