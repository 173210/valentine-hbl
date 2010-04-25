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

u32 find_first_free_syscall (int lib_index, u32 start_syscall) {
	int index = -1;
    u32 syscall = start_syscall;
    int boundary_low = 0, boundary_high = 0;
    int ret = get_syscall_boundaries(lib_index, &boundary_low,  &boundary_high);
    if (!ret)
    {
        LOGSTR0("--ERROR GETTING SYSCALL BOUNDARIES\n");
        return 0;
    }
	while ((index = get_call_index(MAKE_SYSCALL(syscall))) >= 0)
	{
		LOGSTR2("--ESTIMATED SYSCALL 0x%08lX ALREADY EXISTS AT INDEX %d\n", syscall, index);
		syscall--;
        if (!check_syscall_boundaries (syscall, boundary_low, boundary_high))
        {
            syscall = boundary_high - 1;
            //Risk of infinite loop here ?
        }
	}

    return syscall;
}
/*
 * Checks if a syscall looks normal compared to other libraries boundaries.
 * returns 1 if ok, 0 if not
*/
int check_syscall_boundaries (u32 syscall, int boundary_low, int boundary_high) {
    if (syscall <= boundary_low) 
    {
        LOGSTR2("--ERROR: SYSCALL OUT OF LIB'S RANGE, should be higher than 0x%08lX, but we got 0x%08lX\n", boundary_low, syscall);
        return 0;
    }
    if (syscall >= boundary_high)
    {
        LOGSTR2("--ERROR: SYSCALL OUT OF LIB'S RANGE, should be lower than 0x%08lX, but we got 0x%08lX\n", boundary_high, syscall);             
        return 0;
    }

    return 1;
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
	int estimated_syscall;
	u32 firmware_v;

	LOGSTR2("=> ESTIMATING %s : 0x%08lX\n", lib, nid);
	
	// Finding the library on table
	lib_index = get_library_index(lib);

	if (lib_index < 0)
	{
		LOGSTR1("--ERROR: LIBRARY NOT FOUND ON TABLE  %s\n", lib);
        return 0;
    }

	LOGLIB(library_table[lib_index]);
		
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
	
	sceIoClose(nid_file);

	LOGSTR2("--NID index in %s: %d\n", file_path, file_index);

	if (file_index < 0)
	{
		write_debug(" ERROR: NID NOT FOUND ON .NIDS FILE ", file_path, strlen(file_path));
		exit_with_log(" ", &nid, sizeof(nid));
	}

	int aux0 = (int)library_table[lib_index].lowest_syscall + file_index;

	//LOGSTR1("Pre: 0x%08lX\n", aux0);
	
	if (file_index > library_table[lib_index].lowest_index)
	{
		estimated_syscall = aux0 - (int)library_table[lib_index].lowest_index;
	}
	else
	{
		/*
		LOGSTR1("--aux0: %d\n", aux0);
		LOGSTR1("--library_table[lib_index].num_library_exports: %d\n", library_table[lib_index].num_library_exports);
		LOGSTR1("--library_table[lib_index].lowest_index: %d\n", library_table[lib_index].lowest_index);
		*/
		estimated_syscall = aux0 + (int)library_table[lib_index].num_library_exports - (int)library_table[lib_index].lowest_index;
	}

	LOGSTR1("--FIRST ESTIMATED SYSCALL: 0x%08lX\n", estimated_syscall);

	// Check if estimated syscall already exists (should be very rare)
    estimated_syscall = find_first_free_syscall(lib_index, estimated_syscall);

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	LOGSTR1("--FINAL ESTIMATED SYSCALL: 0x%08lX\n", estimated_syscall);
    
	return MAKE_SYSCALL(estimated_syscall);
}

// m0skit0's attempt
// Needs to be more independent from sdk_hbl.S
u32 reestimate_syscall(const char * lib, u32* stub) 
{
#ifdef REESTIMATE_SYSCALL  

	u32 syscall;
    int lib_index;
    
	// Finding the library on table
	lib_index = get_library_index(lib);

	if (lib_index < 0)
	{
		LOGSTR1("--ERROR: LIBRARY NOT FOUND ON TABLE  %s\n", lib);
        return 0;
    }

    LOGSTR1("=Reestimating syscall for stub 0x%08lX\n", stub); 
	stub++;
	syscall = GET_SYSCALL_NUMBER(*stub);
    LOGSTR1("--0x%08lX -->", syscall); 
	syscall--;
    syscall = find_first_free_syscall(lib_index, syscall);
    LOGSTR1(" 0x%08lX\n", syscall); 
	*stub = MAKE_SYSCALL(syscall);
    LOGSTR0("--Done.\n");
	sceKernelDcacheWritebackInvalidateAll();
	return syscall;
#else
    return 0;
#endif	    
}
