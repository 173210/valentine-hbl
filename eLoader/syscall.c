#include "eloader.h"
#include "debug.h"
#include "utils.h"

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
	int lib_index, file_index;
	u32 estimated_syscall;
	tSceLibrary *plibrary_entry;

	DEBUG_PRINT(" ESTIMATING ", lib, strlen(lib));
	DEBUG_PRINT(" ", &nid, sizeof(nid));
	
	// Finding the library on table
	plibrary_entry = get_library_entry(lib);

	if (plibrary_entry == NULL)
	{
		DEBUG_PRINT(" ERROR: LIBRARY NOT FOUND ON TABLE ", lib, strlen(lib));
        return 0;
    }

	DEBUG_PRINT(" LOWEST SYSCALL ON LIBRARY ", &(plibrary_entry->lowest_syscall), sizeof(u32));
		
	// Constructing the file path
	strcpy(file_path, LIB_PATH);
    u32 firmware_v = getFirmwareVersion();
    if (firmware_v >= 500 && firmware_v < 600)
        strcat(file_path, "_5xx/");
    else if (firmware_v >= 600)
        strcat(file_path, "_6xx/");

	strcat(file_path, lib);
	strcat(file_path, LIB_EXTENSION);
    
    // rollback to default name in case of error
    if (!file_exists(file_path))
    {
        strcpy(file_path, LIB_PATH);
        strcat(file_path, "/");
        strcat(file_path, lib);
        strcat(file_path, LIB_EXTENSION);
    }    

	if ((nid_file = sceIoOpen(file_path, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" ERROR: CANNOT OPEN .NIDS FILE ", file_path, strlen(file_path));
	
	// Get NID index in file
	file_index = find_nid_in_file(nid_file, nid);	
	
	sceIoClose(nid_file);

	if (file_index < 0)
	{
		write_debug(" ERROR: NID NOT FOUND ON .NIDS FILE ", file_path, strlen(file_path));
		exit_with_log(" ", &nid, sizeof(nid));
	}

	// Set estimated syscall to lowest syscall in library
	estimated_syscall = plibrary_entry->lowest_syscall;
	
	if (file_index > plibrary_entry->lowest_index)
		estimated_syscall += (unsigned int) file_index - plibrary_entry->lowest_index;
	else
		estimated_syscall += (plibrary_entry->num_library_exports - plibrary_entry->lowest_index) + (unsigned int) (file_index);

	DEBUG_PRINT(" ESTIMATED ", &estimated_syscall, sizeof(estimated_syscall));
	
	return MAKE_SYSCALL(estimated_syscall);
}

// m0skit0's attempt
// Needs to be more independent from sdk_hbl.S
u32 reestimate_syscall(u32* stub) 
{
	u32 syscall;

	stub++;
	syscall = GET_SYSCALL_NUMBER(*stub);
	syscall--;
	*stub = MAKE_SYSCALL(syscall);
	
    return syscall;
}
