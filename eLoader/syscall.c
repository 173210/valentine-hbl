#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "utils.h"
#include "tables.h"

// Searches for NID in a NIDS file and returns the index
int find_nid_in_file(SceUID nid_file, u32 nid)
{
	int i = 0;
	u32 cur_nid;
	
	while(sceIoRead(nid_file, &cur_nid, sizeof(cur_nid)) > 0)
		if (cur_nid == nid)
			return i;
		else
			i++;

	return -1;
}

// Estimate a syscall
// Pass reference NID and distance from desidered function in the export table
// Return syscall instruction
// Syscall accuracy (%) = (exports known from library / total exports from library) x 100
// m0skit0's implementation
u32 estimate_syscall(const char *lib, u32 nid)
{
	char file_path[MAX_LIBRARY_NAME_LENGTH + 100];
	SceUID nid_file;
	int lib_index, file_index, lowest_index;
	u32 estimated_syscall;
	tSceLibrary *plibrary_entry;
	u32 firmware_v;

	LOGSTR2("=> ESTIMATING %s : 0x%08lX\n", lib, nid);
	
	// Finding the library on table
	plibrary_entry = get_library_entry(lib);

	if (plibrary_entry == NULL)
	{
		LOGSTR1("--ERROR: LIBRARY NOT FOUND ON TABLE  %s\n", lib);
        return 0;
    }

	LOGSTR2("--LOWEST NID/SYSCALL ON LIBRARY:  0x%08lX//0x%08lX\n", plibrary_entry->lowest_nid, plibrary_entry->lowest_syscall);
		
	// Constructing the file path
	strcpy(file_path, LIB_PATH);
    firmware_v = getFirmwareVersion();
	
    if (firmware_v >= 500 && firmware_v < 600)
        strcat(file_path, "_5xx/");
    else if (firmware_v >= 600)
        strcat(file_path, "_6xx/");

	strcat(file_path, lib);
	strcat(file_path, LIB_EXTENSION);
    
    // Rollback to default name in case of error
    if (!file_exists(file_path))
    {
        strcpy(file_path, LIB_PATH);
        strcat(file_path, "/");
        strcat(file_path, lib);
        strcat(file_path, LIB_EXTENSION);
    }    

	if ((nid_file = sceIoOpen(file_path, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" ERROR: CANNOT OPEN .NIDS FILE ", file_path, strlen(file_path));
	
	// Get NIDs index in file
	file_index = find_nid_in_file(nid_file, nid);
	lowest_index = find_nid_in_file(nid_file, plibrary_entry->lowest_nid);
	
	sceIoClose(nid_file);

	if (file_index < 0)
	{
		write_debug(" ERROR: NID NOT FOUND ON .NIDS FILE ", file_path, strlen(file_path));
		exit_with_log(" ", &nid, sizeof(nid));
	}

	// Set estimated syscall to lowest syscall in library
	estimated_syscall = plibrary_entry->lowest_syscall;
	
	if (file_index > lowest_index)
		estimated_syscall += (unsigned int) file_index - lowest_index;
	else
		estimated_syscall += (plibrary_entry->num_library_exports - lowest_index) + (unsigned int)file_index;

	// Check if estimated syscall already exists
	int index = -1;
	while ((index = get_call_index(MAKE_SYSCALL(estimated_syscall))) >= 0)
	{
		LOGSTR2("--ESTIMATED SYSCALL 0x%08lX ALREADY EXISTS AT INDEX %d\n", estimated_syscall, index);
		estimated_syscall--;
	}

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	LOGSTR1("--FINAL ESTIMATED SYSCALL: 0x%08lX\n", estimated_syscall);
	
	return MAKE_SYSCALL(estimated_syscall);
}

// m0skit0's attempt
// Needs to be more independent from sdk_hbl.S
u32 reestimate_syscall(u32* stub) 
{
#ifdef REESTIMATE_SYSCALL  
    LOGSTR1("=Reestimating syscall for stub 0x%08lX\n", stub); 
	u32 syscall;

	stub++;
	syscall = GET_SYSCALL_NUMBER(*stub);
    LOGSTR1("--0x%08lX -->", syscall); 
	syscall--;
    LOGSTR1(" 0x%08lX\n", syscall); 
	*stub = MAKE_SYSCALL(syscall);
    LOGSTR0("--Done.\n");
	sceKernelDcacheWritebackInvalidateAll();
	return syscall;
#else
    return 0;
#endif	    
}
