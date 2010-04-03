#!/usr/bin/ruby

# crappy ruby code by wololo

#setup
stub_addr = 0x08A88D74
config = [
    { 
        :lib => "IoFileMgrForUser",
        :functions => [
            [0x109f50bc, "sceIoOpen"], 
            [0x42ec03ac, "sceIoWrite"], 
            [0x810c4bc3, "sceIoClose"], 
            [0x27EB27B8, "sceIoLseek"], 
            [0x6A638D83, "sceIoRead"],  
        ],
    },
    { 
        :lib => "ModuleMgrForUser",
        :functions => [ 
            [0x748CBED9, "sceKernelQueryModuleInfo"], 
            [0x644395E2, "sceKernelGetModuleIdList"],
            [0xD675EBB8, "sceKernelSelfStopUnloadModule"],
        ],
    } ,
    { 
        :lib => "UtilsForUser",
        :functions => [ 
            [0xB435DEC5, "sceKernelDcacheWritebackInvalidateAll"],
        ],
    }  ,  
    { 
        :lib => "ThreadManForUser",
        :functions => [ 
            [0xCEADEB47, "sceKernelDelayThread"],
            [0xF475845D, "sceKernelStartThread"],
            [0x446D8DE6, "sceKernelCreateThread"], 
            [0x616403BA, "sceKernelTerminateThread"], 
            [0x809CE29B, "sceKernelExitDeleteThread"],
            [0x9FA03CD3, "sceKernelDeleteThread"], 
            [0x17C1684E, "sceKernelReferThreadStatus"], 
            [0x383F7BCC, "sceKernelTerminateDeleteThread"],
            [0x94416130, "sceKernelGetThreadmanIdList"], 
            [0xBC6FEBC5, "sceKernelReferSemaStatus"], 
            [0xD8199E4C, "sceKernelReferFplStatus"],
            [0xA66B0120, "sceKernelReferEventFlagStatus"],
            [0x28B6489C, "sceKernelDeleteSema"],
            [0xED1410E0, "sceKernelDeleteFpl"],
            [0xEF9E4C70, "sceKernelDeleteEventFlag"],
        ],
    }   , 
    { 
        :lib => "LoadExecForUser",
        :functions => [
            [0x05572A5F, "sceKernelExitGame"],       
        ],
    },    
    { 
        :lib => "SysMemUserForUser",
        :functions => [
            [0x237DBD4F, "sceKernelAllocPartitionMemory"],
            [0xB6D61D02, "sceKernelFreePartitionMemory"],
            [0xF919F628, "sceKernelTotalFreeMemSize"],
        ],
    },        

]

# Don't change the stuff below unless you know what you do

class String
  def hex_to_bin
    return gsub(/\s/,'').to_a.pack("H*")
  end
end

class Integer
    def mips(bytes)
        return to_s(16).rjust(bytes * 2, "0").hex_to_bin.reverse
    end
end      


curr_addr = 0 #0x0880FF00 # 0x00013F00
nb_nids = 0;
nids_offset = 12
config.each {|libinfo | 
    nb_nids = nb_nids + libinfo[:functions].size
    nids_offset = nids_offset + libinfo[:lib].size + 1 + 3 * 4
}

out = File.new("sdk_hbl.S", "w")
out_conf = File.new("config/imports.config", "w");

out_conf.write(stub_addr.mips(4));
out_conf.write(nb_nids.mips(4));
out_conf.write(config.size.mips(4));

out.puts %Q{.macro AddNID funcname, offset

	.globl  \\funcname
	.ent	\\funcname
\\funcname:
	lui $v0, 0x1 # Put 0x10000 in $v0, $v0 is return value so there's no need to backup it
	lw $v0, 24($v0) # Get HBL stubs address, see eloader.c or loader.c for more info
	addi $v0, $v0, \\offset # Add offset
	jr $v0 # Jump to stub
	nop # Leave this delay slot empty!
	.end	\\funcname

.endm

	.file	1 "sdk.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.text
	.align	2

}

config.each {|libinfo | 
    lib = libinfo[:lib];
    out_conf.write(lib);
    out_conf.write("\0");
    out_conf.write(libinfo[:functions].size.mips(4));
    out_conf.write(nids_offset.mips(8));
    nids_offset = nids_offset + libinfo[:functions].size * 4
}


config.each {|libinfo | 
    out.puts;
    out.puts("# " + libinfo[:lib] + "(" + libinfo[:functions].size.to_s + ")");
    max = 0
    libinfo[:functions].each {|nidinfo | 
        if nidinfo[1].size > max then max = nidinfo[1].size end
    }
    libinfo[:functions].each {|nidinfo | 
        out.puts("\tAddNid #{nidinfo[1]}, 0x" +  curr_addr.to_s(16).rjust(4, "0") + " " * (max - nidinfo[1].size) + " # 0x" + nidinfo[0].to_s(16));       
        out_conf.write(nidinfo[0].mips(4));
        curr_addr += 8;
    }
    
}


out.puts %Q{


	.ident	"HBL-SDK"
}
out.close
out_conf.close
