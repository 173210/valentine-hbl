#!/usr/bin/ruby

# crappy ruby code by wololo

root = "../../"

# .lib.stub addresses
exploit_stubs_map = {
	# Gamocracy One: Legend of Robot - EU version (qwikrazor87)
	"gamo_eu" => {
		"6xx" =>
			[
		0x0888C5F8, #Gamocracy
		0x09F24C30, #sceATRAC3plus_Library
		0x088009A0, #sceKernelLibrary
			],
	},

	# Project Life - KR Version (wololo, exploit by Teck4)
	"lifekr" => {
		"6xx" =>
			[	
                0x08A5A0DC, #game main module
                0x08ACCC30, #sceLibrary
                0x088009A0, #sceLibrary
                0x08AD3950, #sceLibrary
                0x08ADCA30, #sceLibrary
                0x08AE3A30, #sceLibrary
 			],	
	},   


	# Project Life - US Version (wololo, exploit by Teck4)
	"lifeus" => {
		"6xx" =>
			[	
                0x08A5B328, #game main module
                0x08ACDC30, #sceLibrary
                0x088009A0, #sceLibrary
                0x08AD4950, #sceLibrary
                0x08ADDA30, #sceLibrary
                0x08AE4A30, #sceLibrary
 			],	
	},   

	# Project Life - EU Version (wololo, exploit by Teck4)
	"lifeeu" => {
		"6xx" =>
			[	
            0x08A5B940, #game main module
            0x08ACDC30, #sceLibrary
            0x088009A0, #sceLibrary
            0x08AD4950, #sceLibrary
            0x08ADDA30, #sceLibrary
            0x08AE4A30, #sceLibrary
 			],	
	},           
	# Project Life - JP Version (wololo, exploit by Teck4)
	"lifejp" => {
		"6xx" =>
			[			
				0x08A5A280, #game main module
				0x08ACCC30, #sceLibrary
				0x088009A0, #sceLibrary
				0x08AD3950, #sceLibrary
				0x08ADCA30, #scelibrary
				0x08AE3A30, #scelibrary				
			],	
	},

	"signed" => {
		"6xx" =>
			[
				0x08807cb8 #hbl_launcher
			],
	},
	# Everybody's Sukkiri exploit by Darxploit, Flyer, minomushi, some1
	"sukkiri" => {
		"5xx" =>
			[
				0x088BE3AC, #main
				0x09D03760, #sceNet_Library
				0x09CC6230, #sceATRAC3plus_Library
				#0x0880046C, #sceKernelLibrary
				0x09D150B0, #sceNetAdhoc_Library
				0x09D1B400, #sceNetAdhocctl_Library
				0x09D25700, #sceNetAdhocDownload_Library
				0x09D219F0, #sceNetAdhocMatching_Library
				# Relocated message dialog stubs
				0x08EE0000, # scePaf_Module
				0x08F00000, # sceVshCommonUtil_Module
				0x08F20000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x08F40000  # sceVshSDAuto_Module				
			],		
		"570" =>
			[
				0x088BE3AC, #main
				0x09D039E0, #sceNet_Library
				0x09CC6240, #sceATRAC3plus_Library
				0x088005BC, #sceKernelLibrary
				0x09D150F0, #sceNetAdhoc_Library
				0x09D1B500, #sceNetAdhocctl_Library
				0x09D26190, #sceNetAdhocDownload_Library
				0x09D21CC0, #sceNetAdhocMatching_Library
				0x09CD2570, #sceFont_Library
				# Relocated message dialog stubs
				0x08EE0000, # scePaf_Module
				0x08F00000, # sceVshCommonUtil_Module
				0x08F20000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x08F40000  # sceVshSDAuto_Module					
			],
		"6xx" =>
			[		
				0x088BE3AC, #main
				0x09D03E40, #sceNet_Library
				0x09CC6830, #sceATRAC3plus_Library
				0x088009A0, #sceKernelLibrary
				0x09D150F0, #sceNetAdhoc_Library
				0x09D1B570, #sceNetAdhocctl_Library
				0x09D26290, #sceNetAdhocDownload_Library
				0x09D21E10, #sceNetAdhocMatching_Library
				# Relocated message dialog stubs
				0x08EE0000, # scePaf_Module
				0x08F00000, # sceVshCommonUtil_Module
				0x08F20000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x08F40000  # sceVshSDAuto_Module					
			],
	},
	# Everybody's Golf 1
	"everybody" => {
		"50x" =>
			[
				0x08A384EC, #main
				0x09E6DE8C, #sceNet_Library
				0x09E6419C, #sceATRAC3plus_Library
				0x0880043C, #sceKernelLibrary
				0x09E7769C, #sceNetAdhoc_Library
				0x09E7DEAC, #sceNetAdhocctl_Library
				0x09E82C2C, #sceNetAdhocMatching_Library
				0x09E8A67C, #sceMpeg_library	
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module	
			],	
		"550" =>
			[
				0x08A384EC, #main
				0x09E6DE8C, #sceNet_Library
				0x09E6419C, #sceATRAC3plus_Library
				0x0880046C, #sceKernelLibrary
				0x09E7769C, #sceNetAdhoc_Library
				0x09E7DEAC, #sceNetAdhocctl_Library
				0x09E82C2C, #sceNetAdhocMatching_Library
				0x09E8A67C, #sceMpeg_library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module				
			],	
		"6xx" =>
			[			
				0x08A384EC, #main
				0x09E6DE8C, #sceNet_Library
				0x09E6419C, #sceATRAC3plus_Library
				0x088009A0, #sceKernelLibrary
				0x09E7769C, #sceNetAdhoc_Library
				0x09E7DEAC, #sceNetAdhocctl_Library
				0x09E82C2C, #sceNetAdhocMatching_Library
				0x09E8A67C, #sceMpeg_library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module				
			],	
	},
	# Everybody's Golf 2
	"everybody2" => {
		"50x" =>
			[
				0x08ACF408, #main
				0x09E9E630, #sceATRAC3plus_Library
				0x0880043C, #sceKernelLibrary
				0x09EAB0C0, #sceFont_Library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module	
			],
		"550" =>
			[
				0x08ACF408, #main
				0x09E9E630, #sceATRAC3plus_Library
				0x0880046C, #sceKernelLibrary
				0x09EAB0C0, #sceFont_Library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module	
			],
		"6xx" =>
			[
				0x08ACF408, #main
				0x09E9EC30, #sceATRAC3plus_Library
				0x088009A0, #sceKernelLibrary
				0x09EAB0C0, #sceFont_Library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module	
			],
	},	
	# Hotshots golf 1 Exploit (wololo)
	"hotshots" => {
		"5xx" =>
			[
                0x08A35EAC, #main
                0x09E6678C, #sceNet_Library
                0x09E5CA9C, #sceATRAC3plus_Library
                0x0880046C, #sceKernelLibrary
                0x09E6FF9C, #sceNetAdhoc_Library
                0x09E767AC, #sceNetAdhocctl_Library
                0x09E7B52C, #sceNetAdhocMatching_Library
                0x09E82F7C, #sceMpeg_library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module					
			],	
		"6xx" =>
			[
                0x08A35EAC, #main
                0x09E6678C, #sceNet_Library
                0x09E5CA9C, #sceATRAC3plus_Library
                0x088009A0, #sceKernelLibrary
                0x09E6FF9C, #sceNetAdhoc_Library
                0x09E767AC, #sceNetAdhocctl_Library
                0x09E7B52C, #sceNetAdhocMatching_Library
                0x09E82F7C, #sceMpeg_library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
			],
	},
	# Hotshots golf 1 Exploit - Alternative version (wololo/JJS)
	"hotshotsalt" => {
		"5xx" =>
			[
				0x08A35FEC, #main
				0x09E66B8C, #sceNet_Library
				0x09E5CE9C, #sceATRAC3plus_Library
				0x0880043C, #sceKernelLibrary
				0x09E7039C, #sceNetAdhoc_Library
				0x09E76BAC, #sceNetAdhocctl_Library
				0x09E7B92C, #sceNetAdhocMatching_Library
				0x09E8337C, #sceMpeg_library			
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
			],
		"550" =>
			[
				0x08A35FEC, #main
				0x09E6698C, #sceNet_Library
				0x09E5CC9C, #sceATRAC3plus_Library
				0x0880046C, #sceKernelLibrary
				0x09E7019C, #sceNetAdhoc_Library
				0x09E769AC, #sceNetAdhocctl_Library
				0x09E7B72C, #sceNetAdhocMatching_Library
				0x09E8317C, #sceMpeg_library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
			],
		"6xx" =>
			[
				0x08A35FEC, #main
				0x09E6698C, #sceNet_Library
				0x09E5CC9C, #sceATRAC3plus_Library
				0x088009A0, #sceKernelLibrary
				0x09E7019C, #sceNetAdhoc_Library
				0x09E769AC, #sceNetAdhocctl_Library
				0x09E7B72C, #sceNetAdhocMatching_Library
				0x09E8317C, #sceMpeg_library			
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
			],			
	},
    # Hotshots golf 2 Exploit (wololo)
	"hotshots2" => {
		"5xx" =>
			[
                0x08ACFA7C, #main
                0x09E88630, #sceATRAC3plus_Library
                0x0880043C, #sceKernelLibrary
                0x09E950C0, #sceFont_Library   
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
    		],
		"6xx" =>
			[
				0x08ACFA7C, #main
				0x09E88C30, #sceATRAC3plus_Library
				0x088009A0, #sceKernelLibrary
				0x09E950C0, #sceFont_Library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
    		],
	},
    
	# Minna no golf 1 Exploit (j416)
	"minna" => {
		"5xx" =>
			[
				0x08A1CCCC,
				0x0880843C,
				0x09E8809C,
				0x09EE8C7C,
				0x09E91D8C,
				0x09E9B59C,
				0x09EA1DAC,
				0x09EA6AEC,
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
			],	
		"6xx" =>
			[
				0x08A1CCCC,
				0x0880843C,
				0x09E8809C,
				0x09EE8C7C,
				0x09E91D8C,
				0x09E9B59C,
				0x09EA1DAC,
				0x09EA6AEC,
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
			],
	},
	# Minna no golf 2 original release Exploit (j416)	
	"minna2" => {
		"550" =>
			[
				0x08ACCB08, #main
				0x09E32630, #sceATRAC3plus_Library
				0x0880046C, #sceKernelLibrary
				0x09E3F0B0, #sceFont_Library			
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
			],
		"6xx" =>
			[
				0x08ACCB08, #main
				0x09E32C30, #sceATRAC3plus_Library
				0x088009A0, #sceKernelLibrary
				0x09E3F0B0, #sceFont_Library		
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
    		],
	},
	# Minna no golf 2 "the best" budget release Exploit (j416)	
	"minna2best" => {
		"6xx" =>
			[
				0x08ACD4A4, #main
				0x09E93C30, #sceATRAC3plus_Library
				0x088009A0, #sceKernelLibrary
				0x09EA00B0, #sceFont_Library
				# Relocated message dialog stubs
				0x09d10000, # scePaf_Module
				0x09d30000, # sceVshCommonUtil_Module
				0x09d50000, # sceDialogmain_Module
				# Relocated savegame dialog stub
				0x09d70000  # sceVshSDAuto_Module
    		],
	},
    # Patapon 2 Demo Exploit (malloxis & wololo)
	"patapon2" => {
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
					0x09ec6b70, # sceFont_Library
					# Relocated message dialog stubs
					0x09d10000, # scePaf_Module
					0x09d30000, # sceVshCommonUtil_Module
					0x09d50000, # sceDialogmain_Module
					# Relocated savegame dialog stub
					0x09d70000  # sceVshSDAuto_Module											
				 ],
        "503" =>
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
					0x09ec6b70, # sceFont_Library
					# Relocated message dialog stubs
					0x09d10000, # scePaf_Module
					0x09d30000, # sceVshCommonUtil_Module
					0x09d50000, # sceDialogmain_Module
					# Relocated savegame dialog stub
					0x09d70000  # sceVshSDAuto_Module						
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
					0x09ec6b70, # sceFont_Library
					# Relocated message dialog stubs
					0x09d10000, # scePaf_Module
					0x09d30000, # sceVshCommonUtil_Module
					0x09d50000, # sceDialogmain_Module
					# Relocated savegame dialog stub
					0x09d70000  # sceVshSDAuto_Module						
				 ],
		"55x" =>
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
					0x09ec6b70, # sceFont_Library
					# Relocated message dialog stubs
					0x09d10000, # scePaf_Module
					0x09d30000, # sceVshCommonUtil_Module
					0x09d50000, # sceDialogmain_Module
					# Relocated savegame dialog stub
					0x09d70000  # sceVshSDAuto_Module						
					
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
					0x09ec6b70, # sceFont_Library
					# Relocated message dialog stubs
					0x09d10000, # scePaf_Module
					0x09d30000, # sceVshCommonUtil_Module
					0x09d50000, # sceDialogmain_Module
					# Relocated savegame dialog stub
					0x09d70000  # sceVshSDAuto_Module						
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
					0x09ec6b70, # sceFont_Library
					# Relocated message dialog stubs
					0x09d10000, # scePaf_Module
					0x09d30000, # sceVshCommonUtil_Module
					0x09d50000, # sceDialogmain_Module
					# Relocated savegame dialog stub
					0x09d70000  # sceVshSDAuto_Module
				 ],   
    },				 
}

# HBL stubs
config = [
	{
		:lib => "InterruptManager",
		:functions => [
			[0xD61E6961, "sceKernelReleaseSubIntrHandler"],
		],
	},
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
            [0x55F4717D, "sceIoChdir"], 
            
        ],
    },
    { 
        :lib => "ModuleMgrForUser",
        :functions => [ 
            [0xD1FF982A, "sceKernelStopModule"],
            [0x2E0911AA, "sceKernelUnloadModule"],
            [0xD8B73127, "sceKernelGetModuleIdByAddress"],
            [0x644395E2, "sceKernelGetModuleIdList"],
            [0x8F2DF740, "ModuleMgrForUser_8F2DF740"], #sceKernelSelfStopUnloadModuleWithStatus, see hook.c
        ],
    } ,
    { 
        :lib => "UtilsForUser",
        :functions => [ 
			# only one of sceKernelDcacheWritebackInvalidateAll and sceKernelDcacheWritebackAll is used, it depends on the exploit,
			# it might be wise to turn that into an exploit-specific config ?
            [0xB435DEC5, "sceKernelDcacheWritebackInvalidateAll"],
			[0x79D1C3FA, "sceKernelDcacheWritebackAll"],
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
            [0x28B6489C, "sceKernelDeleteSema"],
            [0xED1410E0, "sceKernelDeleteFpl"],
            [0xef9e4c70, "sceKernelDeleteEventFlag"], 
            [0xEDBA5844, "sceKernelDeleteCallback"],
            [0x7E65B999, "sceKernelCancelAlarm"],
            [0xAA73C935, "sceKernelExitThread"],
            [0x68DA9E36, "sceKernelDelayThreadCB"],
            [0x876DBFAD, "sceKernelSendMsgPipe"],
            [0x74829B76, "sceKernelReceiveMsgPipe"],
            [0xDF52098F, "sceKernelTryReceiveMsgPipe"],
            [0x293B45B8, "sceKernelGetThreadId"],
            [0xE81CAF8F, "sceKernelCreateCallback"],
            [0x3F53E640, "sceKernelSignalSema"],
            [0x4E3A1105, "sceKernelWaitSema"],
            [0xD6DA4BA1, "sceKernelCreateSema"],
        ],
    }   , 
    { 
        :lib => "LoadExecForUser",
        :functions => [
            [0x05572A5F, "sceKernelExitGame"],   
            [0x4AC57943, "sceKernelRegisterExitCallback"],            
        ],
    },    
    { 
        :lib => "SysMemUserForUser",
        :functions => [
            [0x237DBD4F, "sceKernelAllocPartitionMemory"],
            [0x9D9A5BA1, "sceKernelGetBlockHeadAddr"],
            [0xB6D61D02, "sceKernelFreePartitionMemory"],
# The two following can be overriden by some of our code, which will not fail
# Our code is slow though, so if syscalls can be done 100% perfect at some point let's put those back
#            [0xF919F628, "sceKernelTotalFreeMemSize"],
#            [0xA291F107, "sceKernelMaxFreeMemSize"],
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
            [0x3A622550, "sceCtrlPeekBufferPositive"],
        ],
    },
    {
    	:lib => "sceUmdUser",
    	:functions => [
    		[0xBD2BDE07, "sceUmdUnRegisterUMDCallBack"],
    	],
    },  
    {
    	:lib => "sceAudio",
    	:functions => [
    		[0x6FC46853, "sceAudioChRelease"],
            [0xB011922F, "sceAudioGetChannelRestLength"],
            [0x136CAF51, "sceAudioOutputBlocking"],
            [0x13F592BC, "sceAudioOutputPannedBlocking"],
            [0x5EC81C55, "sceAudioChReserve"],
            
    	],
    },  
    {
    	:lib => "sceRtc",
    	:functions => [
    		[0xE7C27D1B, "sceRtcGetCurrentClockLocalTime"],
    	],
    }, 
    {
    	:lib => "scePower",
    	:functions => [
    		[0x469989AD, "scePower_469989AD"], # == scePowerSetClockFrequency see hook.c
    		[0xEBD177D6, "scePower_EBD177D6"], # == scePowerSetClockFrequency see hook.c
    	],
    },     
   	{
    	:lib => "sceUtility",
    	:functions => [
    		[0x2A2B3DE0, "sceUtilityLoadModule"],
    		[0xE49BFE92, "sceUtilityUnloadModule"],
            [0x67AF3428, "sceUtilityMsgDialogShutdownStart"],
            [0x9A1C91D7, "sceUtilityMsgDialogGetStatus"],
            [0x95FC253B, "sceUtilityMsgDialogUpdate"],
            [0x2AD8E239, "sceUtilityMsgDialogInitStart"],
    	],
    },  
   	{
    	:lib => "sceGe_user",
    	:functions => [
    		[0x1F6752AD, "sceGeEdramGetSize"],
    		[0xE47E40E4, "sceGeEdramGetAddr"],
    	],
    }
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



nb_nids = 0;
nids_offset = 0;
config.each {|libinfo | 
    nb_nids = nb_nids + libinfo[:functions].size
    nids_offset = nids_offset + libinfo[:lib].size + 1 + 3 * 4
}

# Write new loader.h
start_addr = 0x10000; #0x0880FF00 # 0x00013F00
out_loader_h = File.new( root + "loader.h", "w");
out_loader_h.binmode;
out_loader_h.puts %Q[/* This file was automatically generated by eLoaderconf.rb */
#ifndef ELOADER_LOADER
#define ELOADER_LOADER

// HBL stubs size
]
out_loader_h.puts "#define NUM_HBL_IMPORTS"  + " 0x"  + nb_nids.to_s(16);
out_loader_h.puts "// HBL stubs address\n#define HBL_STUBS_START "  + " 0x"  + start_addr.to_s(16);
out_loader_h.puts %Q[#define GLOBALS_ADDR 0x10200
#define GLOBALS_END_ADDR 0x14000

#endif
]

puts "#define NUM_HBL_IMPORTS"  + " 0x"  + nb_nids.to_s(16);


# Game Specific config files
exploit_stubs_map.each { |folder, stub_addresses|
    curr_addr = start_addr
	out = File.new( root + folder +  "/sdk_hbl.S", "w")
	out.binmode();
	out_conf = {};
    config_folder = root + folder + "/config";
    Dir.mkdir(config_folder) unless File.directory?(config_folder);
	stub_addresses.each { |k, v|
		out_conf[k] = File.new( config_folder + "/imports.config_" + k  , "w");
        puts config_folder + "/imports.config_" + k;
		out_conf[k].binmode;
		out_conf[k].write(v.size.mips(4));
		out_conf[k].write(config.size.mips(4));
		out_conf[k].write(nb_nids.mips(4));
		v.each{ |stub_addr|
			out_conf[k].write(stub_addr.mips(4));
		}
	}

	out.puts %Q[# This file was automatically generated by eLoaderconf.rb
	# If you want to edit this file, edit eLoaderconf.rb instead
	.macro AddNID funcname, offset

		.globl  \\funcname
		.ent    \\funcname
	\\funcname = \\offset
	#\\funcname:
	#	lui $v0, 0x1            # Put 0x10000 in $v0, $v0 is return value so there's no need to back it up
	#	lw $v0, 0x18 ($v0)      # Get HBL stubs address, see eloader.c or loader.c for more info
	#	addi $v0, $v0, \\offset # Add offset
	#	jr $v0                  # Jump to stub
	#	nop                     # Leave this delay slot empty!
		.end	\\funcname

	.endm

		.file	1 "sdk.c"
		.section .mdebug.eabi32
		.section .gcc_compiled_long32
		.previous
		.text
		.align	2

	]




	out_conf.each { |k, v|
		nids_offset_file = nids_offset + 12 + stub_addresses[k].size * 4;
		config.each {|libinfo | 
			lib = libinfo[:lib];
			v.write(lib);
			v.write("\0");
			v.write(libinfo[:functions].size.mips(4));
			v.write(nids_offset_file.mips(8));
			nids_offset_file = nids_offset_file + libinfo[:functions].size * 4
		}
	}


	config.each {|libinfo | 
		out.puts;
		out.puts("# " + libinfo[:lib] + "(" + libinfo[:functions].size.to_s + ")");
		max = 0
		libinfo[:functions].each {|nidinfo | 
			if nidinfo[1].size > max then max = nidinfo[1].size end
		}
		libinfo[:functions].each {|nidinfo | 
			out.puts("\tAddNID #{nidinfo[1]}, 0x" +  curr_addr.to_s(16).rjust(4, "0") + " " * (max - nidinfo[1].size) + " # 0x" + nidinfo[0].to_s(16));   
			out_conf.each { |k, v|        
				v.write(nidinfo[0].mips(4));
			}
			curr_addr += 8;
		}
		
	}


	out.puts %Q[
		.ident	"HBL-SDK"
	]

	out_conf.each { |k , v |
	 v.close
	}
}
