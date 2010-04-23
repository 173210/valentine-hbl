#include "sdk.h"
#include "eloader.h"
#include "scratchpad.h"
#include "elf.h"
#include "loader.h"
#include "thread.h"
#include "debug.h"
#include "config.h"
#include "menu.h"
#include "graphics.h"
#include "utils.h"
#include "tables.h"
#include "hook.h"

/* eLoader */
/* Entry point: _start() */

// Any way to have those non globals ?
u32 gp = 0;
u32* entry_point = 0;
u32 hbsize = 4000000; //default value for the hb size roughly 4MB. This value is never used in theory

// Menu variables
int g_menu_enabled = 0; // this is set to 1 at runtime if a menu.bin file exists
u32 * isSet = EBOOT_SET_ADDRESS;
u32 * ebootPath = EBOOT_PATH_ADDRESS;  
u32 * menu_pointer = MENU_LOAD_ADDRESS;

// This function can be used to catch up the import calls when the ELF is up and running
// For debugging only; does not perform the calls
/*
#ifdef DEBUG
		
	void late_import(void)
	{
		asm("\tsw $ra, aux\n");
		int i;
		
		dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_APPEND | PSP_O_WRONLY, 0777);
		sceIoWrite(dbglog, &aux, sizeof(u32));	
		sceIoClose(dbglog);			
	}
 
#endif
*/

// Autoresolves HBL missing stubs
// Some stubs are compulsory, like sceIo*
void resolve_missing_stubs()
{
	int i, ret;
	unsigned int num_nids;
	u32* cur_stub = *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR;
	u32 nid = 0, syscall = 0;
	char lib_name[MAX_LIBRARY_NAME_LENGTH];

	ret = config_initialize();

	if (ret < 0)
		exit_with_log("**CONFIG INIT FAILED**", &ret, sizeof(ret));

	ret = config_num_nids_total(&num_nids);

#ifdef DEBUG
	LOGSTR0("--> HBL STUBS BEFORE ESTIMATING:\n");	
	for(i=0; i<num_nids; i++)
	{
		LOGSTR2("--Stub address: 0x%08lX (offset: 0x%08lX)\n", cur_stub, (u32)cur_stub - *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR);
		LOGSTR1("  0x%08lX ", *cur_stub);
		cur_stub++;
		LOGSTR1("0x%08lX\n", *cur_stub);
		cur_stub++;
	}
	cur_stub = *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR;
#endif
	
	for (i=0; i<num_nids; i++)
	{
		if (*cur_stub == 0)
		{
			LOGSTR1("-Resolving unknown import 0x%08lX: ", (u32)cur_stub - *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR);

			// Get NID & library for i-th import
			ret = get_lib_nid(i, lib_name, &nid);

			LOGSTR0(lib_name);
			LOGSTR1(" 0x%08lX\n", nid);

			// Is it known by HBL?
			ret = get_nid_index(nid);

			// If it's known, get the call
			if (ret > 0)
				syscall = nid_table[ret].call;
			
			// If not, estimate
			else
				syscall = estimate_syscall(lib_name, nid);

			// DEBUG_PRINT("**RESOLVED SYS**", lib_name, strlen(lib_name));
			// DEBUG_PRINT(" ", &syscall, sizeof(syscall));

			resolve_call(cur_stub, syscall);
		}
		cur_stub += 2;
	}

#ifdef DEBUG
	cur_stub = *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR;
	LOGSTR0("--> HBL STUBS AFTER ESTIMATING:\n");	
	for(i=0; i<num_nids; i++)
	{
		LOGSTR2("--Stub address: 0x%08lX (offset: 0x%08lX)\n", cur_stub, (u32)cur_stub - *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR);
		LOGSTR1("  0x%08lX ", *cur_stub);
		cur_stub++;
		LOGSTR1("0x%08lX\n", *cur_stub);
		cur_stub++;
	}	
#endif	

	sceKernelDcacheWritebackInvalidateAll();

	config_close();
}

// Subsitutes the right instruction
void resolve_call(u32 *call_to_resolve, u32 call_resolved)
{
	/*
	#ifdef DEBUG
		dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
		if(dbglog >= 0)
		{
			sceIoWrite(dbglog, " CALL TO RESOLVE ", strlen(" CALL TO RESOLVE "));
			sceIoWrite(dbglog, &call_to_resolve, sizeof(u32*));
			sceIoWrite(dbglog, " CALL RESOLVED ", strlen(" CALL RESOLVED "));
			sceIoWrite(dbglog, &call_resolved, sizeof(u32));
			sceIoClose(dbglog);
		}
	#endif
	*/

	// SYSCALL
	if(!(call_resolved & SYSCALL_MASK_RESOLVE))
	{
		/*
		#ifdef DEBUG
			dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
			if(dbglog >= 0)
			{
				sceIoWrite(dbglog, " SYSCALL ", strlen(" SYSCALL "));
				sceIoClose(dbglog);
			}
		#endif
		*/
		*call_to_resolve = JR_RA_OPCODE;
		*(++call_to_resolve) = call_resolved;
	}
	
	// JUMP
	else
	{
		/*
		#ifdef DEBUG
			dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
			if(dbglog >= 0)
			{
				sceIoWrite(dbglog, " JUMP ", strlen(" JUMP "));
				sceIoClose(dbglog);
			}
		#endif
		*/
		*call_to_resolve = call_resolved;
		*(++call_to_resolve) = NOP_OPCODE;
	}

	// This is a jump to "late_import" function
	// To use late importing (only for debugging for now), uncomment this and the function late_import(), 
	// and comment out previous code
	/*
	#ifdef DEBUG		
		*call_to_resolve = MAKE_CALL(late_import);
	#endif	
	*/
}

void main_loop() 
{
    isSet[0] = 0;
	
    loadMenu();
	
    while(! isSet[0])
        sceKernelDelayThread(5000);
	
    start_eloader((char *)ebootPath, 1);
}

// Jumps to ELF's entry point
void runThread(SceSize args, void *argp)
{
	void (*start_entry)(SceSize, void*) = entry_point;
	// sceKernelFreePartitionMemory(*((SceUID*)ADDR_HBL_BLOCK_UID));
	// sceKernelFreePartitionMemory(*((SceUID*)ADDR_HBL_STUBS_BLOCK_UID));	
	start_entry(args, argp);
	sceKernelExitThread(0);
}

/* Resolves imports in ELF's program section already loaded in memory */
/* Uses game's imports to do the resolving (this can be further improved) */
/* Returns number of resolves */
unsigned int resolve_imports(tStubEntry* pstub_entry, unsigned int stubs_size)
{
	unsigned int i,j,nid_index;
	u32* cur_nid;
	u32* cur_call;
	u32 real_call;
	unsigned int resolving_count = 0;

	DEBUG_PRINT("RESOLVING IMPORTS", &stubs_size, sizeof(unsigned int));

	/* Browse ELF stub headers */
	for(i=0; i<stubs_size; i+=sizeof(tStubEntry))
	{
		//DEBUG_PRINT("POINTER TO STUB ENTRY:", &pstub_entry, sizeof(u32*));

		cur_nid = pstub_entry->nid_pointer;
		cur_call = pstub_entry->jump_pointer;

		/* For each stub header, browse all stubs */
		for(j=0; j<pstub_entry->stub_size; j++)
		{

			//DEBUG_PRINT("Current nid:", cur_nid, sizeof(u32*));
			//DEBUG_PRINT("Current call:", &cur_call, sizeof(u32*));

			/* Get syscall/jump instruction for current NID */
			nid_index = get_call_nidtable(*cur_nid, &real_call);

			//DEBUG_PRINT(" REAL CALL (TABLE) ", &real_call, sizeof(u32));
            
            switch (*cur_nid) 
			{
#ifdef FAKE_THREADS
                case 0x446D8DE6: //sceKernelCreateThread
                    real_call = MAKE_JUMP(_hook_sceKernelCreateThread);
                    break;
#endif

#ifdef RETURN_TO_MENU_ON_EXIT                
                case 0x05572A5F: //sceKernelExitGame
                    if (g_menu_enabled)
                        real_call = MAKE_JUMP(_hook_sceKernelExitGame);
                    break;
#endif

#ifdef FAKEMEM    
                case 0xA291F107:
                    DEBUG_PRINT("mem trick", NULL, 0);
                    real_call = MAKE_JUMP(_hook_sceKernelMaxFreeMemSize);
                    break;
                case 0x9D9A5BA1:
                    DEBUG_PRINT("mem trick", NULL, 0);
                    real_call = MAKE_JUMP(_hook_sceKernelGetBlockHeadAddr);
                    break;
                case 0x237DBD4F:
                    DEBUG_PRINT("mem trick", NULL, 0);
                    real_call = MAKE_JUMP(_hook_sceKernelAllocPartitionMemory);
                    break; 
#endif
					
/*
Work in progress, attempt for the mp3 library not to fail                  
                case 0x07EC321A:	//sceMp3ReserveMp3Handle
                case 0x0DB149F4:	//sceMp3NotifyAddStreamData
                case 0x2A368661:	//sceMp3ResetPlayPosition
                case 0x354D27EA:	//	sceMp3GetSumDecodedSample
                case 0x35750070:	//	sceMp3InitResource
                case 0x3C2FA058:	//	sceMp3TermResource
                case 0x3CEF484F:	//	sceMp3SetLoopNum
                case 0x44E07129:	//	sceMp3Init
                case 0x732B042A:	//	sceMp3EndEntry
                case 0x7F696782:	//	sceMp3GetMp3ChannelNum
                case 0x87677E40:	//	sceMp3GetBitRate
                case 0x87C263D1:	//	sceMp3GetMaxOutputSample
                case 0x8AB81558:	//	sceMp3StartEntry
                case 0x8F450998:	//	sceMp3GetSamplingRate
                case 0xA703FE0F:	//	sceMp3GetInfoToAddStreamData
                case 0xD021C0FB:	//	sceMp3Decode
                case 0xD0A56296:	//	sceMp3CheckStreamDataNeeded
                case 0xD8F54A51:	//	sceMp3GetLoopNum
                case 0xF5478233:	//	sceMp3ReleaseMp3Handle
                    real_call = MAKE_JUMP(_hook_genericSuccess);
                    break;
*/
                   
            }
            
			/* If NID not found in game imports */
			/* Syscall estimation if library available */
			if (real_call == 0)
			{
				/*
				#ifdef DEBUG
					dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
					if (dbglog >= 0)
					{
						sceIoWrite(dbglog, " LIBRARY NAME ", strlen(" LIBRARY NAME "));
						sceIoWrite(dbglog, pstub_entry->library_name, strlen(pstub_entry->library_name));					
						sceIoClose(dbglog);
					}
				#endif
				*/

				real_call = estimate_syscall(pstub_entry->library_name, *cur_nid);
				
				/* Commit changes to RAM */
				sceKernelDcacheWritebackInvalidateAll();

				/*
				#ifdef DEBUG	
					dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
					if (dbglog >= 0)
					{
						sceIoWrite(dbglog, " ESTIMATED SYSCALL VALUE ", strlen(" ESTIMATED SYSCALL VALUE "));
						sceIoWrite(dbglog, &real_call, sizeof(real_call));
						sceIoClose(dbglog);
					}
				#endif
				*/

			}

			/* If it's an instruction, resolve it */
			/* 0xC -> syscall 0 */
			/* Jumps are always > 0xC */		
			if(real_call > 0xC)
			{	
				/* Write it in ELF stubs memory */
				resolve_call(cur_call, real_call);
				resolving_count++;
			}

			sceKernelDcacheWritebackInvalidateAll();

			/*
			#ifdef DEBUG
				sceIoWrite(dbglog, &real_call, sizeof(u32));	
				sceIoWrite(dbglog, cur_call, sizeof(u32)*2);
			#endif
			*/

			cur_nid++;
			cur_call += 2;
		}
		
		pstub_entry++;
	}
	
	return resolving_count;	
}

// Relocates based on a MIPS relocation type
// Returns 0 on success, -1 on fail
int relocate_entry(tRelEntry reloc_entry)
{
	u32 buffer = 0, code = 0, offset = 0, offset_target, i;
	
	/*
	#ifdef DEBUG
		dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
		sceIoWrite(dbglog, " RELOCATING ", strlen(" RELOCATING "));
		sceIoWrite(dbglog, &reloc_entry, sizeof(tRelEntry));
		sceIoClose(dbglog);
	#endif
	*/

	// Actual offset
	offset_target = (u32)reloc_entry.r_offset + (u32)PRX_LOAD_ADDRESS;
	
	/*
	#ifdef DEBUG
		dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
		sceIoWrite(dbglog, &(offset_target), sizeof(u32));
		sceIoClose(dbglog);
	#endif
	*/

	// Load word to be relocated into buffer
    u32 misalign = offset_target%4;
    if (misalign) {
        u32 array[2];
        array[0] = *(u32*)(offset_target - misalign);
        array[1] = *(u32*)(offset_target + 4 - misalign);
        u8* array8 = (u8*)&array;
        u8* buffer8 = (u8*)&buffer;
        for (i = 3 + misalign; i >= misalign; --i){
            buffer8[i-misalign] = array8[i];
        }
    } else {
        buffer = *(u32*)offset_target;
    }

	/*	
	#ifdef DEBUG
		dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
		sceIoWrite(dbglog, &buffer, sizeof(u32));
		sceIoClose(dbglog);
	#endif
	*/
	
	//Relocate depending on reloc type
	switch(ELF32_R_TYPE(reloc_entry.r_info))
	{
		// Nothing
		case R_MIPS_NONE:
			return 0;
			
		// 32-bit address
		case R_MIPS_32:
			buffer += PRX_LOAD_ADDRESS;
			break;
		
		// Jump instruction
		case R_MIPS_26:    
			code = buffer & 0xfc000000;
			offset = (buffer & 0x03ffffff) << 2;
			offset += PRX_LOAD_ADDRESS;
			buffer = ((offset >> 2) & 0x03ffffff) | code;
			break;
		
		// Low 16 bits relocate as high 16 bits
		case R_MIPS_HI16:     
			offset = (buffer << 16) + PRX_LOAD_ADDRESS;
			offset = offset >> 16;
			buffer = (buffer & 0xffff0000) | offset;
			break;
		
		// Low 16 bits relocate as low 16 bits
		case R_MIPS_LO16:       
			offset = (buffer & 0x0000ffff) + PRX_LOAD_ADDRESS;
			buffer = (buffer & 0xffff0000) | (offset & 0x0000ffff);
			break;
		
		default:
			return -1;		
	}
	
	// Restore relocated word
    if (misalign) {
        u32 array[2];
        array[0] = *(u32*)(offset_target - misalign);
        array[1] = *(u32*)(offset_target + 4 - misalign);
        u8* array8 = (u8*)&array;
        u8* buffer8 = (u8*)&buffer;
        for (i = 3 + misalign; i >= misalign; --i){
             array8[i] = buffer8[i-misalign];
        }
        *(u32*)(offset_target - misalign) = array[0];
        *(u32*)(offset_target + 4 - misalign) = array[1];
    } else { 
        *(u32*)offset_target = buffer;
    }
	
	/*
	#ifdef DEBUG
		dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
		sceIoWrite(dbglog, offset_target, sizeof(u32));
		sceIoClose(dbglog);
	#endif
	*/

	return 0;
}

// Relocates PRX sections that need to
// Returns number of relocated entries
unsigned int relocate_sections(SceUID elf_file, u32 start_offset, Elf32_Ehdr *pelf_header)
{
	Elf32_Half i;
	Elf32_Shdr sec_header;
	Elf32_Off strtab_offset;
	Elf32_Off cur_offset;	
	char section_name[40];
	unsigned int j, section_name_size, entries_relocated = 0, num_entries;
	tRelEntry* reloc_entry;
	
	// Seek string table
	sceIoLseek(elf_file, start_offset + pelf_header->e_shoff + pelf_header->e_shstrndx * sizeof(Elf32_Shdr), PSP_SEEK_SET);
	sceIoRead(elf_file, &sec_header, sizeof(Elf32_Shdr));
	strtab_offset = sec_header.sh_offset;

	// First section header
	cur_offset = pelf_header->e_shoff;
	
	DEBUG_PRINT(" NUMBER OF SECTIONS ", &(pelf_header->e_shnum), sizeof(Elf32_Half));

	// Browse all section headers
	for(i=0; i<pelf_header->e_shnum; i++)
	{
		// Get section header
		sceIoLseek(elf_file, start_offset + cur_offset, PSP_SEEK_SET);
		sceIoRead(elf_file, &sec_header, sizeof(Elf32_Shdr));
		
		/*
		// Get section name
		section_name_size = elf_read_string(elf_file, strtab_offset + sec_header.sh_name, section_name);

		#ifdef DEBUG
			dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
			sceIoWrite(dbglog, " SECTION NAME ", strlen(" SECTION NAME "));
			sceIoWrite(dbglog, section_name, strlen(section_name));
			sceIoClose(dbglog);
		#endif
		*/

		if(sec_header.sh_type == LOPROC)
		{
			// DEBUG_PRINT(" RELOCATING SECTION ", NULL, 0);
			
			// Allocate memory for section
			num_entries = sec_header.sh_size / sizeof(tRelEntry);
			reloc_entry = malloc(sec_header.sh_size);
			if(!reloc_entry)
			{
				DEBUG_PRINT(" CANNOT ALLOCATE MEMORY FOR SECTION ", NULL, 0);
				continue;
			}
			
			// Read section
			sceIoLseek(elf_file, start_offset + sec_header.sh_offset, PSP_SEEK_SET);
			sceIoRead(elf_file, reloc_entry, sec_header.sh_size);

			for(j=0; j<num_entries; j++)
			{
				//DEBUG_PRINT(" RELOC_ENTRY ", &(reloc_entry[j]), sizeof(tRelEntry));	
                relocate_entry(reloc_entry[j]);
				entries_relocated++;
			}
			
			// Free section memory
			free(reloc_entry);
		}

		// Next section header
		cur_offset += sizeof(Elf32_Shdr);
	}
	
	// DEBUG_PRINT(" FINISHED RELOCATING  ",NULL, 0);

	// All relocation section processed
	return entries_relocated;
}

// Jumps to ELF's entry point
void execute_elf(SceSize args, void *argp)
{
	void (*start_elf)(SceSize, void*) = entry_point;	
	start_elf(args, argp);
}

// Allocates memory for homebrew so it doesn't overwrite itself
void allocate_memory(u32 size, void* addr)
{
#ifndef FAKEMEM
	SceUID mem;
	
	DEBUG_PRINT(" ALLOC EXECUTABLE MEMORY ", NULL, 0);
	mem = sceKernelAllocPartitionMemory(2, "ELFMemory", PSP_SMEM_Addr, size, addr);
	if(mem < 0)
		DEBUG_PRINT(" allocate_memory FAILED ", &mem, sizeof(mem));
#endif
}

// HBL entry point
// Needs path to ELF or EBOOT
void start_eloader(char *eboot_path, int is_eboot)
{
	unsigned int num_nids, stubs_size, stubs_resolved;
	unsigned int sections_relocated;
	tStubEntry* pstub_entry;
	SceUID elf_file, thid;
	Elf32_Ehdr elf_header;
	u32 offset = 0;

	//DEBUG_PRINT("EBOOT:", eboot_path, strlen(eboot_path));

	// Extracts ELF from PBP
	if (is_eboot)		
		elf_file = elf_eboot_extract_open(eboot_path, &offset);
	// Plain ELF
	else
		elf_file = sceIoOpen(eboot_path, PSP_O_RDONLY, 0777);
	
	// Read ELF header
	sceIoRead(elf_file, &elf_header, sizeof(Elf32_Ehdr));
	
	DEBUG_PRINT(" ELF TYPE ", &(elf_header.e_type), sizeof(Elf32_Half));
    
    gp = getGP(elf_file, offset, &elf_header); 
    //DEBUG_PRINT(" GP IS: ", &gp, sizeof(u32));           

	// Static ELF
	if(elf_header.e_type == (Elf32_Half) ELF_STATIC)
	{	
		DEBUG_PRINT(" STATIC ELF ", NULL, 0);		

		// Load ELF program section into memory
		hbsize = elf_load_program(elf_file, offset, &elf_header, allocate_memory);		
	
		// Locate ELF's .lib.stubs section
		stubs_size = elf_find_imports(elf_file, offset, &elf_header, &pstub_entry);
	}
	
	// Relocatable ELF (PRX)
	else if(elf_header.e_type == (Elf32_Half) ELF_RELOC)
	{	
		DEBUG_PRINT(" PRX ELF ", NULL, 0);
   
		// Load program section into memory and also get stub headers
		stubs_size = prx_load_program(elf_file, offset, &elf_header, &pstub_entry, &hbsize, allocate_memory);
		
		/*
		#ifdef DEBUG
			dbglog = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
			sceIoWrite(dbglog, " STUBS SIZE ", strlen(" STUBS SIZE "));
			sceIoWrite(dbglog, &stubs_size, sizeof(int));
			sceIoClose(dbglog);
		#endif
		*/

		//Relocate all sections that need to
		sections_relocated = relocate_sections(elf_file, offset, &elf_header);

		// Relocate ELF entry point and GP register
		elf_header.e_entry = (u32)elf_header.e_entry + (u32)PRX_LOAD_ADDRESS;
        gp += (u32)PRX_LOAD_ADDRESS;
	}
	
	// Unknown ELF type
	else
	{
		exit_with_log(" UNKNOWN ELF TYPE ", NULL, 0);
	}
	
	// Resolve ELF's stubs with game's stubs and syscall estimation */
	stubs_resolved = resolve_imports(pstub_entry, stubs_size);
   
	// No need for ELF file anymore
	sceIoClose(elf_file);	
	
	// DEBUG_PRINT(" PROGRAM SECTION START ", &(elf_header.e_entry), sizeof(Elf32_Addr));		
	DEBUG_PRINT(" EXECUTING ELF ", NULL, 0);    
    
    // Commit changes to RAM
	sceKernelDcacheWritebackInvalidateAll();
	
	// Create and start hb thread
    if (gp)
        SET_GP(gp);
	entry_point = (u32 *)elf_header.e_entry;

	thid = sceKernelCreateThread("homebrew", runThread, 0x18, 0x10000, 0, NULL);

	if(thid >= 0)
	{
		thid = sceKernelStartThread(thid, strlen(eboot_path) + 1, (void *)eboot_path);
		if (thid < 0)
		{
			LOGSTR1(" HB Thread couldn't start. Error 0x%08lX\n", thid);
			sceKernelExitGame();
		}
    } 
	else 
	{
        LOGSTR1(" HB Thread couldn't be created. Error 0x%08lX\n", thid);
		sceKernelExitGame();
	}

	// Uncomment only if no homebrew thread created
	// execute_elf(strlen(eboot_path) + 1, (void *)eboot_path);

	return;
}

/* Loads a Basic menu as a thread
* In the future, we might want the menu to be an actual homebrew
*/
void loadMenu()
{
    // Just trying the basic functions used by the menu
    SceUID id = -1;
    int attempts = 0;
	SceUID menu_file;
    SceOff file_size;
	int bytes_read;
	SceIoDirent entry;
	SceUID menuThread;
		
    print_to_screen("Loading Menu");

// this crashes too often :(
/*	
    while ((id < 0) && (attempts < MAX_REESTIMATE_ATTEMPTS))
	{
        attempts++;
        id = sceIoDopen("ms0:");
        if (id <= 0)
		{

            DEBUG_PRINT(" sceIoDopen syscall estimation failed, attempt to reestimate ",NULL, 0);
            reestimate_syscall(0xB29DDF9C, attempts); //sceIoDopen TODO move to config ?
        }
    }
	
    if (id < 0) 
	{
        print_to_screen("Loading Menu Failed (syscall ?)");
        exit_with_log(" FATAL, sceIoDopen syscall estimation failed ",NULL, 0);
    }
	
	else 
	{
        attempts = 0;        
        memset(&entry, 0, sizeof(SceIoDirent)); 
        while (sceIoDread(id, &entry) <= 0 && attempts < 10) 
		{       
            attempts++;
            DEBUG_PRINT(" sceIoDread syscall estimation failed, attempt to reestimate ",NULL, 0);
            reestimate_syscall(0xE3EB004C, attempts); //sceIoDread TODO move to config ?
            memset(&entry, 0, sizeof(SceIoDirent));
        }
    }
	
    sceIoDclose(id);
*/
	//DEBUG_PRINT(" LOADER RUNNING ", NULL, 0);	

	if ((menu_file = sceIoOpen(MENU_PATH, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" FAILED TO LOAD MENU ", &menu_file, sizeof(menu_file));

	// Get MENU size
	file_size = sceIoLseek(menu_file, 0, PSP_SEEK_END);
	sceIoLseek(menu_file, 0, PSP_SEEK_SET);    
    
	// Load MENU to buffer
	if ((bytes_read = sceIoRead(menu_file, (void*)menu_pointer, file_size)) < 0)
		exit_with_log(" ERROR READING MENU ", &bytes_read, sizeof(bytes_read));
        
    void (*start_entry)(SceSize, void*) = menu_pointer;	 
	menuThread = sceKernelCreateThread("menu", start_entry, 0x18, 0x10000, 0, NULL);

	if(menuThread >= 0)
	{
		menuThread = sceKernelStartThread(menuThread, 0, NULL);
    } 

	else 
	{
        exit_with_log(" Menu Launch failed ", NULL, 0);
    }        
}

// HBL main thread
int start_thread(SceSize args, void *argp)
{
	int num_nids;
	
	// Build NID table
    print_to_screen("Build Nids table");
	num_nids = build_nid_table(nid_table);
    LOGSTR1("NUM NIDS: %d\n", num_nids);
	
	if(num_nids > 0)
	{	
		// FIRST THING TO DO!!!
        print_to_screen("Resolving Missing Stubs");
		resolve_missing_stubs();
	
		// Free memory
        print_to_screen("Free memory");
		free_game_memory();
		
        print_to_screen("-- Done (Free memory)");
        LOGSTR0("START HBL\n");

        // Start the menu or run directly the hardcoded eboot      
        if (file_exists(EBOOT_PATH))
            start_eloader(EBOOT_PATH, 1);
        else if (file_exists(ELF_PATH))
            start_eloader(ELF_PATH, 0);
        else 
		{
            g_menu_enabled = 1;
            main_loop();
        }
	}
	
	// Loop forever
    print_to_screen("Looping HBL Thread");

	while(1)
		sceKernelDelayThread(100000);
	
	return 0;
}

// Entry point
void _start(unsigned long, unsigned long *) __attribute__ ((section (".text.start")));
void _start(unsigned long arglen, unsigned long *argp)
{	
	SceUID thid;
    void *fb = (void *)0x444000000;
    int firmware_version = getFirmwareVersion();
	
    sceDisplaySetFrameBuf(fb, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 1);
    SetColor(0);
    print_to_screen("Starting HBL -- http://code.google.com/p/valentine-hbl");
    
	switch (firmware_version) {
    case 0:
    case 1:
        print_to_screen("Unknown Firmware :(");
        break; 
    default:
        PRTSTR2("Firmware %d.%dx detected", firmware_version / 100,  (firmware_version % 100) / 10);
        break;
    }
    
	// Create and start eloader thread
	thid = sceKernelCreateThread("HBL", start_thread, 0x18, 0x10000, 0, NULL);
	
	if(thid >= 0)
	{
		thid = sceKernelStartThread(thid, 0, NULL);
	}
	
	sceKernelExitDeleteThread(0);

	// Never executed (hopefully)
	return 0;
}

// Big thanks to people who share information !!!
