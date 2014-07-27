#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/utils/string.h>
#include <common/sdk.h>
#include <common/debug.h>
#include <common/utils.h>
#include <exploit_config.h>

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

/* the PS3 does not allow long filenames,
So we are using a hash + base36 encoding for nids
*/
#ifdef SMALL_FILENAMES
unsigned long hash_djb2(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
    {
        //We convert to lowercase here because it matches our requirements.
       // This, of course, completely ruins the hash algorithm for case sensitive strings, don't do this at home!!!
        if ( c >= 'A' && c <= 'Z' )
        {
            c+= 'a' - 'A';
        }

        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

int toBase36 (char * dest,const unsigned long src)
{
       char base_digits[36] =
	 {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

   int converted_number[64];
   int base, index=0;

   base = 36;
   unsigned long number_to_convert = src;

   /* convert to the indicated base */
   while (number_to_convert != 0)
   {
	 converted_number[index] = number_to_convert % base;
	 number_to_convert = number_to_convert / base;
	 ++index;
   }

   /* now print the result in reverse order */
   --index;  /* back up to last entry in the array */
   for(  ; index>=0; index--) /* go backward through array */
   {
	 *dest = base_digits[converted_number[index]];
     dest++;
   }
   *dest = 0; //terminate string
   return 1;
}
#endif

// Opens .nids file for a given library
SceUID open_nids_file(const char* libname)
{
	char file[MAX_LIB_NAME_LEN + 100];

    int fw_ver = get_fw_ver();

	int i = 0;

#ifdef SMALL_FILENAMES
    char lib[9];
    unsigned long hash = hash_djb2(libname);
    toBase36 (lib,hash);
    LOG_PRINTF("converted lib name %s into %s", (u32) libname, (u32) lib);
#else
    const char * lib = libname;
#endif
    //We try to open a lib file base on the version of the firmware as precisely as possible,
    //then fallback to less precise versions. for example,try in this order:
    // libs_503, libs_50x, libs_5xx, libs
	do
	{
		switch (i)
		{
#ifdef FLAT_FOLDER
			case 0:
				_sprintf(file, "%s_%d%s%s", (u32)LIB_PATH, fw_ver, (u32)lib, (u32)LIB_EXTENSION);
				break;
			case 1:
				_sprintf(file, "%s_%dx%s%s", (u32)LIB_PATH, fw_ver / 10, (u32)lib, (u32)LIB_EXTENSION);
				break;
			case 2:
				_sprintf(file, "%s_%dxx%s%s", (u32)LIB_PATH, fw_ver / 100, (u32)lib, (u32)LIB_EXTENSION);
				break;
			case 3:
				_sprintf(file, "%s%s%s", (u32)LIB_PATH, (u32)lib, (u32)LIB_EXTENSION);
				break;
#else
			case 0:
				_sprintf(file, "%s_%d/%s%s", (u32)LIB_PATH, fw_ver, (u32)lib, (u32)LIB_EXTENSION);
				break;
			case 1:
				_sprintf(file, "%s_%dx/%s%s", (u32)LIB_PATH, fw_ver / 10, (u32)lib, (u32)LIB_EXTENSION);
				break;
			case 2:
				_sprintf(file, "%s_%dxx/%s%s", (u32)LIB_PATH, fw_ver / 100, (u32)lib, (u32)LIB_EXTENSION);
				break;
			case 3:
				_sprintf(file, "%s/%s%s", (u32)LIB_PATH, (u32)lib, (u32)LIB_EXTENSION);
				break;
#endif
		}
		i++;
	}
	while ((i < 4) && !file_exists(file));

	LOG_PRINTF("Opening %s\n", (u32)file);

	return sceIoOpen(file, PSP_O_RDONLY, 0777);
}


/*
 * Checks if a syscall looks normal compared to other libraries boundaries.
 * returns 1 if ok, 0 if not
*/
int check_syscall_boundaries (u32 syscall, u32 boundary_low, u32 boundary_high)
{
    if (syscall <= boundary_low)
    {
        LOG_PRINTF("--ERROR: SYSCALL OUT OF LIB'S RANGE, should be higher than 0x%08X, but we got 0x%08X\n", boundary_low, syscall);
        return 0;
    }
    if (syscall >= boundary_high)
    {
        LOG_PRINTF("--ERROR: SYSCALL OUT OF LIB'S RANGE, should be lower than 0x%08X, but we got 0x%08X\n", boundary_high, syscall);
        return 0;
    }

    return 1;
}

u32 find_first_free_syscall (int lib_index, u32 start_syscall)
{
	int index = -1;
    u32 syscall = start_syscall;
	int boundary_low = 0, boundary_high = 0;

    int ret = get_syscall_boundaries(lib_index, &boundary_low,  &boundary_high);

    if (!ret)
    {
        LOG_PRINTF("--ERROR GETTING SYSCALL BOUNDARIES\n");
        return 0;
    }

	while ((index = get_call_index(MAKE_SYSCALL(syscall))) >= 0)
	{
		LOG_PRINTF("--ESTIMATED SYSCALL 0x%08X ALREADY EXISTS AT INDEX %d\n", syscall, index);
		syscall--;
        if (!check_syscall_boundaries (syscall, boundary_low, boundary_high))
        {
            syscall = boundary_high - 1;
            //Risk of infinite loop here ?
        }
	}

    return syscall;
}

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
// Estimate a syscall from library's closest known syscall
u32 estimate_syscall_closest(int lib_index, u32 nid, SceUID nid_file)
{
	LOG_PRINTF("=> FROM CLOSEST\n");
	
	// Get NIDs index in file
	int nid_index = find_nid_in_file(nid_file, nid);

	if (nid_index < 0)
	{
		LOG_PRINTF("->ERROR: NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}

	LOG_PRINTF("NID index in file: %d\n", nid_index);

	// Get higher and lower known NIDs
	int higher_nid_index = get_higher_known_nid(lib_index, nid);
	int lower_nid_index = get_lower_known_nid(lib_index, nid);

	// Get higher and lower NID index on file
	int higher_index_file = -1;
	if (higher_nid_index >= 0)
	{
		higher_index_file = find_nid_in_file(nid_file, globals->nid_table.table[higher_nid_index].nid);
		LOG_PRINTF("Higher known NID: 0x%08X; index: %d\n", globals->nid_table.table[higher_nid_index].nid, higher_index_file);
	}

	int lower_index_file = -1;
	if (lower_nid_index >= 0)
	{
		lower_index_file = find_nid_in_file(nid_file, globals->nid_table.table[lower_nid_index].nid);
		LOG_PRINTF("Lower known NID: 0x%08X; index: %d\n", globals->nid_table.table[lower_nid_index].nid, lower_index_file);
	}

	// Check which one is closer
	int closest_index = -1;

	if (higher_index_file >= 0)
	{
		if (lower_index_file >= 0)
		{
			int higher_dist = higher_index_file - nid_index;
			int lower_dist = nid_index - lower_index_file;
			if (higher_dist < lower_dist)
				closest_index = higher_index_file;
			else
				closest_index = lower_index_file;
		}
		else
			closest_index = higher_index_file;
	}
	else
		closest_index = lower_index_file;

	LOG_PRINTF("Closest: %d\n", closest_index);

	// Estimate based on closest known NID
	u32 estimated_syscall;
	if (closest_index > nid_index)
		estimated_syscall = GET_SYSCALL_NUMBER(globals->nid_table.table[higher_nid_index].call) - (higher_index_file - nid_index);
	else
		estimated_syscall = GET_SYSCALL_NUMBER(globals->nid_table.table[lower_nid_index].call) + (nid_index - lower_index_file);

	LOG_PRINTF("--FIRST ESTIMATED SYSCALL: 0x%08X\n", estimated_syscall);

	// Check if estimated syscall already exists (should be very rare)
    estimated_syscall = find_first_free_syscall(lib_index, estimated_syscall);

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	LOG_PRINTF("--FINAL ESTIMATED SYSCALL: 0x%08X\n", estimated_syscall);

	return estimated_syscall;
}

// Estimate a syscall from library's closest lower known syscall
u32 estimate_syscall_higher(int lib_index, u32 nid, SceUID nid_file)
{
	LOG_PRINTF("=> FROM HIGHER\n");
    
	// Get NIDs index in file
	int nid_index = find_nid_in_file(nid_file, nid);

	if (nid_index < 0)
	{
		LOG_PRINTF("->ERROR: NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}

	LOG_PRINTF("NID index in file: %d\n", nid_index);

	int higher_nid_index = get_higher_known_nid(lib_index, nid);

	if (higher_nid_index < 0)
	{
		LOG_PRINTF("NID is highest in library, switching to lower method\n");
		return estimate_syscall_lower(lib_index, nid, nid_file);  // Infinite call risk here!!
	}

	LOG_PRINTF("Higher known NID/SYSCALL: 0x%08X/0x%08X\n", globals->nid_table.table[higher_nid_index].nid, GET_SYSCALL_NUMBER(globals->nid_table.table[higher_nid_index].call));

	int higher_index = find_nid_in_file(nid_file, globals->nid_table.table[higher_nid_index].nid);

	if (higher_index < 0)
	{
		LOG_PRINTF("->ERROR: LOWER NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}

	LOG_PRINTF("Lower known NID index: %d\n", higher_index);

	u32 estimated_syscall = GET_SYSCALL_NUMBER(globals->nid_table.table[higher_nid_index].call) - (higher_index - nid_index);

	LOG_PRINTF("--FIRST ESTIMATED SYSCALL: 0x%08X\n", estimated_syscall);

	// Check if estimated syscall already exists (should be very rare)
    estimated_syscall = find_first_free_syscall(lib_index, estimated_syscall);

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	LOG_PRINTF("--FINAL ESTIMATED SYSCALL: 0x%08X\n", estimated_syscall);

	return estimated_syscall;
}

// Estimate a syscall from library's closest lower known syscall
u32 estimate_syscall_lower(int lib_index, u32 nid, SceUID nid_file)
{
	LOG_PRINTF("=> FROM LOWER\n");
    
	// Get NIDs index in file
	int nid_index = find_nid_in_file(nid_file, nid);

	if (nid_index < 0)
	{
		LOG_PRINTF("->ERROR: NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}

	LOG_PRINTF("NID index in file: %d\n", nid_index);

	int lower_nid_index = get_lower_known_nid(lib_index, nid);

	if (lower_nid_index < 0)
	{
		LOG_PRINTF("NID is lowest in library, switching to higher method\n");
		return estimate_syscall_higher(lib_index, nid, nid_file);  // Infinite call risk here!!
	}

	LOG_PRINTF("Lower known NID/SYSCALL: 0x%08X/0x%08X\n", globals->nid_table.table[lower_nid_index].nid, GET_SYSCALL_NUMBER(globals->nid_table.table[lower_nid_index].call));

	int lower_index = find_nid_in_file(nid_file, globals->nid_table.table[lower_nid_index].nid);

	if (lower_index < 0)
	{
		LOG_PRINTF("->ERROR: LOWER NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}

	LOG_PRINTF("Lower known NID index: %d\n", lower_index);

	u32 estimated_syscall = GET_SYSCALL_NUMBER(globals->nid_table.table[lower_nid_index].call) + (nid_index - lower_index);

	LOG_PRINTF("--FIRST ESTIMATED SYSCALL: 0x%08X\n", estimated_syscall);

	// Check if estimated syscall already exists (should be very rare)
    estimated_syscall = find_first_free_syscall(lib_index, estimated_syscall);

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	LOG_PRINTF("--FINAL ESTIMATED SYSCALL: 0x%08X\n", estimated_syscall);

	return estimated_syscall;
}

// Estimate a syscall from library's lowest known syscall
u32 estimate_syscall_lowest(int lib_index, u32 nid, SceUID nid_file)
{
	LOG_PRINTF("=> FROM LOWEST\n");
    
	// Get NIDs index in file
	int nid_index = find_nid_in_file(nid_file, nid);

	sceIoClose(nid_file);

	LOG_PRINTF("--NID index in file: %d\n", nid_index);

	if (nid_index < 0)
	{
		LOG_PRINTF("->ERROR: NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}

	int estimated_syscall;
	if ((u32)nid_index >= globals->lib_table.table[lib_index].lowest_index)
	{
		estimated_syscall = (int)globals->lib_table.table[lib_index].lowest_syscall + nid_index - (int)globals->lib_table.table[lib_index].lowest_index;
	}
	else
	{
		estimated_syscall = (int)globals->lib_table.table[lib_index].lowest_syscall + nid_index + (int)globals->lib_table.table[lib_index].num_lib_exports - (int)globals->lib_table.table[lib_index].lowest_index + (int)globals->lib_table.table[lib_index].gap;
	}

	LOG_PRINTF("--FIRST ESTIMATED SYSCALL: 0x%08X\n", estimated_syscall);

	// Check if estimated syscall already exists (should be very rare)
	// This is not needed if the syscalls are known to be there
	if (!globals->syscalls_known)
		estimated_syscall = find_first_free_syscall(lib_index, estimated_syscall);

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	LOG_PRINTF("--FINAL ESTIMATED SYSCALL: 0x%08X\n", estimated_syscall);

	return estimated_syscall;
}

// Estimate a syscall
// Pass reference NID and distance from desidered function in the export table
// Return syscall instruction
// Syscall accuracy (%) = (exports known from library / total exports from library) x 100
// m0skit0's implementation
u32 estimate_syscall(const char *lib, u32 nid, HBLEstimateMethod method)
{
	LOG_PRINTF("=> ESTIMATING %s : 0x%08X\n", (u32)lib, nid);

	// Finding the library on table
	int lib_index = get_lib_index(lib);

	if (lib_index < 0)
	{
		LOG_PRINTF("->ERROR: LIBRARY NOT FOUND ON TABLE  %s\n", (u32)lib);
        return 0;
    }

	
	LOGLIB(globals->lib_table.table[lib_index]);

	// Cannot estimate jump system call
	if (globals->lib_table.table[lib_index].calling_mode == JUMP_MODE)
	{
		LOG_PRINTF("->WARNING: trying to estimate jump system call\n");
		return 0;
	}

	SceUID nid_file = open_nids_file(lib);

	if (nid_file < 0)
	{
		LOG_PRINTF("->ERROR: couldn't open NIDS file for %s\n", (u32)lib);
		return 0;
	}

	u32 estimated_syscall;
	switch (method)
	{
		case FROM_CLOSEST:
			estimated_syscall = estimate_syscall_closest(lib_index, nid, nid_file);
			break;

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
			LOG_PRINTF("Unknown estimation method %d\n", method);
			estimated_syscall = 0;
			break;
	}

    sceIoClose(nid_file);

	add_nid_to_table(nid, MAKE_SYSCALL(estimated_syscall), lib_index);

	LOG_PRINTF("Estimation done\n");

	return MAKE_SYSCALL(estimated_syscall);
}

// m0skit0's attempt
// Needs to be more independent from sdk_hbl.S
u32 reestimate_syscall(const char * lib, u32 nid, u32* stub, HBLEstimateMethod type)
{
#ifdef REESTIMATE_SYSCALL
	LOG_PRINTF("=Reestimating function 0x%08X for stub 0x%08X: ", nid, (u32)stub);

	// Finding the library on table
	int lib_index = get_lib_index(lib);

	if (lib_index < 0)
	{
		LOG_PRINTF("--ERROR: LIBRARY NOT FOUND ON TABLE  %s\n", (u32)lib);
        return 0;
    }

	SceUID nid_file = open_nids_file(lib);

	if (nid_file < 0)
	{
		LOG_PRINTF("->ERROR: couldn't open NIDS file for %s\n", (u32)lib);
		return 0;
	}

	stub++;
	u32 syscall = GET_SYSCALL_NUMBER(*stub);
    LOG_PRINTF("0x%08X -->", syscall);

	switch (type)
	{
		case FROM_CLOSEST:
			syscall = estimate_syscall_closest(lib_index, nid, nid_file);
			break;

		case FROM_LOWER:
			syscall = estimate_syscall_lower(lib_index, nid, nid_file);
			break;

		case FROM_HIGHER:
			syscall = estimate_syscall_higher(lib_index, nid, nid_file);
			break;

		case FROM_LOWEST:
			syscall = estimate_syscall_lowest(lib_index, nid, nid_file);
			break;

		case SUBSTRACT:
			syscall--;
			break;

		case ADD_TWICE:
			syscall += 2;
			break;

		default:
			LOG_PRINTF("Method %d not known\n", type);
			break;
	}

    sceIoClose(nid_file);

    syscall = find_first_free_syscall(lib_index, syscall);
    LOG_PRINTF(" 0x%08X\n", syscall);

	*stub = MAKE_SYSCALL(syscall);
    LOG_PRINTF("--Done.\n");

	add_nid_to_table(nid, MAKE_SYSCALL(syscall), lib_index);

	CLEAR_CACHE;
	return syscall;
#else
    return 0;
#endif
}
#endif
