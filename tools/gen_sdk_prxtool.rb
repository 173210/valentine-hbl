#!/usr/bin/ruby

memory_base = 0x08804000;

f = File.new("functions.txt", "r")
h = {}
#0x267A6DD2 [0x08A888E8] - __sceSasRevParam
regexp = /^(0x.+) \[(0x.+)\] - (.+)$/
while (line = f.gets)
	if (line.match(regexp)) 
	  line.scan(regexp) {|nid,address,name| 
		h[name] = "0x" + (address.hex + memory_base).to_s(16).rjust(8, "0");
	  }
	end
end


out = File.new("sdk.S", "w")

out.puts %Q{.macro AddNID funcname, nid

	.globl  \\funcname
	.ent    \\funcname
\\funcname = \\nid
	.end    \\funcname

.endm

	.file	1 "sdk.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.text
	.align	2
	
	
}

h.each { |function,address|
	out.puts "\tAddNID #{function}, #{address}"
}

out.puts %Q{


	.ident	"VAL-SDK"
}
out.close
