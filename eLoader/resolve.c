#include "sdk.h"
#include "debug.h"
#include "eloader.h"
#include "scratchpad.h"
#include "elf.h"
#include "tables.h"
#include "hook.h"
#include "modmgr.h"

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

			// NID & library for i-th import
			ret = get_lib_nid(i, lib_name, &nid);

			LOGSTR0(lib_name);
			LOGSTR1(" 0x%08lX\n", nid);

			// Is it known by HBL?
			ret = get_nid_index(nid);

			// If it's known, get the call
			if (ret > 0)
			{
				LOGSTR0("-Found in NID table, using real call\n");
				syscall = nid_table[ret].call;
			}
			
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
	// SYSCALL
	if(!(call_resolved & SYSCALL_MASK_RESOLVE))
	{
		*call_to_resolve = JR_RA_OPCODE;
		*(++call_to_resolve) = call_resolved;
	}
	
	// JUMP
	else
	{
		*call_to_resolve = call_resolved;
		*(++call_to_resolve) = NOP_OPCODE;
	}
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

#ifdef HOOK_CHDIR_AND_FRIENDS    
    int chdir_ok = test_sceIoChdir();
#endif

	LOGSTR1("RESOLVING IMPORTS. Stubs size: %d\n", stubs_size);

	/* Browse ELF stub headers */
	for(i=0; i<stubs_size; i+=sizeof(tStubEntry))
	{
		LOGSTR1("Pointer to stub entry: 0x%08lX\n", pstub_entry);	

		cur_nid = pstub_entry->nid_pointer;
		cur_call = pstub_entry->jump_pointer;

		/* For each stub header, browse all stubs */
		for(j=0; j<pstub_entry->stub_size; j++)
		{

			LOGSTR1("Current nid: 0x%08lX\n", *cur_nid);
			LOGSTR1("Current call: 0x%08lX\n", cur_call);

			/* Get syscall/jump instruction for current NID */
			nid_index = get_call_nidtable(*cur_nid, &real_call);

			LOGSTR1("Index for NID on table: %d\n", nid_index);
            
			// HOOOOOOK THAT!!!
            switch (*cur_nid) 
			{

//utility functions, we need those
                case 0x237DBD4F: // sceKernelAllocPartitionMemory
                    LOGSTR0(" mem trick ");
                    real_call = MAKE_JUMP(_hook_sceKernelAllocPartitionMemory);
                    break;                     
    
#ifdef FAKE_THREADS
                case 0x446D8DE6: //sceKernelCreateThread
                    real_call = MAKE_JUMP(_hook_sceKernelCreateThread);
                    break;
#endif

#ifdef RETURN_TO_MENU_ON_EXIT                
                case 0x05572A5F: // sceKernelExitGame
                    if (g_menu_enabled)
                        real_call = MAKE_JUMP(_hook_sceKernelExitGame);
                    break;
#endif 

#ifdef LOAD_MODULE
				case 0x977DE386: // sceKernelLoadModule
					LOGSTR0(" loadmodule trick ");
					real_call = MAKE_JUMP(_hook_sceKernelLoadModule);
					break;
				
				case 0x50F0C1EC: // sceKernelStartModule
					LOGSTR0(" loadmodule trick ");
					real_call = MAKE_JUMP(_hook_sceKernelStartModule);
					break;
#endif  

//overrides to avoid syscall estimates, those are not necessary but reduce estimate failures and improve compatibility for now
                case 0xA291F107: // sceKernelMaxFreeMemSize (avoid syscall estimation)
                    LOGSTR0(" mem trick ");
                    real_call = MAKE_JUMP(sceKernelMaxFreeMemSize);
                    break;
                case 0xC41C2853: //	sceRtcGetTickResolution (avoid syscall estimation)
                    real_call = MAKE_JUMP(_hook_sceRtcGetTickResolution);
                    break;  
                case 0x68963324: //	sceIoLseek32 (avoid syscall estimation)
                    real_call = MAKE_JUMP(_hook_sceIoLseek32);
                    break;                    
                case 0x3A622550: //	sceCtrlPeekBufferPositive (avoid syscall estimation)
                    real_call = MAKE_JUMP(_hook_sceCtrlPeekBufferPositive);
                    break;   
                case 0x3F7AD767: //	sceRtcGetCurrentTick (avoid syscall estimation)
                    real_call = MAKE_JUMP(_hook_sceRtcGetCurrentTick);
                    break;   
                    
#ifdef HOOK_AUDIOFUNCTIONS                    
                case 0x38553111: //sceAudioSRCChReserve(avoid syscall estimation)
                    real_call = MAKE_JUMP(_hook_sceAudioSRCChReserve);
                    break;  
                case 0x5C37C0AE: //	sceAudioSRCChRelease (avoid syscall estimation)
                    real_call = MAKE_JUMP(_hook_sceAudioSRCChRelease);
                    break;  
                case 0xE0727056: // sceAudioSRCOutputBlocking (avoid syscall estimation)
                    real_call = MAKE_JUMP(_hook_sceAudioSRCOutputBlocking);
                    break; 
#endif   

#ifdef HOOK_CHDIR_AND_FRIENDS    
                case 0x55F4717D: //	sceIoChdir (only if it failed)
                    if (chdir_ok)
                        break;
                    LOGSTR0(" Chdir trick sceIoChdir\n");
                    real_call = MAKE_JUMP(_hook_sceIoChdir);
                    break; 
                case 0x109F50BC: //	sceIoOpen (only ifs sceIoChdir failed)
                    if (chdir_ok)
                        break;
                    LOGSTR0(" Chdir trick sceIoOpen\n");                        
                    real_call = MAKE_JUMP(_hook_sceIoOpen);
                    break;  
                case 0xB29DDF9C: //	sceIoDopen (only if sceIoChdir failed)
                    if (chdir_ok)
                        break;
                    LOGSTR0(" Chdir trick sceIoDopen\n");                        
                    real_call = MAKE_JUMP(_hook_sceIoDopen);
                    break;                      
#endif		
            }

			LOGSTR1("Real call before estimation: 0x%08lX\n", real_call);
            
			/* If NID not found in game imports */
			/* Syscall estimation if library available */
			if (real_call == 0)
			{
				real_call = estimate_syscall(pstub_entry->library_name, *cur_nid);
			}

			LOGSTR1("Real call after estimation: 0x%08lX\n", real_call);

			/* If it's an instruction, resolve it */
			/* 0xC -> syscall 0 */
			/* Jumps are always > 0xC */		
			if(real_call > 0xC)
			{	
				/* Write it in ELF stubs memory */
				resolve_call(cur_call, real_call);
				resolving_count++;
			}

			LOGSTR3("Resolved stub 0x%08lX: 0x%08lX 0x%08lX\n", cur_call, *cur_call, *(cur_call+1))

			sceKernelDcacheWritebackInvalidateAll();

			cur_nid++;
			cur_call += 2;
		}
		
		pstub_entry++;
	}
	
    LOGSTR0("RESOLVING IMPORTS: Done.");
	return resolving_count;	
}
