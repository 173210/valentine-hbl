#!/usr/bin/ruby
require 'rexml/document'

doc = REXML::Document.new(open("libdoc.xml"))
funcs = []

doc.elements.each("PSPLIBDOC/PRXFILES/PRXFILE/LIBRARIES/LIBRARY/FUNCTIONS/FUNCTION") { |func|
	funcs << [ func.elements["NID"].text.hex, func.elements["NAME"].text]
}

modimp = File.new("modimp.txt", "r")

out = File.new("sdk.S", "w")
out.binmode()

out.puts %Q[.macro AddNID funcname, offset

	.globl	\\funcname
	.ent	\\funcname
	.type	\\funcname, @function
\\funcname = \\offset
	.end	\\funcname
	.size	\\funcname, 8

.endm

	.text
	.align	2

]

regexplib = /^Import Library (.+), attr 0x....$/
regexpfunc = /^Entry .+ : UID (.+), Function (.+)$/

while (line = modimp.gets)
	line.chomp!
	if (line.match(regexplib))
		lib = line.scan(regexplib)[0][0]
	elsif (line.match(regexpfunc))
		line.scan(regexpfunc) { |nid, addr|
			func = funcs.assoc(nid.hex)
			out.puts("\tAddNID " + (func ? func[1] : lib + "_" + nid[2..-1]) + ", " + addr)
		}
	end
end

out.puts %Q[

	.ident	"VAL-SDK"

]
