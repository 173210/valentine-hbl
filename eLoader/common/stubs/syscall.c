#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/sdk.h>
#include <exploit_config.h>

// Searches for NID in a NIDS file and returns the index
static int find_nid_file(SceUID fd, int nid)
{
	int i = 0;
	int cur, ret;

	sceIoLseek(fd, 0, PSP_SEEK_SET);
	while((ret = sceIoRead(fd, &cur, sizeof(cur))) > 0)
		if (cur == nid)
			return i;
		else
			i++;

	return ret;
}

// Opens .nids file for a given library
SceUID open_nids_file(const char *lib)
{
	const char lib_path[] = LIB_PATH;
	const char lib_ext[] = LIB_EXT;
	char file[sizeof(lib_path) + MAX_LIB_NAME_LEN + sizeof(lib_ext) + 3];
	int fw_ver = globals->module_sdk_version;
	int i, j, ret;

	for (i = 0; lib_path[i]; i++)
		file[i] = lib_path[i];

	//We try to open a lib file base on the version of the firmware as precisely as possible,
	//then fallback to less precise versions. for example,try in this order:
	// libs_503, libs_50x, libs_5xx, libs
	file[i++] = '_';

	j = i + 2;
	while (i <= j) {
		fw_ver >>= 8;
		file[i++] = '0' + (fw_ver & 0xF);
	}

	for (j = 0; lib[j]; j++) {
		file[i++] = lib[j];
	}

	for (j = 0; lib_ext[j]; j++) {
		file[i++] = lib_ext[j];
	}

	file[i] = '\0';

	for (i = 0; i < 3; i++) {
		ret = sceIoOpen(file, PSP_O_RDONLY, 0777);
		if (ret > 0)
			break;

		file[j--] = 'x';
	}

	if (ret < 0) {
		for (i = 0; lib[i]; i++) {
			file[j++] = lib[i];
		}

		for (i = 0; lib_ext[i]; i++) {
			file[j++] = lib_ext[i];
		}

		ret = sceIoOpen(file, PSP_O_RDONLY, 0777);
	}

	return ret;
}


/*
 * Checks if a syscall looks normal compared to other libraries boundaries.
 * returns 1 if ok, 0 if not
 */
static int check_syscall(int call, int low, int high)
{
	if (call <= low) {
        	dbg_printf("--ERROR: SYSCALL OUT OF LIB'S RANGE, should be higher than 0x%08X, but we got 0x%08X\n",
			low, call);
		return 0;
	} else if (call >= high) {
		dbg_printf("--ERROR: SYSCALL OUT OF LIB'S RANGE, should be lower than 0x%08X, but we got 0x%08X\n",
			high, call);
		return 0;
	} else
		return 1;
}

static int find_first_free_syscall(int lib, int start)
{
	int syscall = start;
	int low = 0, high = 0;
	int index;

	if (!get_syscall_boundaries(lib, &low,  &high)) {
		dbg_printf("--ERROR GETTING SYSCALL BOUNDARIES\n");
		return 0;
	}

	while ((index = get_call_index(MAKE_SYSCALL(syscall))) >= 0) {
		dbg_printf("--ESTIMATED SYSCALL 0x%08X ALREADY EXISTS AT INDEX %d\n", syscall, index);
		syscall--;
		if (!check_syscall(syscall, low, high)) {
			syscall = high - 1;
			//Risk of infinite loop here ?
		}
	}

	return syscall;
}

// Estimate a syscall from library's closest known syscall
static int estimate_syscall_closest(int lib_index, int nid_index, int nid, SceUID fd)
{
	int higher_nid_index, lower_nid_index;
	int closest_index = -1;

	int higher_file_index = -1;
	int lower_file_index = -1;

	int est_call;

	dbg_printf("=> FROM CLOSEST\n");

	// Get higher and lower known NIDs
	higher_nid_index = get_higher_known_nid(lib_index, nid);
	lower_nid_index = get_lower_known_nid(lib_index, nid);

	if (lower_nid_index >= 0) {
		lower_file_index = find_nid_file(fd, globals->nid_table[lower_nid_index].nid);
		dbg_printf("Lower known NID: 0x%08X; index: %d\n",
			globals->nid_table[lower_nid_index].nid, lower_file_index);
	}

	// Get higher and lower NID index on file
	if (higher_nid_index >= 0) {
		higher_file_index = find_nid_file(fd, globals->nid_table[higher_nid_index].nid);
		dbg_printf("Higher known NID: 0x%08X; index: %d\n",
			globals->nid_table[higher_nid_index].nid, higher_file_index);

		// Check which one is closer
		closest_index = higher_file_index < 0 ||
			(lower_file_index >= 0 && higher_file_index - nid_index >= nid_index - lower_file_index) ?
				lower_file_index : higher_file_index;
	}

	dbg_printf("Closest: %d\n", closest_index);

	// Estimate based on closest known NID
	if (closest_index > nid_index)
		est_call = GET_SYSCALL_NUMBER(globals->nid_table[higher_nid_index].call)
				+ nid_index - higher_file_index;
	else
		est_call = GET_SYSCALL_NUMBER(globals->nid_table[lower_nid_index].call)
				+ nid_index - lower_file_index;

	return est_call;
}

// Estimate a syscall from library's lowest known syscall
static int estimate_syscall_lowest(int lib_index, int nid_index)
{
	int est_call;

	dbg_printf("=> FROM LOWEST\n");

	est_call = globals->lib_table[lib_index].lowest_syscall + nid_index - globals->lib_table[lib_index].lowest_index;
	if (nid_index < globals->lib_table[lib_index].lowest_index)
		est_call += globals->lib_table[lib_index].num_lib_exports + globals->lib_table[lib_index].gap;

	return est_call;
}

// Estimate a syscall
// Pass reference NID and distance from desidered function in the export table
// Return syscall number
// Syscall accuracy (%) = (exports known from library / total exports from library) x 100
// m0skit0's implementation
int estimate_syscall(const char *lib, int nid, HBLEstimateMethod method)
{
	SceUID fd;
	int lib_index, nid_index, est_call;

	dbg_printf("=> ESTIMATING %s : 0x%08X\n", lib, nid);

	// Finding the library on table
	lib_index = get_lib_index(lib);
	if (lib_index < 0) {
		dbg_printf("->ERROR: LIBRARY NOT FOUND ON TABLE  %s\n", lib);
		return 0;
	}

	// Cannot estimate jump system call
	if (globals->lib_table[lib_index].calling_mode == JUMP_MODE) {
		dbg_printf("->WARNING: trying to estimate jump system call\n");
		return 0;
	}

	fd = open_nids_file(lib);
	if (fd < 0) {
		dbg_printf("->ERROR: couldn't open NIDS file for %s\n", lib);
		return 0;
	}
	
	// Get NIDs index in file
	nid_index = find_nid_file(fd, nid);
	if (nid_index < 0) {
		dbg_printf("->ERROR: NID NOT FOUND ON .NIDS FILE\n");
		return 0;
	}
	dbg_printf("NID index in file: %d\n", nid_index);

	switch (method)
	{
		case FROM_CLOSEST:
			est_call = estimate_syscall_closest(lib_index, nid_index, nid, fd);
			break;

		case FROM_LOWEST:
			est_call = estimate_syscall_lowest(lib_index, nid_index);
			break;

		default:
			dbg_printf("Unknown estimation method %d\n", method);
			est_call = 0;
			break;
	}

	dbg_printf("--FIRST ESTIMATED SYSCALL: 0x%08X\n", est_call);

	// Check if estimated syscall already exists (should be very rare)
	// This is not needed if the syscalls are known to be there
	if (!globals->syscalls_known)
		est_call = find_first_free_syscall(lib_index, est_call);

	// TODO: refresh library descriptor with more accurate information if any estimated syscalls already existed

	dbg_printf("--FINAL ESTIMATED SYSCALL: 0x%08X\n", est_call);

	sceIoClose(fd);

	add_nid(nid, MAKE_SYSCALL(est_call), lib_index);

	dbg_printf("Estimation done\n");

	return est_call;
}

// m0skit0's attempt
// Needs to be more independent from sdk_hbl.S
#ifdef REESTIMATE_SYSCALL
int reestimate_syscall(const char *lib, int nid, int *stub, HBLEstimateMethod method)
{
	SceUID fd;
	int lib_index, syscall;

	syscall = GET_SYSCALL_NUMBER(stub[1]);

	dbg_printf("=Reestimating function 0x%08X for stub 0x%08X: 0x%08X\n",
		nid, stub, syscall);

	switch (type) {
		case SUBSTRACT:
			syscall = find_first_free_syscall(lib, --syscall);
			break;
		case ADD_TWICE:
			syscall = find_first_free_syscall(lib, ++syscall);
			syscall++;
			break;
		default:
			syscall = estimate_syscall(lib, nid, method);
	}
	dbg_printf("--> 0x%08X\n", syscall);

	stub[1] = MAKE_SYSCALL(syscall);

	dbg_printf("--Done.\n");

	return syscall;
}
#endif
