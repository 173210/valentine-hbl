#!/usr/bin/ruby

# crappy ruby code by wololo

root = "../../"

# .lib.stub addresses
stub_addresses = {
        "50x" =>
                [
					0x08A88D74, # Labo
					0x0880043c, # sceKernelLibrary
					0x09e76e30, # sceATRAC3plus_Library
					0x09e82a20, # sceMpeg_library
					0x09e91460, # sceNet_Library
					0x09ea2db0, # sceNetAdhoc_Library
					0x09ea9100, # sceNetAdhocctl_Library
					0x09eaf6f0, # sceNetAdhocMatching_Library
					0x09eb3400, # sceNetAdhocDownload_Library
					0x09eb483c, # sceNetAdhocDiscover_Library
					0x09ec6b70  # sceFont_Library
				 ],
        "550" =>
                [
					0x08A88D74, # Labo
					0x0880046c, # sceKernelLibrary
					0x09e76e30, # sceATRAC3plus_Library
					0x09e82a20, # sceMpeg_library
					0x09e91460, # sceNet_Library
					0x09ea2db0, # sceNetAdhoc_Library
					0x09ea9100, # sceNetAdhocctl_Library
					0x09eaf6f0, # sceNetAdhocMatching_Library
					0x09eb3400, # sceNetAdhocDownload_Library
					0x09eb483c, # sceNetAdhocDiscover_Library
					0x09ec6b70  # sceFont_Library
				 ],
		"555" =>
                [
					0x08A88D74, # Labo
					0x0880046c, # sceKernelLibrary
					0x09e76e30, # sceATRAC3plus_Library
					0x09e82a20, # sceMpeg_library
					0x09e91440, # sceNet_Library
					0x09ea2db0, # sceNetAdhoc_Library
					0x09ea9100, # sceNetAdhocctl_Library
					0x09eaf6f0, # sceNetAdhocMatching_Library
					0x09eb3400, # sceNetAdhocDownload_Library
					0x09eb483c, # sceNetAdhocDiscover_Library
					0x09ec6b70  # sceFont_Library
				 ],
        "570" =>
                [
					0x08A88D74, # Labo
					0x088005bc, # sceKernelLibrary
					0x09e76E40, # sceATRAC3plus_Library
					0x09e82DA0, # sceMpeg_library
					0x09e916E0, # sceNet_Library
					0x09ea2dF0, # sceNetAdhoc_Library
					0x09ea9200, # sceNetAdhocctl_Library
					0x09eaf9c0, # sceNetAdhocMatching_Library
					0x09eb3E90, # sceNetAdhocDownload_Library
					0x09eb533c, # sceNetAdhocDiscover_Library
					0x09ec6b70  # sceFont_Library
				 ],
        "6xx" =>
                [
					0x08A88D74, # Labo
					0x088009A0, # sceKernelLibrary
					0x09e77430, # sceATRAC3plus_Library
					0x09e83230, # sceMpeg_library
					0x09e91B40, # sceNet_Library
					0x09ea2dF0, # sceNetAdhoc_Library
					0x09ea9270, # sceNetAdhocctl_Library
					0x09eafB10, # sceNetAdhocMatching_Library
					0x09eb3F90, # sceNetAdhocDownload_Library
					0x09eb543c, # sceNetAdhocDiscover_Library
					0x09ec6b70  # sceFont_Library
				 ],                 
}

# HBL stubs
config = [
    { 
        :lib => "IoFileMgrForUser",
        :functions => [
            [0x109f50bc, "sceIoOpen"], 
            [0x42ec03ac, "sceIoWrite"], 
            [0x810c4bc3, "sceIoClose"], 
            [0x27EB27B8, "sceIoLseek"], 
            [0x6A638D83, "sceIoRead"],  
            [0xB29DDF9C, "sceIoDopen"],
            [0xE3EB004C, "sceIoDread"],
            [0xEB092469, "sceIoDclose"],
            [0x54F5FB11, "sceIoDevctl"],        
        ],
    },
    { 
        :lib => "ModuleMgrForUser",
        :functions => [ 
            [0x748CBED9, "sceKernelQueryModuleInfo"], 
            [0x644395E2, "sceKernelGetModuleIdList"],
            [0xD1FF982A, "sceKernelStopModule"],
            [0x2E0911AA, "sceKernelUnloadModule"],
            [0xD8B73127, "sceKernelGetModuleIdByAddress"],
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
            [0xa66b0120, "sceKernelReferEventFlagStatus"], 
            [0x28B6489C, "sceKernelDeleteSema"],
            [0xED1410E0, "sceKernelDeleteFpl"],
            [0xef9e4c70, "sceKernelDeleteEventFlag"], 
            [0x293B45B8, "sceKernelGetThreadId"],
            [0xEDBA5844, "sceKernelDeleteCallback"],
            [0x532A522E, "sceKernelExitThread"],
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
            [0x9D9A5BA1, "sceKernelGetBlockHeadAddr"],
            [0xB6D61D02, "sceKernelFreePartitionMemory"],
            [0xF919F628, "sceKernelTotalFreeMemSize"],
        ],
    },   
    { 
        :lib => "sceDisplay",
        :functions => [
            [0x289D82FE, "sceDisplaySetFrameBuf"],
        ],
    },  
    { 
        :lib => "sceCtrl",
        :functions => [
            [0x1F803938, "sceCtrlReadBufferPositive"],
        ],
    },
    {
    	:lib => "sceUmdUser",
    	:functions => [
    		[0xBD2BDE07, "sceUmdUnRegisterUMDCallBack"],
    	],
    },      

]

# Don't change the stuff below unless you know what you do

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


curr_addr = 0; #0x0880FF00 # 0x00013F00
nb_nids = 0;
nids_offset = 12 + stub_addresses["50x"].size * 4
config.each {|libinfo | 
    nb_nids = nb_nids + libinfo[:functions].size
    nids_offset = nids_offset + libinfo[:lib].size + 1 + 3 * 4
}
puts "#define NUM_HBL_IMPORTS"  + " 0x"  + nb_nids.to_s(16);

out = File.new( root + "sdk_hbl.S", "w")
out_conf = {};
stub_addresses.each { |k, v|
    out_conf[k] = File.new( root + "config/imports.config_" + k  , "w");
    out_conf[k].write(v.size.mips(4));
    out_conf[k].write(config.size.mips(4));
    out_conf[k].write(nb_nids.mips(4));
    v.each{ |stub_addr|
        out_conf[k].write(stub_addr.mips(4));
    }
}

out.puts %Q{# This file was automatically generated by eLoaderconf.rb
# If you want to edit this file, edit eLoaderconf.rb instead
.macro AddNID funcname, offset

	.globl  \\funcname
	.ent    \\funcname
\\funcname:
	lui $v0, 0x1            # Put 0x10000 in $v0, $v0 is return value so there's no need to back it up
	lw $v0, 0x18 ($v0)      # Get HBL stubs address, see eloader.c or loader.c for more info
	addi $v0, $v0, \\offset # Add offset
	jr $v0                  # Jump to stub
	nop                     # Leave this delay slot empty!
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
    out_conf.each { |k, v|
        v.write(lib);
        v.write("\0");
        v.write(libinfo[:functions].size.mips(4));
        v.write(nids_offset.mips(8));
    }
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
        out_conf.each { |k, v|        
            v.write(nidinfo[0].mips(4));
        }
        curr_addr += 8;
    }
    
}


out.puts %Q{
	.ident	"HBL-SDK"
}

out_conf.each { |k , v |
 v.close
}
