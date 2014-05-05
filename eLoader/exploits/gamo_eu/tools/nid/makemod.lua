#!/usr/bin/env lua

if not arg[1] then
	print('Error:\nBase address expected.')
	os.exit()
end

if not arg[2] then
	print('Error:\nModule name expected.')
	os.exit()
end
modname = arg[2]
while #modname < 0x20 do
	modname = modname .. string.char(0)
end

mod = {}
base = arg[1]
cur = 1

function padding(str)
	padstr = ''
	for i = 1, 8 - (string.len(str) % 4) do
		padstr = padstr .. string.char(0)
	end
	return padstr
end

function int2bin(int, two)
	hex = string.format('%08x', int)
	bin = ''
	bin = bin .. string.char('0x' .. string.sub(hex, -2, -1))
	bin = bin .. string.char('0x' .. string.sub(hex, -4, -3))
	if not two then
		bin = bin .. string.char('0x' .. string.sub(hex, -6, -5))
		bin = bin .. string.char('0x' .. string.sub(hex, -8, -7))
	end
	return bin
end

for line in io.lines('nidlist.txt') do
	if string.sub(line, 1, 1) == '\t' then
		table.insert(mod[#mod].func, string.sub(line, 2, -13))
		table.insert(mod[#mod].nid, string.sub(line, -10))
		print('\t' .. mod[#mod].func[#mod[#mod].func] .. ', ' .. mod[#mod].nid[#mod[#mod].nid])
	else
		mod[cur] = {}
		mod[cur].lib = string.sub(line, 1, -13)
		mod[cur].flagver = string.sub(line, -10)
		mod[cur].func = {}
		mod[cur].nid = {}
		print(mod[cur].lib .. ', ' .. mod[cur].flagver)
		cur = cur + 1
	end
end

f = io.open('MOD.BIN', 'wb')

libstrconcat = ''
callsize = 0
stubsize = 0x14 * #mod

for i = 1, #mod do
	mod[i].nidcount = 0
	mod[i].callcount = 0
	for j = 1, #mod[i].func do
		f:write('Syscalls')
		callsize = callsize + 8
		mod[i].nidcount = mod[i].nidcount + 4
		mod[i].callcount = mod[i].callcount + 8
	end
	libstr = mod[i].lib
	paddedstr = libstr .. padding(libstr)
	mod[i].padlib = paddedstr
	libstrconcat = libstrconcat .. paddedstr
end

f:write(int2bin(0) .. int2bin(0) .. int2bin(0x80000000) .. int2bin(0x00020404))

modstart = base + callsize + stubsize + #libstrconcat + 0x58
libstart = base + callsize + stubsize + 0x58
liboff = libstart

f:write(int2bin(modstart) .. int2bin(0) .. int2bin(0))

nidcount = 0
callcount = 0

for i = 1, #mod do
	f:write(int2bin(liboff) .. int2bin(mod[i].flagver) .. int2bin(5, '') .. int2bin(#mod[i].func, '') .. int2bin(modstart + 0x10 + nidcount) .. int2bin(base + callcount))
	liboff = liboff + #mod[i].padlib
	nidcount = nidcount + mod[i].nidcount
	callcount = callcount + mod[i].callcount
end

f:write(int2bin(0) .. int2bin(0x01010000))
f:write(modname)
f:write(int2bin(base + callsize + 4) .. int2bin(base + callsize + 0x14) .. int2bin(base + callsize + 0x1c) .. int2bin(base + callsize + 0x1c + (#mod * 0x14)) .. int2bin(0) .. libstrconcat)
f:write(int2bin(0xd632acdb) .. int2bin(0xcee8593c) .. int2bin(0x0f7c276c) .. int2bin(0xcf0cc697))

for i = 1, #mod do
	for j = 1, #mod[i].nid do
		f:write(int2bin(mod[i].nid[j]))
	end
end

f:close()
