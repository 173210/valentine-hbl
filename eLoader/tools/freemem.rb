#!/usr/bin/ruby
# Creates the memory address arrays for the free memory step of HBL.
#
# Required input files are:
#
# "memdump.bin", a user memory dump from PSPLink with 
# "savemem 0x08800000 0x01800000 memdump.bin"
#
# and from the same game session:
#
# "uidlist.txt", which is the the output of the "uidlist"
# command in PSPLink
#
# Initial code by JJS

# Copied from eLoaderconf.rb
class String
# Thanks to mon_Ouie for ruby 1.9 compatibility
  def to_a
    [self]
  end

  def hex_to_bin
    return gsub(/\s/,'').to_a.pack("H*")
  end
end

class Integer
    def mips(bytes)
        return to_s(16).rjust(bytes * 2, "0").hex_to_bin.reverse
    end
end     


fileEntry = Struct.new(:type, :uid, :name)
addresses = Array.new
currentHeader = nil
baseAddress = 0x08800000

uidlist = File.new("uidlist.txt", "r")
memdumpfile = File.new("memdump.bin", "r")
memdumpfile.binmode
memdump = memdumpfile.read


#[Fpl]    UID 0x00288E0D (attr 0x0 entry 0x88014470)
regexpHeader = /^\[(.+)\]    UID (0x.+) \(attr (0x.+) entry (0x.+)\)$/

#(UID): 0x048E3761, (entry): 0x882471b8, (size): 48, (attr): 0xFF, (Name): sgx-ao-evf
regexpEntry = /^\(UID\): (0x.+), \(entry\): (0x.+), \(size\): (.+), \(attr\): (0x.+), \(Name\): (.+)$/

while (line = uidlist.gets)
    line.chomp!
	if (line.match(regexpHeader)) 
		line.scan(regexpHeader) { |name, uid, attr, entry|
			currentHeader = name
		}
	end
	
	if (line.match(regexpEntry)) 
		line.scan(regexpEntry) { |uid, entry, size, attr, name|
			if attr == "0xFF"
				# skip thread stacks and the "main" thread which is unloaded with the module
				if (name.index("stack:") == nil) and not ((currentHeader == "Thread") and (name == "main"))
					addresses.push(fileEntry.new(currentHeader, uid, name))
				end
			end
		}
	end
end

currentHeader = nil

addresses.each { |addressEntry|
	if addressEntry.type == "Thread"
		# the thread uid is right next to the thread stack
		index = memdump.index(addressEntry.uid.to_i(16).mips(4) + "FF".to_i(16).mips(1))
	else
		index = memdump.index(addressEntry.uid.to_i(16).mips(4))
	end
	
	if (index != nil)
		if currentHeader != addressEntry.type
			if currentHeader != nil
				print " }\n\n"
			end
			
			currentHeader = addressEntry.type
			
			if addressEntry.type == "Thread"
				print "#define TH_ADDR_LIST { "
			elsif addressEntry.type == "EventFlag"
				print "#define EV_ADDR_LIST { "
			elsif addressEntry.type == "Semaphore"
				print "#define SEMA_ADDR_LIST { "
			elsif addressEntry.type == "SceSysMemMemoryBlock"
				print "#define GAME_FREEMEM_ADDR { "
			else
				print "#define "+addressEntry.type.upcase+"_ADDR { "
			end
		else
			print ", "
		end
		
		print "0x0#{(index + baseAddress).to_s(16).upcase}"
	end
}
	
print " }"