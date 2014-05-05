#!/usr/bin/ruby

lib = ["Kernel_Library"]

def scandump(dump, search, count)
	filepos = dump.index(search);
	if filepos then
		count += 1
		dump2 = dump[filepos + 8 .. -1];
		count = scandump(dump2, search, count);
	end
	count
end

listdir = []
Dir.entries(".").each { |entry|
	listdir.push(entry) if entry[-4 .. -1] == ".bin"
}

listdir.sort! { |x, y| x <=> y }

listdir.each { |entry|
	f = File.open(entry, 'rb')
	r = f.read()
	f.close
	puts entry
	n = 0
	lib.each { |libs|
		n = scandump(r, libs + 0.chr, n);
	}
	puts "\t" + n.to_s
}