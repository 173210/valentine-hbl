#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "utils.h"
#include "tables.h"
#include "syscall.h"

// Searches for NID in a NIDS file and returns the index
int find_nid_in_file(SceUID nid_file, u32 nid)
{
	int i = 0;
	u32 cur_nid;

	sceIoLseek(nid_file, 0, PSP_SEEK_SET);
	while(sceIoRead(nid_file, &cur_nid, sizeof(cur_nid)) > 0)
		if (cur_nid == nid)
			return i;
		else
			i++;

	return -1;
}

// Opens .nids file for a given library
SceUID open_nids_file(const char* lib)
{
	char file_path[MAX_LIBRARY_NAME_LENGTH + 100];
	
	// Constructing the file path
	strcpy(file_path, LIB_PATH);
    int firmware_v = getFirmwareVersion();
	
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

	LOGSTR1("Opening %s\n", file_path);

	return sceIoOpen(file_path, PSP_O_RDONLY, 0777);
}

u32 find_first_free_syscall (int lib_index, u32 start_syscall) 
{
	int index = -1;
    u32 syscall = start_syscall;
    int boundary_low = 0, boundary_high = 0;
	
    int ret = get_syscall_boundaries(lib_index, &boundary_low,  &boundary_high);
	
    if (!ret)
    {
        LOGSTR0("--ERROR GETTING SYSCALL BOUNDARIES\n");
        return 0;
    }

	int i = 0;
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
int check_syscall_boundaries (u32 syscall, int boundary_low, int boundary_high) 
{
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

// Estimate a syscall from library's closest lower known syscall
u32 estimate_syscall_higher(int lib_index, u32 nid, SceUID nid_file)
{
	LOGSTR0("=> FROM HIGHER\n");
		
	// Get NIDs index in file
	int nid_index = find_nid_in_file(nid_file, nid);

	if (nid_index < 0)
	{
		LOGSTR0("->ERROR: NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}

	LOGSTR1("NID index in file: %d\n", nid_index);

	u32 higher_nid_index = get_higher_known_nid(lib_index, nid);

	if (higher_nid_index < 0)
	{
		LOGSTR0("NID is highest in library, switching to lower method\n");
		return estimate_syscall_lower(lib_index, nid, nid_file);  // Infinite call risk here!!
	}

	LOGSTR2("Higher known NID/SYSCALL: 0x%08lX/0x%08lX\n", nid_table->table[higher_nid_index].nid, GET_SYSCALL_NUMBER(nid_table->table[higher_nid_index].call));
	
	int higher_index = find_nid_in_file(nid_file, nid_table->table[higher_nid_index].nid);

	if (higher_index < 0)
	{
		LOGSTR0("->ERROR: LOWER NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}	

	LOGSTR1("Lower known NID index: %d\n", higher_index);
	
	sceIoClose(nid_file);

	u32 estimated_syscall = GET_SYSCALL_NUMBER(nid_table->table[higher_nid_index].call) - (higher_index - nid_index);

	LOGSTR1("--FIRST ESTIMATED SYSCALL: 0x%08lX\n", estimated_syscall);

	// Check if estimated syscall already exists (should be very rare)
    estimated_syscall = find_first_free_syscall(lib_index, estimated_syscall);

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	LOGSTR1("--FINAL ESTIMATED SYSCALL: 0x%08lX\n", estimated_syscall);
    
	return estimated_syscall;
}

// Estimate a syscall from library's closest lower known syscall
u32 estimate_syscall_lower(int lib_index, u32 nid, SceUID nid_file)
{
	LOGSTR0("=> FROM LOWER\n");
		
	// Get NIDs index in file
	int nid_index = find_nid_in_file(nid_file, nid);

	if (nid_index < 0)
	{
		LOGSTR0("->ERROR: NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}

	LOGSTR1("NID index in file: %d\n", nid_index);

	u32 lower_nid_index = get_lower_known_nid(lib_index, nid);

	if (lower_nid_index < 0)
	{
		LOGSTR0("NID is lowest in library, switching to higher method\n");
		return estimate_syscall_higher(lib_index, nid, nid_file);  // Infinite call risk here!!
	}

	LOGSTR2("Lower known NID/SYSCALL: 0x%08lX/0x%08lX\n", nid_table->table[lower_nid_index].nid, GET_SYSCALL_NUMBER(nid_table->table[lower_nid_index].call));
	
	int lower_index = find_nid_in_file(nid_file, nid_table->table[lower_nid_index].nid);

	if (lower_index < 0)
	{
		LOGSTR0("->ERROR: LOWER NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}	

	LOGSTR1("Lower known NID index: %d\n", lower_index);
	
	sceIoClose(nid_file);

	u32 estimated_syscall = GET_SYSCALL_NUMBER(nid_table->table[lower_nid_index].call) + (nid_index - lower_index);

	LOGSTR1("--FIRST ESTIMATED SYSCALL: 0x%08lX\n", estimated_syscall);

	// Check if estimated syscall already exists (should be very rare)
    estimated_syscall = find_first_free_syscall(lib_index, estimated_syscall);

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	LOGSTR1("--FINAL ESTIMATED SYSCALL: 0x%08lX\n", estimated_syscall);
    
	return estimated_syscall;
}

// Estimate a syscall from library's lowest known syscall
u32 estimate_syscall_lowest(int lib_index, u32 nid, SceUID nid_file)
{
	LOGSTR0("=> FROM LOWEST\n");
		
	// Get NIDs index in file
	int nid_index = find_nid_in_file(nid_file, nid);
	
	sceIoClose(nid_file);

	LOGSTR1("--NID index in file: %d\n", nid_index);

	if (nid_index < 0)
	{
		LOGSTR0("->ERROR: NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}
	
	int estimated_syscall;	
	if (nid_index > library_table->table[lib_index].lowest_index)
	{
		estimated_syscall = (int)library_table->table[lib_index].lowest_syscall + nid_index - (int)library_table->table[lib_index].lowest_index;
	}
	else
	{
		estimated_syscall = (int)library_table->table[lib_index].lowest_syscall + nid_index + (int)library_table->table[lib_index].num_library_exports - (int)library_table->table[lib_index].lowest_index;
	}

	LOGSTR1("--FIRST ESTIMATED SYSCALL: 0x%08lX\n", estimated_syscall);

	// Check if estimated syscall already exists (should be very rare)
    estimated_syscall = find_first_free_syscall(lib_index, estimated_syscall);

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	LOGSTR1("--FINAL ESTIMATED SYSCALL: 0x%08lX\n", estimated_syscall);
    
	return estimated_syscall;
}

// Estimate a syscall
// Pass reference NID and distance from desidered function in the export table
// Return syscall instruction
// Syscall accuracy (%) = (exports known from library / total exports from library) x 100
// m0skit0's implementation
u32 estimate_syscall(const char *lib, u32 nid, HBLEstimateMethod method)
{
	LOGSTR2("=> ESTIMATING %s : 0x%08lX\n", lib, nid);
	
	// Finding the library on table
	int lib_index = get_library_index(lib);

	if (lib_index < 0)
	{
		LOGSTR1("->ERROR: LIBRARY NOT FOUND ON TABLE  %s\n", lib);
        return 0;
    }

	LOGLIB(library_table->table[lib_index]);

	SceUID nid_file = open_nids_file(lib);

	if (nid_file < 0)
	{
		LOGSTR1("->ERROR: couldn't open NIDS file for %s\n", lib);
		return 0;
	}

	u32 estimated_syscall;
	switch (method)
	{
		case FROM_LOWEST:
			estimated_syscall = estimate_syscall_lowest(lib_index, nid, nid_file);
			break;

		case FROM_LOWER:
			estimated_syscall = estimate_syscall_lower(lib_index, nid, nid_file);
			break;

		case FROM_HIGHER:
			estimated_syscall = estimate_syscall_higher(lib_index, nid, nid_file);
			break;

		default:
			LOGSTR1("Unknown estimation method %d\n", method);
			estimated_syscall = 0;
			break;
	}			

	add_nid_to_table(nid, MAKE_SYSCALL(estimated_syscall), lib_index);

	LOGSTR0("Estimation done\n");

	return MAKE_SYSCALL(estimated_syscall);
}

// m0skit0's attempt
// Needs to be more independent from sdk_hbl.S
u32 reestimate_syscall(const char * lib, u32 nid, u32* stub, HBLEstimateMethod type) 
{
#ifdef REESTIMATE_SYSCALL
	LOGSTR2("=Reestimating function 0x%08lX for stub 0x%08lX: ", nid, stub);
    
	// Finding the library on table
	int lib_index = get_library_index(lib);

	if (lib_index < 0)
	{
		LOGSTR1("--ERROR: LIBRARY NOT FOUND ON TABLE  %s\n", lib);
        return 0;
    }

	SceUID nid_file = open_nids_file(lib);

	if (nid_file < 0)
	{
		LOGSTR1("->ERROR: couldn't open NIDS file for %s\n", lib);
		return 0;
	}

	stub++;
	u32 syscall = GET_SYSCALL_NUMBER(*stub);
    LOGSTR1("0x%08lX -->", syscall); 
	
	switch (type)
	{
		case SUBSTRACT:
			syscall--;
			break;

		case ADD_TWICE:
			syscall += 2;
			break;

		case FROM_LOWER:
			syscall = estimate_syscall_lower(lib_index, nid, nid_file);
			break;

		case FROM_HIGHER:
			syscall = estimate_syscall_higher(lib_index, nid, nid_file);
			break;
			
		/*
		case ADD:
			syscall++;
			break;
		*/

		case FROM_LOWEST:
			syscall = estimate_syscall_lowest(lib_index, nid, nid_file);
			break;

		default:
			LOGSTR1("Method %d not known\n", type);
			break;
	}
		
    syscall = find_first_free_syscall(lib_index, syscall);
    LOGSTR1(" 0x%08lX\n", syscall);
	
	*stub = MAKE_SYSCALL(syscall);
    LOGSTR0("--Done.\n");

	add_nid_to_table(nid, MAKE_SYSCALL(syscall), lib_index);
	
	sceKernelDcacheWritebackInvalidateAll();
	return syscall;
#else
    return 0;
#endif	    
}
