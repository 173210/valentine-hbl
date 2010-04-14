#include "eloader.h"
#include "debug.h"

// Estimate a syscall
// Pass reference NID and distance from desidered function in the export table
// Return syscall number
// m0skit0's first implementation, no estimation
/*
u32 estimate_syscall_old(u32 ref_nid, int off) {
	u32 ref_call;
	
	get_call_nidtable(ref_nid, &ref_call);
	
	return MAKE_SYSCALL(GET_SYSCALL_NUMBER(ref_call) + off);
}
*/

// Estimate a syscall
// Pass reference NID and distance from desired function in the export table
// Return syscall instruction
// ab5000's implementation, estimation works :)
/*
u32 estimate_syscall(const char *lib, u32 nid)
{
	SceUID fd;
	int nidnum, ret, i, sysl_i, sysm_i;
	char buf[64];
	u16 sysl, sysm, sysc;
	u32 curnid, curcall;

	// Open library file
	strcpy(buf, LIB_PATH);
	strcat(buf, lib);
	strcat(buf, LIB_EXTENSION);

	if((fd = sceIoOpen(buf, PSP_O_RDONLY, 0777)) < 0)	
		return 0;

	// Get NID count
	nidnum = sceIoLseek(fd, 0, PSP_SEEK_END) / sizeof(u32);
	if(nidnum < 0)
	{		
		sceIoClose(fd);		
		return 0;
	}
	
	ret = sceIoLseek(fd, 0, PSP_SEEK_SET);
	if(ret < 0)
	{		
		sceIoClose(fd);		
		return 0;
	}
	
	sysl = 0xFFFF;
	sysl_i = 0;
	sysm = 0;
	sysm_i = -1;
	
	// Find lowest syscall and desired nid position
	for(i=0;i<nidnum;i++)
	{
		ret = sceIoRead(fd, &curnid, sizeof(curnid));
		if(ret < 0)
		{
			sceIoClose(fd);		
			return 0;
		}
		
		// Check if we have found desidered NID
		if(curnid == nid)
			sysm_i = i;
		
		// Get syscall from MoHH stubs
		get_call_nidtable(curnid, &curcall);
		
		// Get syscall value
		sysc = GET_SYSCALL_NUMBER(curcall);
		
		// Check lowest syscall
		if(sysc && sysc < sysl)
		{
			sysl = sysc;
			sysl_i = i;
		}
	}
	
	// File I/O done, close the file
	sceIoClose(fd);
	
	// Lowest syscall if 0xFFFF only if the library isn't in MoHH stubs
	// sysm_i is -1 only if the NID isn't found in library
	if(sysl == 0xFFFF || sysm_i == -1)		
		return 0;
	
	// Find unknown syscall
	sysm = sysl + ((sysm_i < sysl_i) ? (nidnum - sysl_i + sysm_i) : (sysm_i - sysl_i));

#ifdef DEBUG
	write_debug(" ESTIMATION ", NULL, 0);
	write_debug(" LIBRARY ", lib, strlen(lib));
	write_debug(" NID ", &nid, sizeof(nid));
	write_debug(" OLD ESTIMATION ", &sysm, sizeof(sysm));
	estimate_syscall_new (lib, nid);
#endif
	
	return MAKE_SYSCALL(sysm);
}
*/

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
	strcat(file_path, lib);
	strcat(file_path, LIB_EXTENSION);

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
