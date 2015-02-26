#include <common/stubs/syscall.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/memory.h>
#include <common/path.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <hbl/modmgr/modmgr.h>
#include <hbl/stubs/hook.h>
#include <hbl/stubs/md5.h>
#include <hbl/stubs/resolve.h>
#include <hbl/eloader.h>
#include <hbl/settings.h>
#include <exploit_config.h>

//Note: most of the Threads monitoring code largely inspired (a.k.a. copy/pasted) by Noobz-Fanjita-Ditlew. Thanks guys!

// Hooks for some functions used by Homebrews
// Hooks are put in place by resolve_imports() calling setup_hook()

#define PATH_MAX 260
#define SIZE_THREAD_TRACKING_ARRAY 32
#define MAX_CALLBACKS 32

static int dirLen;
static char mod_chdir[PATH_MAX] = HBL_ROOT; //cwd of the currently running module
static int cur_cpufreq = 0; //current cpu frequency
static int cur_busfreq = 0; //current bus frequency
static SceUID running_th[SIZE_THREAD_TRACKING_ARRAY];
static SceUID pending_th[SIZE_THREAD_TRACKING_ARRAY];
static SceUID exited_th[SIZE_THREAD_TRACKING_ARRAY];
static SceUID openFiles[16];
static unsigned numOpenFiles = 0;
static SceUID osAllocs[512];
static unsigned osAllocNum = 0;
static SceKernelCallbackFunction cbfuncs[MAX_CALLBACKS];
static int cbids[MAX_CALLBACKS];
static int cbcount = 0;
static int audio_th[8];
static int cur_ch_id = -1;

void (* net_term_func[5])();
int net_term_num = 0;

#ifdef FORCE_HARDCODED_VRAM_SIZE
// Hardcode edram size if the function is not available
u32 _hook_sceGeEdramGetSize() {
    return 0x00200000;
}
#endif

//
// Thread Hooks
//

// Forward declarations (functions used before they are defined lower down the file)
int _hook_sceAudioChRelease(int channel);
int _hook_sceAudioSRCChRelease();
int _hook_scePowerSetClockFrequency(int pllfreq, int cpufreq, int busfreq);
SceUID sceIoDopen_Vita(const char *dirname);
u32 _hook_sceKernelUtilsMt19937UInt(SceKernelUtilsMt19937Context* ctx);


// Thread hooks original code thanks to Noobz & Fanjita, adapted by wololo
/*****************************************************************************/
/* Special exitThread handling:                                              */
/*                                                                           */
/*   Delete the thread from the list of tracked threads, then pass on the    */
/*   call.                                                                   */
/*                                                                           */
/* See also hookExitDeleteThread below                                       */
/*****************************************************************************/
int _hook_sceKernelExitThread(int status)
{
	int i;
	int found = 0;
	int thid = sceKernelGetThreadId();

	dbg_printf("Enter hookExitThread : %08X\n", thid);

	WAIT_SEMA(globals->thSema, 1, 0);

	for (i = 0; i < num_run_th; i++) {
		if (running_th[i] == thid) {
			found = 1;
			num_run_th--;
			dbg_printf("Running threads: %d\n", num_run_th);
		}

		if (found && i <= SIZE_THREAD_TRACKING_ARRAY - 2)
			running_th[i] = running_th[i + 1];
	}

	running_th[num_run_th] = 0;


#ifdef DELETE_EXIT_THREADS
	dbg_printf("Num exit thr: %08X\n", num_exit_th);

	/*************************************************************************/
	/* Add to exited list                                                    */
	/*************************************************************************/
	if (num_exit_th < SIZE_THREAD_TRACKING_ARRAY)
	{
		dbg_printf("Set array\n");
		exited_th[num_exit_th] = thid;
		num_exit_th++;

		dbg_printf("Exited threads: %d\n", num_exit_th);
	}
#endif

#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
	for (i = 0; i < 8; i++)
	{
		WAIT_SEMA(globals->audioSema, 1, 0);
		if (audio_th[i] == thid)
			_hook_sceAudioChRelease(i);
		sceKernelSignalSema(globals->audioSema, 1);
	}
#endif

	sceKernelSignalSema(globals->thSema, 1);

	dbg_printf("Exit hookExitThread\n");

	return sceKernelExitThread(status);
}

/*****************************************************************************/
/* Special exitThread handling:                                              */
/*                                                                           */
/*   Delete the thread from the list of tracked threads, then pass on the    */
/*   call.                                                                   */
/*                                                                           */
/* See also hookExitThread above                                             */
/*****************************************************************************/
int _hook_sceKernelExitDeleteThread(int status)
{
	int i;
	int found = 0;
	int thid = sceKernelGetThreadId();

    
	dbg_printf("Enter hookExitDeleteThread : %08X\n", thid);

	WAIT_SEMA(globals->thSema, 1, 0);

	for (i = 0; i < num_run_th; i++) {
		if (running_th[i] == thid) {
			found = 1;
			num_run_th--;
			dbg_printf("Running threads: %d\n", num_run_th);
		}

		if (found && i <= SIZE_THREAD_TRACKING_ARRAY - 2)
			running_th[i] = running_th[i + 1];
	}

	running_th[num_run_th] = 0;



	dbg_printf("Exit hookExitDeleteThread\n");

#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
	for (i = 0; i < 8; i++) {
		WAIT_SEMA(globals->audioSema, 1, 0);
		if (audio_th[i] == thid)
			_hook_sceAudioChRelease(i);
		sceKernelSignalSema(globals->audioSema, 1);
	}
#endif

	//return (sceKernelExitDeleteThread(status));
	// (Patapon does not import this function)
	// But modules on p5 do.

#ifdef DELETE_EXIT_THREADS
	dbg_printf("Num exit thr: %08X\n", num_exit_th);

	/*************************************************************************/
	/* Add to exited list                                                    */
	/*************************************************************************/
	if (num_exit_th < SIZE_THREAD_TRACKING_ARRAY)
	{
		dbg_printf("Set array\n");
		exited_th[num_exit_th] = thid;
		num_exit_th++;

		dbg_printf("Exited threads: %d\n", num_exit_th);
	}
#endif

	sceKernelSignalSema(globals->thSema, 1);
	return sceKernelExitDeleteThread(status);
}


/*****************************************************************************/
/* Special createThread handling:                                            */
/*                                                                           */
/*   Pass on the call                                                        */
/*   Add the thread to the list of pending threads.                          */
/*****************************************************************************/
SceUID _hook_sceKernelCreateThread(const char *name, void * entry,
        int initPriority, int stackSize, SceUInt attr,
        SceKernelThreadOptParam *option)
{

    
	SceUID lreturn = sceKernelCreateThread(name, entry, initPriority, stackSize, attr, option);

    if (lreturn < 0)
    {
        return lreturn;
    }


    dbg_printf("API returned %08X\n", lreturn);

    /*************************************************************************/
    /* Add to pending list                                                   */
    /*************************************************************************/
    WAIT_SEMA(globals->thSema, 1, 0);
    if (num_pend_th < SIZE_THREAD_TRACKING_ARRAY)
    {
      dbg_printf("Set array\n");
      pending_th[num_pend_th] = lreturn;
      num_pend_th++;

      dbg_printf("Pending threads: %d\n", num_pend_th);
    }

    sceKernelSignalSema(globals->thSema, 1);
    return(lreturn);

}

/*****************************************************************************/
/* Special startThread handling:                                             */
/*                                                                           */
/*   Remove the thread from the list of pending threads.                     */
/*   Add the thread to the list of tracked threads, then pass on the call.   */
/*****************************************************************************/
int _hook_sceKernelStartThread(SceUID thid, SceSize arglen, void *argp)
{
	int i;
	int found = 0;

	dbg_printf("Enter hookRunThread: %08X\n", thid);

	WAIT_SEMA(globals->thSema, 1, 0);

	dbg_printf("Number of pending threads: %08X\n", num_pend_th);

	/***************************************************************************/
	/* Remove from pending list                                                */
	/***************************************************************************/
	for (i = 0; i < num_pend_th; i++) {
		if (pending_th[i] == thid) {
			found = 1;
			num_pend_th--;
			dbg_printf("Pending threads: %d\n", num_pend_th);
		}

		if (found && i <= SIZE_THREAD_TRACKING_ARRAY - 2)
			pending_th[i] = pending_th[i + 1];
	}

	if (found) {
		pending_th[num_pend_th] = 0;

		/***************************************************************************/
		/* Add to running list                                                     */
		/***************************************************************************/
		dbg_printf("Number of running threads: %08X\n", num_run_th);
		if (num_run_th < SIZE_THREAD_TRACKING_ARRAY) {
			running_th[num_run_th] = thid;
			num_run_th++;
			dbg_printf("Running threads: %d\n", num_run_th);
		}
	}

	sceKernelSignalSema(globals->thSema, 1);


	dbg_printf("Exit hookRunThread: %08X\n", thid);

	return sceKernelStartThread(thid, arglen, argp);

}



//Cleans up Threads and allocated Ram
void threads_cleanup()
{

        int lthisthread = sceKernelGetThreadId();
    u32 i;
    dbg_printf("Threads cleanup\n");
    WAIT_SEMA(globals->thSema, 1, 0);
#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
    dbg_printf("cleaning audio threads\n");
	for (i=0;i<8;i++)
	{
		//WAIT_SEMA(globals->audioSema, 1, 0);
		_hook_sceAudioChRelease(i);
		//sceKernelSignalSema(globals->audioSema, 1);
	}
#endif

    dbg_printf("Running Threads cleanup\n");
    while (num_run_th > 0)
    {
        dbg_printf("%d running threads remain\n", num_run_th);
        if (running_th[num_run_th - 1] != lthisthread)
        {
            dbg_printf("Kill thread ID %08X\n", running_th[num_run_th - 1]);
            kill_thread(running_th[num_run_th - 1]);
        }
        else
        {
        dbg_printf("Not killing myself - yet\n");
        }
        num_run_th--;
    }


    dbg_printf("Pending Threads cleanup\n");
    while (num_pend_th > 0)
    {
        dbg_printf("%d pending threads remain\n", num_pend_th);
        /*************************************************************************/
        /* This test shouldn't really apply to pending threads, but do it        */
        /* anyway                                                                */
        /*************************************************************************/
        if (pending_th[num_pend_th - 1] != lthisthread)
        {
            dbg_printf("Kill thread ID %08X\n", pending_th[num_pend_th - 1]);
            sceKernelDeleteThread(pending_th[num_pend_th - 1]);
        }
        else
        {
            dbg_printf("Not killing myself - yet\n");
        }
        num_pend_th--;
    }

#ifdef DELETE_EXIT_THREADS
    dbg_printf("Sleeping Threads cleanup\n");
    /***************************************************************************/
    /* Delete the threads that exitted but haven't been deleted yet            */
    /***************************************************************************/
    while (num_exit_th > 0)
    {
        dbg_printf("%d exited threads remain\n", num_exit_th);
        if (exited_th[num_exit_th - 1] != lthisthread)
        {
            dbg_printf("Delete thread ID %08X\n", exited_th[num_exit_th - 1]);
            sceKernelDeleteThread(exited_th[num_exit_th - 1]);
        }
        else
        {
            dbg_printf("Not killing myself - yet\n");
        }
        num_exit_th--;
    }
#endif

    sceKernelSignalSema(globals->thSema, 1);
    dbg_printf("Threads cleanup Done\n");
}

//
// File I/O manager Hooks
//
// returns 1 if a string is an absolute file path, 0 otherwise
static int path_is_absolute(const char *path)
{
	if (path != NULL)
		while (*path != '/' && *path != '\0') {
			if (*path == ':')
				return 1;
			path++;
		}

	return 0;
}

// converts a relative path to an absolute one based on cwd
// returns the length of resolved_path
static int realpath(const char *path, char *resolved_path)
{
	const char *src;
	int i = 0;

	if (path == NULL || resolved_path == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	if (!path_is_absolute(path))
		for (src = mod_chdir; *src; src++) {
			resolved_path[i] = *src;
			i++;
		}

	while (i < PATH_MAX) {
		if (resolved_path[i - 1] == '/') {
			if (path[0] == '/') {
				path++;
				continue;
			} else if (path[0] == '.') {
				if (!path[1])
					path++;
				else if (path[1] == '/') {
					path++;
					continue;
				} else if (path[1] == '.' && (path[2] == '/' || !path[3])) {
					if (resolved_path[i - 1] == ':')
						return SCE_KERNEL_ERROR_NOFILE;
					else {
						i -= 2;
						while (resolved_path[i] != '/')
							i--;
					}
					path += 2;
					continue;
				}
			}
		}

		resolved_path[i] = path[0];
		i++;
		if (!path[0])
			return i;
		path++;
	}

	return SCE_KERNEL_ERROR_NAMETOOLONG;
}

//hook this ONLY if test_sceIoChdir() fails!
SceUID _hook_sceIoDopen(const char *dirname)
{
	char resolved_path[PATH_MAX];
	int ret;

	if (dirname == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	if (!path_is_absolute(dirname)) {
		ret = realpath(dirname, resolved_path);
		if (ret < 0)
			return ret;
		dirname = resolved_path;
	}
	return globals->isEmu ?
		sceIoDopen_Vita(resolved_path) :
		sceIoDopen(resolved_path);
}


// Adds Vita's missing "." / ".." entries

SceUID sceIoDopen_Vita(const char *dirname)
{
    	dbg_printf("sceIoDopen_Vita start\n");
    SceUID id = sceIoDopen(dirname);

	if (id >= 0)
	{
		int dirname_len = strlen(dirname);

		// If directory isn't "ms0:" or "ms0:/", add "." & ".." entries
		if (dirname[dirname_len - 1] != ':' && dirname[dirname_len - 2] != ':')
		{
			int i = 0;

			while (i < dirLen && globals->dirFix[i][0] >= 0)
				++i;

			if (i < MAX_OPEN_DIR_VITA)
			{
				globals->dirFix[i][0] = id;
				globals->dirFix[i][1] = 2;

				if (i == dirLen)
					dirLen++;
			}
		}

	}


	dbg_printf("sceIoDopen_Vita Done\n");
    return id;
}


int sceIoDread_Vita(SceUID id, SceIoDirent *dir)
{
    	dbg_printf("sceIoDread_Vita start\n");
	int i = 0, errCode = 1;
	while (i < dirLen && id != globals->dirFix[i][0])
		++i;


	if (id == globals->dirFix[i][0])
	{
        memset(dir, 0, sizeof(SceIoDirent));
		if (globals->dirFix[i][1] == 1)
		{
			strcpy(dir->d_name,"..");
			dir->d_stat.st_attr |= FIO_SO_IFDIR;
			dir->d_stat.st_mode |= FIO_S_IFDIR;
		}
		else if (globals->dirFix[i][1] == 2)
		{
			strcpy(dir->d_name,".");
			dir->d_stat.st_attr |= FIO_SO_IFDIR;
			dir->d_stat.st_mode |= FIO_S_IFDIR;
		}
		globals->dirFix[i][1]--;

		if (globals->dirFix[i][1] == 0)
		{
			globals->dirFix[i][0] = -1;
			globals->dirFix[i][1] = -1;
		}
	}
	else
	{
		errCode = sceIoDread(id, dir);
	}

	dbg_printf("sceIoDread_Vita Done\n");
	return errCode;
}

int sceIoDclose_Vita(SceUID id)
{
    	dbg_printf("sceIoDclose_Vita start\n");
	int i = 0;
	while (i < dirLen && id != globals->dirFix[i][0] )
		++i;


	if (i < dirLen && id == globals->dirFix[i][0])
	{
			globals->dirFix[i][0] = -1;
			globals->dirFix[i][1] = -1;
	}

	dbg_printf("sceIoDclose_Vita Done\n");
	return sceIoDclose(id);
}

static int _hook_sceIoMkdir(const char *dir, SceMode mode)
{
	int ret;
	char resolved[PATH_MAX];

	if (globals->isEmu || !path_is_absolute(dir)) {
		ret = realpath(dir, resolved);
		if (ret < 0)
			return ret;
		dir = resolved;

		if (globals->isEmu &&!strcasecmp("ms0:/PSP/GAME", resolved)) {
			resolved[5] = 'Q';

			ret = sceIoRename("ms0:/PSP.", "ms0:/QSP");
			if (ret)
				return ret;

			ret = sceIoMkdir(resolved, mode);

			while (sceIoRename("ms0:/QSP", "ms0:/PSP."));

			return ret;
		}
	}

	return sceIoMkdir(dir, mode);
}

static int _hook_sceIoRename(const char *oldname, const char *newname)
{
	char oldResolved[PATH_MAX];
	char newResolved[PATH_MAX];
	int oldLen, i, ret;

	if (globals->isEmu || !path_is_absolute(oldname)) {
		oldLen = realpath(oldname, oldResolved);
		if (oldLen < 0)
			return oldLen;
		oldname = oldResolved;

		if (globals->isEmu) {
			if (oldLen < PATH_MAX
				&& !strcasecmp(oldResolved + oldLen -  8, "BOOT.PBP")
				&& (oldResolved[oldLen - 9] == 'E'
					|| oldResolved[oldLen - 9] == 'e'
					|| oldResolved[oldLen - 9] == 'P'
					|| oldResolved[oldLen - 9] == 'p')) {
				oldResolved[oldLen] = '.';
				oldResolved[oldLen + 1] = 0;
			}

			for (i = 0; i < PATH_MAX - 1; i++) {
				if (!newname[i]) {
					if (!strcasecmp(newResolved + i -  8, "BOOT.PBP")
						&& (newResolved[i - 9] == 'E'
							|| newResolved[i - 9] == 'e'
							|| newResolved[i - 9] == 'P'
							|| newResolved[i - 9] == 'p')) {
						newResolved[i] = '.';
						newResolved[i + 1] = 0;
						newname = newResolved;
					}
					break;
				}
				newResolved[i] = newname[i];
			}

			if (!strcasecmp("ms0:/PSP/GAME", oldResolved)) {
				oldResolved[5] = 'Q';

				ret = sceIoRename("ms0:/PSP.", "ms0:/QSP");
				if (ret)
					return ret;

				ret = sceIoRename(oldResolved, newname);

				while (sceIoRename("ms0:/QSP", "ms0:/PSP."));

				return ret;
			} else if (!strcasecmp(oldResolved, "ms0:/PSP")) {
				oldResolved[oldLen] = '.';
				oldResolved[oldLen + 1] = 0;
			}
		}
	}

	return sceIoRename(oldname, newname);
}

static int _hook_sceIoRemove(const char *file)
{
	int ret;
	char resolved[PATH_MAX];

	if (globals->isEmu || !path_is_absolute(file)) {
		ret = realpath(file, resolved);
		if (ret < 0)
			return ret;
		file = resolved;

		if (globals->isEmu) {
			if (ret < PATH_MAX
				&& !strcasecmp(resolved + ret -  8, "BOOT.PBP")
				&& (resolved[ret - 9] == 'E'
					|| resolved[ret - 9] == 'e'
					|| resolved[ret - 9] == 'P'
					|| resolved[ret - 9] == 'p')) {
				resolved[ret] = '.';
				resolved[ret + 1] = 0;
			}

			if (!strcasecmp("ms0:/PSP/GAME", file)) {
				resolved[5] = 'Q';

				ret = sceIoRename("ms0:/PSP.", "ms0:/QSP");
				if (ret)
					return ret;

				ret = sceIoRemove(resolved);

				while (sceIoRename("ms0:/QSP", "ms0:/PSP."));

				return ret;
			}
		}
	}
	return sceIoRemove(file);
}


//hook this ONLY if test_sceIoChdir() fails!
int _hook_sceIoChdir(const char *dirname)
{
	char resolved_path[PATH_MAX];
	int ret;

	dbg_printf("_hook_sceIoChdir start\n");

	ret = realpath(dirname, resolved_path);
	if (ret < 0)
		return ret;
	else if (ret >= PATH_MAX)
		return SCE_KERNEL_ERROR_NAMETOOLONG;

        // save chDir into global variable
	strcpy(mod_chdir, resolved_path);
	if (mod_chdir[ret - 1] != '/') {
		mod_chdir[ret] = '/';
		mod_chdir[ret + 1] = '\0';
	}

	dbg_printf("_hook_sceIoChdir: %s becomes %s\n", dirname, mod_chdir);
	return 0;
}

static SceUID _hook_sceIoOpen(const char *file, int flags, SceMode mode)
{
	SceUID ret;
	unsigned i;
	char resolved[PATH_MAX];

	if (globals->isEmu || (!globals->chdir_ok && !path_is_absolute(file))) {
		ret = realpath(file, resolved);
		if (ret < 0)
			return ret;

		if (globals->isEmu && ret < PATH_MAX
			&& mode & PSP_O_WRONLY
			&& !strcasecmp(resolved + ret -  8, "BOOT.PBP")
			&& (resolved[ret - 9] == 'E'
				|| resolved[ret - 9] == 'e'
				|| resolved[ret - 9] == 'P'
				|| resolved[ret - 9] == 'p')) {
			resolved[ret] = '.';
			resolved[ret + 1] = 0;
		}

		dbg_printf("sceIoOpen override: %s become %s\n", file, resolved);
		file = resolved;
	}

	ret = sceIoOpen(file, flags, mode);

	if (ret >= 0) {
		WAIT_SEMA(globals->ioSema, 1, 0);

		if (numOpenFiles < sizeof(openFiles) / sizeof(SceUID) - 1)
			for (i = 0; i < sizeof(openFiles) / sizeof(SceUID); i++)
				if (openFiles[i] == 0) {
					openFiles[i] = ret;
					numOpenFiles++;
					break;
				}
		else
			dbg_printf("WARNING: file list full, cannot add newly opened file\n");

		sceKernelSignalSema(globals->ioSema, 1);
	}

	return ret;
}


static int _hook_sceIoClose(SceUID fd)
{
	SceUID ret;
	unsigned i;
    
	ret = sceIoClose(fd);

	if (!ret) {
		WAIT_SEMA(globals->ioSema, 1, 0);

		for (i = 0; i < sizeof(openFiles) / sizeof(SceUID); i++) {
			if (openFiles[i] == fd) {
				openFiles[i] = 0;
				numOpenFiles--;
				break;
			}
		}

		sceKernelSignalSema(globals->ioSema, 1);
	}

	return ret;
}


// Close all files that remained open after the homebrew quits
void files_cleanup()
{
	unsigned i;

	dbg_printf("Files Cleanup\n");
    
	WAIT_SEMA(globals->ioSema, 1, 0);
	for (i = 0; i < sizeof(openFiles) / sizeof(SceUID); i++)
	{
		if (openFiles[i] != 0)
		{
			sceIoClose(openFiles[i]);
			dbg_printf("closing file UID 0x%08X\n", openFiles[i]);
			openFiles[i] = 0;
		}
	}

	numOpenFiles = 0;
	sceKernelSignalSema(globals->ioSema, 1);
	dbg_printf("Files Cleanup Done\n");
}


void net_term()
{
	while (net_term_num > 0) {
		net_term_num--;
		net_term_func[net_term_num]();
	}
}

// Release the kernel audio channel
static void audio_term()
{
	// sceAudioSRCChRelease
#ifdef HOOK_AUDIOFUNCTIONS
	_hook_sceAudioSRCChRelease();
#else
	sceAudioSRCChRelease();
#endif
}

void ram_cleanup()
{
        unsigned i;

	dbg_printf("Ram Cleanup\n");

	WAIT_SEMA(globals->memSema, 1, 0);
	for (i = 0; i < osAllocNum; i++)
		sceKernelFreePartitionMemory(osAllocs[i]);
	osAllocNum = 0;
	sceKernelSignalSema(globals->memSema, 1);

	dbg_printf("Ram Cleanup Done\n");
}


void exit_everything_but_me()
{
	net_term();
	audio_term();
#ifdef SUB_INTR_HANDLER_CLEANUP
	subinterrupthandler_cleanup();
#endif
    threads_cleanup();
    ram_cleanup();
	files_cleanup();
}

//To return to the menu instead of exiting the game
void  _hook_sceKernelExitGame()
{
    /***************************************************************************/
    /* Call any exit callback first. Or not, it should only be called when     */
	/* quitting through the HOME exit screen.                                  */
    /***************************************************************************/
/*
	    if (!hook_exit_cb_called && hook_exit_cb)
    {
        dbg_printf("Call exit CB: %08X\n", hook_exit_cb);
        hook_exit_cb_called = 1;
        hook_exit_cb(0,0,NULL);
    }
*/
	dbg_printf("_hook_sceKernelExitGame called\n");
    exit_everything_but_me();
    sceKernelExitDeleteThread(0);
}

SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr)
{
        dbg_printf("call to sceKernelAllocPartitionMemory partitionId: %d, name: %s, type:%d, size:%d, addr:0x%08X\n", partitionid, (u32)name, type, size, (u32)addr);
    WAIT_SEMA(globals->memSema, 1, 0);

	// Try to allocate the requested memory. If the allocation fails due to an insufficient
	// amount of free memory try again with 10kB less until the allocation succeeds.
	// Don't allow to go under 80 % of the initial amount of memory.
	SceUID uid;
	SceSize original_size = size;
	do
	{
		uid = sceKernelAllocPartitionMemory(partitionid,name, type, size, addr);
		if (uid <= 0)
		{
			// Allocation failed, check if we can go lower for another try
			if ((size > 10000) && ((size - 10000) > original_size - (original_size / 5)))
			{
				// Try again with 10kB less
				size -= 10000;
			}
			else
			{
				// Limit reached, break out of loop
				break;
			}
		}
	}
    while (uid <= 0);

	dbg_printf("-> final allocation made for %d of %d requested bytes with result 0x%08X\n", size, original_size, uid);

	if (uid > 0)
	{
        /***********************************************************************/
        /* Succeeded OS alloc.  Record the block ID in the tracking list.      */
        /* (Don't worry if there's no space to record it, we'll just have to   */
        /* leak it).                                                           */
        /***********************************************************************/
        if (osAllocNum < sizeof(osAllocs) / sizeof(SceUID))
        {
            osAllocs[osAllocNum] = uid;
            osAllocNum ++;
            dbg_printf("Num tracked OS blocks now: %08X\n", osAllocNum);
        }
        else
        {
            dbg_printf("!!! EXCEEDED OS ALLOC TRACKING ARRAY\n");
        }
    }
    sceKernelSignalSema(globals->memSema, 1);
    return uid;
}

int _hook_sceKernelFreePartitionMemory(SceUID blockid)
{
	int ret;
	unsigned i;
	int found = 0;

	WAIT_SEMA(globals->memSema, 1, 0);
	ret = sceKernelFreePartitionMemory(blockid);

	/*************************************************************************/
	/* Remove UID from list of alloc'd mem.                                  */
	/*************************************************************************/
	for (i = 0; i < osAllocNum; i++) {
		if (osAllocs[i] == blockid)
			found = 1;

		if (found && i < sizeof(osAllocs) / sizeof(SceUID) - 2)
			osAllocs[i] = osAllocs[i + 1];
	}

	if (found)
		osAllocNum--;

	sceKernelSignalSema(globals->memSema, 1);

	return ret;

}


/*****************************************************************************/
/* Create a callback.  Record the details so that we can refer to it if      */
/* need be.                                                                  */
/*****************************************************************************/
int _hook_sceKernelCreateCallback(const char *name, SceKernelCallbackFunction func, void *arg)
{
        int lrc = sceKernelCreateCallback(name, func, arg);

    dbg_printf("Enter createcallback: %s\n", (u32)name);

    WAIT_SEMA(globals->cbSema, 1, 0);
    if (cbcount < MAX_CALLBACKS)
    {
        cbids[cbcount] = lrc;
        cbfuncs[cbcount] = func;
        cbcount ++;
    }
    sceKernelSignalSema(globals->cbSema, 1);

    dbg_printf("Exit createcallback: %s ID: %08X\n", (u32) name, lrc);

    return(lrc);
}

/*****************************************************************************/
/* Register an ExitGame handler                                              */
/*****************************************************************************/
int _hook_sceKernelRegisterExitCallback(int cbid)
{
        int i;

    dbg_printf("Enter registerexitCB: %08X\n", cbid);

    WAIT_SEMA(globals->cbSema, 1, 0);
    for (i = 0; i < cbcount; i++)
    {
        if (cbids[i] == cbid)
        {
            dbg_printf("Found matching CB, func: %08X\n", cbfuncs[i]);
            hook_exit_cb = cbfuncs[i];
            break;
        }
    }
    sceKernelSignalSema(globals->cbSema, 1);

    dbg_printf("Exit registerexitCB: %08X\n",cbid);

    return(0);
}


//#############
// RTC
// #############

#define ONE_SECOND_TICK 1000000
#define ONE_DAY_TICK 24 * 3600 * ONE_SECOND_TICK

// always returns 1000000
// based on http://forums.ps2dev.org/viewtopic.php?t=4821
u32 _hook_sceRtcGetTickResolution()
{
    return ONE_SECOND_TICK;
}


int _hook_sceRtcSetTick (pspTime  *time, const u64  *t)
{
//super approximate, let's hope people call this only for display purposes
    u32 tick = *t; //get the lower 32 bits
    time->year = 2097; //I know...
    tick = tick % (365 * (u32)ONE_DAY_TICK);
    time->month = tick / (30 * (u32)ONE_DAY_TICK);
    tick = tick % (30 * (u32)ONE_DAY_TICK);
    time->day = tick / ONE_DAY_TICK;
    tick = tick % ONE_DAY_TICK;
    time->hour = tick / (3600 * (u32)ONE_SECOND_TICK);
    tick = tick % (3600 * (u32)ONE_SECOND_TICK);
    time->minutes = tick / (60 * ONE_SECOND_TICK);
    tick = tick % (60 * ONE_SECOND_TICK);
    time->seconds = tick / (ONE_SECOND_TICK);
    tick = tick % (ONE_SECOND_TICK);
    time->microseconds = tick;
    return 0;
}

//dirty, assume we're utc
int _hook_sceRtcConvertUtcToLocalTime  (const u64  *tickUTC, u64  *tickLocal)
{
  *tickLocal = *tickUTC;
  return 1;
}

int _hook_sceRtcGetTick  (const pspTime  *time, u64  *tick)
{
    u64 seconds =
        (u64) time->day * 24 * 3600
        + (u64) time->hour * 3600
        + (u64) time->minutes * 60
        + (u64) time->seconds;
    *tick = seconds * 1000000   + (u64) time->microseconds;
    return 0;
}

#ifdef HOOK_sceRtcGetCurrentTick
int _hook_sceRtcGetCurrentTick(u64 * tick)
{
    pspTime time;
    sceRtcGetCurrentClockLocalTime(&time);
    return _hook_sceRtcGetTick (&time, tick);
}
#endif

//based on http://forums.ps2dev.org/viewtopic.php?t=12490
int _hook_sceIoLseek32 (SceUID  fd, int offset, int whence)
{
    return sceIoLseek(fd, offset, whence);
}


//audio hooks

int _hook_sceAudioChReserve(int channel, int samplecount, int format)
{
	int lreturn = sceAudioChReserve(channel,samplecount,format);

#ifdef MONITOR_AUDIO_THREADS
    	if (lreturn >= 0)
	{
	    if (lreturn >= 8)
		{
			dbg_printf("AudioChReserve return out of range: %08X\n", lreturn);
			//waitForButtons();
		}
		else
		{
			WAIT_SEMA(globals->audioSema, 1, 0);
			audio_th[lreturn] = sceKernelGetThreadId();
			sceKernelSignalSema(globals->audioSema, 1);
		}
	}
#endif

	return lreturn;
}

// see commented code in http://forums.ps2dev.org/viewtopic.php?p=81473
// //   int audio_channel = sceAudioChReserve(0, 2048 , PSP_AUDIO_FORMAT_STEREO);
//   sceAudioSRCChReserve(2048, wma_samplerate, 2);
int _hook_sceAudioSRCChReserve (int samplecount, int UNUSED(freq), int channels)
{

    int format = PSP_AUDIO_FORMAT_STEREO;
    if (channels == 1)
        format = PSP_AUDIO_FORMAT_MONO;
    //samplecount needs to be 64 bytes aligned, see http://code.google.com/p/valentine-hbl/issues/detail?id=270
    int result =  _hook_sceAudioChReserve (PSP_AUDIO_NEXT_CHANNEL, PSP_AUDIO_SAMPLE_ALIGN(samplecount), format);
    if (result >=0 && result < 8)
    {
                cur_ch_id = result;
    }
    return result;
}

//         sceAudioSRCOutputBlocking(PSP_AUDIO_VOLUME_MAX, wma_output_buffer[wma_output_index]);
//   //      sceAudioOutputBlocking(audio_channel, PSP_AUDIO_VOLUME_MAX, wma_output_buffer[wma_output_index]);
int _hook_sceAudioSRCOutputBlocking (int vol,void * buf)
{
    #ifdef HOOK_sceAudioOutputPannedBlocking_WITH_sceAudioOutputBlocking
    return sceAudioOutputBlocking(cur_ch_id,vol, buf);
#else
    return sceAudioOutputPannedBlocking(cur_ch_id,vol, vol, buf);
#endif
}

#ifdef HOOK_sceAudioOutputBlocking_WITH_sceAudioOutputPannedBlocking
//quite straightforward
int _hook_sceAudioOutputBlocking(int channel,int vol,void * buf)
{
    return sceAudioOutputPannedBlocking(channel,vol, vol, buf);
}
#endif

#ifdef HOOK_sceAudioOutputPannedBlocking_WITH_sceAudioOutputBlocking
//quite straightforward
int _hook_sceAudioOutputPannedBlocking(int channel, int vol, int UNUSED(vol2), void * buf)
{
    return sceAudioOutputBlocking(channel,vol, buf);
}
#endif


// Ditlew
int _hook_sceAudioChRelease(int channel)
{
	int lreturn;

	lreturn = sceAudioChRelease( channel );

#ifdef MONITOR_AUDIO_THREADS
	if (lreturn >= 0)
	{
        		WAIT_SEMA(globals->audioSema, 1, 0);
		audio_th[channel] = 0;
		sceKernelSignalSema(globals->audioSema, 1);
	}
#endif
	return lreturn;
}

//
int _hook_sceAudioSRCChRelease()
{
        if (cur_ch_id < 0 || cur_ch_id > 7)
    {
        dbg_printf("FATAL: cur_ch_id < 0 in _hook_sceAudioSRCChRelease\n");
        return SCE_KERNEL_ERROR_ERROR;
    }
    int result = _hook_sceAudioChRelease(cur_ch_id);
    cur_ch_id--;
    return result;
}

// see http://forums.ps2dev.org/viewtopic.php?p=52329
int _hook_scePowerGetCpuClockFrequencyInt()
{
        if (!cur_cpufreq)
    {
        //that's a bit dirty :(
        _hook_scePowerSetClockFrequency(333, 333, 166);
    }
    return cur_cpufreq;
}

int _hook_scePowerGetBusClockFrequency() {
        if (!cur_busfreq)
    {
        //that's a bit dirty :(
        _hook_scePowerSetClockFrequency(333, 333, 166);
    }
    return cur_busfreq;
}


//Alias, see http://forums.ps2dev.org/viewtopic.php?t=11294
int _hook_scePowerSetClockFrequency(int pllfreq, int cpufreq, int busfreq)
{
#if defined(HOOK_scePowerSetClockFrequency_WITH_scePower_EBD177D6)
    int ret = scePower_EBD177D6(pllfreq, cpufreq, busfreq);
#elif defined(HOOK_scePowerSetClockFrequency_WITH_scePower_469989AD)
    int ret = scePower_469989AD(pllfreq, cpufreq, busfreq);
#else
    int ret = 0;
#endif
    if (ret >= 0)
    {
                pllfreq = pllfreq;
        cpufreq = cpufreq;
        busfreq = busfreq;
    }

    return ret;
}

//Dirty
int _hook_scePowerGetBatteryLifeTime()
{
    return 60;
}

//Dirty
int _hook_scePowerGetBatteryLifePercent()
{
    return 50;
}

// http://forums.ps2dev.org/viewtopic.php?p=43495
int _hook_sceKernelDevkitVersion()
{
	return globals->module_sdk_version;
}

#ifdef HOOK_sceKernelSelfStopUnloadModule_WITH_ModuleMgrForUser_8F2DF740
// see http://powermgrprx.googlecode.com/svn-history/r2/trunk/include/pspmodulemgr.h
int _hook_sceKernelSelfStopUnloadModule  (int exitcode, SceSize  argsize, void *argp)
{
    int status;
    return ModuleMgrForUser_8F2DF740(exitcode, argsize, argp, &status, NULL);

}
#endif

int _hook_sceKernelGetThreadCurrentPriority()
{
    return 0x18;
    //TODO : real management of threads --> keep track of their priorities ?
}

// Sleep is not delay... can we fix this?
#ifdef HOOK_sceKernelSleepThreadCB_WITH_sceKernelDelayThreadCB
int _hook_sceKernelSleepThreadCB()
{
    while (1)
        sceKernelDelayThreadCB(1000000);
    return 1;
}
#endif

#ifdef HOOK_sceKernelTrySendMsgPipe_WITH_sceKernelSendMsgPipe
int _hook_sceKernelTrySendMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2)
{
    return sceKernelSendMsgPipe(uid, message, size, unk1, unk2, 0);
}
#endif

#ifdef HOOK_sceKernelTryReceiveMsgPipe_WITH_sceKernelReceiveMsgPipe
int _hook_sceKernelTryReceiveMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2)
{
    return sceKernelReceiveMsgPipe(uid, message, size, unk1, unk2, 0);
}
#endif

#ifdef HOOK_sceKernelReceiveMsgPipe_WITH_sceKernelTryReceiveMsgPipe
int _hook_sceKernelReceiveMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2, int UNUSED(timeout))
{
    return sceKernelTryReceiveMsgPipe(uid, message, size, unk1, unk2);
}
#endif

//
// Cache
//
#ifdef HOOK_sceKernelDcacheWritebackAll_WITH_sceKernelDcacheWritebackRange
void _hook_sceKernelDcacheWritebackAll (void)
{
     sceKernelDcacheWritebackRange((void*)0x08800000, 0x17FFFFF);
}
#endif

#ifdef HOOK_sceKernelDcacheWritebackInvalidateAll_WITH_sceKernelDcacheWritebackInvalidateRange
void _hook_sceKernelDcacheWritebackInvalidateAll (void)
{
     sceKernelDcacheWritebackInvalidateRange((void*)0x08800000, 0x17FFFFF);
}
#endif


// ###############
// UtilsForUser
// ###############
SceUID _hook_sceKernelLoadModule(const char *path, int UNUSED(flags), SceKernelLMOption * UNUSED(option))
{
	dbg_printf("_hook_sceKernelLoadModule\n");
	dbg_printf("Attempting to load %s\n", (u32)path);

	SceUID elf_file = _hook_sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (elf_file < 0)
	{
		dbg_printf("Error 0x%08X opening requested module %s\n", elf_file, (u32)path);
		return elf_file;
	}

	SceOff offset = 0;
	dbg_printf("_hook_sceKernelLoadModule offset: 0x%08X\n", offset);
	SceUID ret = load_module(elf_file, path, NULL, offset);
    sceIoClose(elf_file);
	dbg_printf("load_module returned 0x%08X\n", ret);

	return ret;
}

int	_hook_sceKernelStartModule(SceUID modid, SceSize UNUSED(argsize), void *UNUSED(argp), int *UNUSED(status), SceKernelSMOption *UNUSED(option))
{
	SceUID ret;
	int gp;

	dbg_printf("_hook_sceKernelStartModule\n");

	__asm__("sw $gp, %0" : "=m" (gp));

	ret = start_module(modid);
	dbg_printf("start_module returned 0x%08X\n", ret);

	__asm__("lw $gp, %0" :: "m" (gp));

	return ret;
}

#ifdef HOOK_UTILITY

int _hook_sceUtilityLoadModule(int id)
{
	return id == PSP_MODULE_AV_MPEGBASE
		|| id == PSP_MODULE_AV_ATRAC3PLUS
		|| id == PSP_MODULE_AV_MP3
		|| id == PSP_MODULE_NET_INET
		|| id == PSP_MODULE_NET_COMMON
		|| id == PSP_MODULE_NET_ADHOC
		|| id == PSP_MODULE_NET_HTTP ?
		0 :  SCE_KERNEL_ERROR_ERROR;
}

int _hook_sceUtilityLoadNetModule(int id)
{
	if ((id == PSP_NET_MODULE_COMMON)
		|| (id == PSP_NET_MODULE_INET)
		|| (id == PSP_NET_MODULE_ADHOC))
		return 0;

	return SCE_KERNEL_ERROR_ERROR;
}

int _hook_sceUtilityLoadAvModule(int id)
{
	if (id == PSP_AV_MODULE_MP3
		|| id == PSP_AV_MODULE_AVCODEC
		|| id == PSP_AV_MODULE_ATRAC3PLUS
		|| id == PSP_AV_MODULE_MPEGBASE)
		return 0;
    return SCE_KERNEL_ERROR_ERROR;
}

#endif

// A function that just returns "ok" but does nothing
// Note: in Sony's world, "0" means ok
int _hook_generic_ok()
{
    return 0;
}

// A function that return a generic error
int _hook_generic_error()
{
    return SCE_KERNEL_ERROR_ERROR;
}

#ifdef HOOK_sceKernelGetThreadId
int _hook_sceKernelGetThreadId()
{
	// Somehow sceKernelGetThreadId isn't imported by the game,
	// and can be called from HBL but not from a homebrew
    // This is because HBL accepts both jumps and syscalls, while the nid table for games only accepts syscalls
    return sceKernelGetThreadId();
}
#endif

#ifdef HOOK_sceAudioOutput2GetRestSample_WITH_sceAudioGetChannelRestLength
//	sceAudioOutput2GetRestSample lame hook (hopefully it's not super necessary)
int _hook_sceAudioOutput2GetRestSample()
{
	int sum = 0, i, res;

	for (i=0;i<PSP_AUDIO_CHANNEL_MAX;++i)
	{
		res = sceAudioGetChannelRestLength(i);
		if (res >=0)	sum+= res;
	}


    return sum;
}
#endif


#ifdef HOOK_mersenne_twister_rdm
u32 rdm_seed;

int _hook_sceKernelUtilsMt19937Init(SceKernelUtilsMt19937Context *ctx, u32 seed)
{
	int i;
	rdm_seed = seed;

    for (i = 0; i < 624; i++)
        _hook_sceKernelUtilsMt19937UInt(ctx);

	return 0;
}

u32 _hook_sceKernelUtilsMt19937UInt(SceKernelUtilsMt19937Context UNUSED(*ctx))
{
	// (Bill's generator)
	rdm_seed = (((rdm_seed * 214013L + 2531011L) >> 16) & 0xFFFF);
	rdm_seed |= ((rdm_seed * 214013L + 2531011L) & 0xFFFF0000);

	return rdm_seed;
}
#endif


#ifdef HOOK_sceKernelTotalFreeMemSize
// return 10 MB
SceSize _hook_sceKernelTotalFreeMemSize()
{
	return sceKernelTotalFreeMemSize(); //this is the function we hardcoded in memory.c, not the official one
}
#endif


#ifdef HOOK_sceKernelReferThreadStatus
int _hook_sceKernelReferThreadStatus(SceUID thid, SceKernelThreadInfo UNUSED(*info))
{
	memset(info+sizeof(SceSize), 0, info->size-sizeof(SceSize));

	return 0;
}
#endif

#ifdef HOOK_sceDisplayGetFrameBuf
static void *frame_topaddr[2];
static int frame_bufferwidth[2], frame_pixelformat[2];
int _hook_sceDisplaySetFrameBuf(void *topaddr,int bufferwidth, int pixelformat,int sync)
{
	frame_topaddr[ sync ] = topaddr;
	frame_bufferwidth[ sync ] = bufferwidth;
	frame_pixelformat[ sync ] = pixelformat;

	return sceDisplaySetFrameBuf( topaddr, bufferwidth, pixelformat, sync);
}

int _hook_sceDisplayGetFrameBuf(void **topaddr, int *bufferwidth, int *pixelformat, int sync)
{
	*topaddr = frame_topaddr[ sync ];
	*bufferwidth = frame_bufferwidth[ sync ];
	*pixelformat = frame_pixelformat[ sync ];
	return 0;
}
#endif

#ifdef HOOK_Osk
int _hook_sceUtilityOskInitStart (SceUtilityOskParams *params)
{
	if ( *((char*)params->data->outtext) == 0 )
	{
		if (params->data->outtextlimit > 3)
		{
			params->data->outtext[0] = 'L';
			params->data->outtext[1] = '_';
			params->data->outtext[2] = 'L';
			params->data->outtext[3] = 0;
		}
		else
		{
			params->data->outtext[0] = 'L';
			params->data->outtext[1] = 0;
		}

		/*	memset(oskData->outtext, 'L', oskData->outtextlimit);
			*((char*) (oskData->outtext+oskData->outtextlimit-1) ) = 0;*/
	}

	return 0;
}

#endif



// Returns a hooked call for the given NID or zero
u32 setup_hook(u32 nid, u32 existing_real_call)
{
	switch (nid) {
		case 0x237DBD4F:
			return J_ASM(_hook_sceKernelAllocPartitionMemory);
		case 0xB6D61D02:
			return J_ASM(_hook_sceKernelFreePartitionMemory);
		case 0x446D8DE6:
			return J_ASM(_hook_sceKernelCreateThread);
		case 0xF475845D:
			return J_ASM(_hook_sceKernelStartThread);
 		case 0xAA73C935:
			return J_ASM(_hook_sceKernelExitThread);
 		case 0x809CE29B:
			return J_ASM(_hook_sceKernelExitDeleteThread);
		case 0x4C25EA72: //kuKernelLoadModule
		case 0x977DE386: // sceKernelLoadModule
			return J_ASM(_hook_sceKernelLoadModule);
		case 0x50F0C1EC:
			return J_ASM(_hook_sceKernelStartModule);
		case 0xC629AF26:
			return J_ASM(_hook_sceUtilityLoadAvModule);
		case 0x0D5BC6D2:
			dbg_printf(" Hook sceUtilityLoadUsbModule\n");
			return J_ASM(_hook_generic_error);
		case 0x1579a159:
			return J_ASM(_hook_sceUtilityLoadNetModule);
		case 0x2A2B3DE0:
			return J_ASM(_hook_sceUtilityLoadModule);
		case 0xF7D8D092: //sceUtilityUnloadAvModule
		case 0xF64910F0: //sceUtilityUnloadUsbModule
		case 0x64d50c56: //sceUtilityUnloadNetModule
		case 0xE49BFE92: // sceUtilityUnloadModule
			return J_ASM(_hook_generic_ok);
	}

	if (return_to_xmb_on_exit) {
		switch (nid) {
			case 0x05572A5F:
				return J_ASM(_hook_sceKernelExitGame);
			case 0xE81CAF8F:
				return J_ASM(_hook_sceKernelCreateCallback);
			case 0x4AC57943:
				return J_ASM(_hook_sceKernelRegisterExitCallback);
			case 0x6FC46853:
				return J_ASM(_hook_sceAudioChRelease);
			case 0x5EC81C55:
				return J_ASM(_hook_sceAudioChReserve);
		}
	}

	if (existing_real_call) {
		switch (nid) {
			case 0x109F50BC:
				return J_ASM(_hook_sceIoOpen);
			case 0x810C4BC3:
				return J_ASM(_hook_sceIoClose);
		}

		if (globals->isEmu || !globals->chdir_ok) {
			switch (nid) {
				case 0x55F4717D:
					return J_ASM(_hook_sceIoChdir);
				case 0x06A70004:
					return J_ASM(_hook_sceIoMkdir);
				case 0x779103A0:
					return J_ASM(_hook_sceIoRename);
				case 0xF27A9C51:
					return J_ASM(_hook_sceIoRemove);
				case 0xB29DDF9C:
					return J_ASM(_hook_sceIoDopen);
			}

			if (globals->isEmu)
				switch (nid) {
					case 0xE3EB004C:
						return J_ASM(sceIoDread_Vita);
					case 0xEB092469:
						return J_ASM(sceIoDclose_Vita);
				}
		}

		return 0;
	}

	switch (nid) {
#ifdef HOOK_sceDisplayGetFrameBuf
		case 0xEEDA2E54:
			return J_ASM(_hook_sceDisplayGetFrameBuf);
		case 0x289D82FE:
			if (get_nid_index(0xEEDA2E54) < 0)
				return J_ASM(_hook_sceDisplaySetFrameBuf);
#endif
#ifdef HOOK_sceKernelSleepThreadCB_WITH_sceKernelDelayThreadCB
		case 0x82826F70:
			return J_ASM(_hook_sceKernelSleepThreadCB);
#endif
		/*
		* Overrides to avoid syscall estimates.
		* Those are not necessary but reduce estimate failures and improve compatibility for now
		*/
		case 0x06A70004: //	sceIoMkdir
			if (override_sceIoMkdir == GENERIC_SUCCESS)
				return J_ASM(_hook_generic_ok);
			break;
		case 0xC8186A58:
			return J_ASM(_hook_sceKernelUtilsMd5Digest);
		case 0x9E5C5086:
			return J_ASM(_hook_sceKernelUtilsMd5BlockInit);
		case 0x61E1E525:
			return J_ASM(_hook_sceKernelUtilsMd5BlockUpdate);
		case 0xB8D24E78:
			return J_ASM(_hook_sceKernelUtilsMd5BlockResult);
		case 0x3FC9AE6A:
			return J_ASM(_hook_sceKernelDevkitVersion);
		case 0xA291F107:
			return J_ASM(sceKernelMaxFreeMemSize);
		case 0xC41C2853:
			return J_ASM(_hook_sceRtcGetTickResolution);
#ifdef HOOK_sceRtcGetCurrentTick
		case 0x3F7AD767:
			return J_ASM(_hook_sceRtcGetCurrentTick);
#endif
		case 0x7ED29E40:
			return J_ASM(_hook_sceRtcSetTick);
		case 0x6FF40ACC:
			return J_ASM(_hook_sceRtcGetTick);
		case 0x57726BC1: //	sceRtcGetDayOfWeek
			return J_ASM(_hook_generic_ok); //always monday in my world
		case 0x34885E0D:
			return J_ASM(_hook_sceRtcConvertUtcToLocalTime);
		case 0x68963324:
			return J_ASM(_hook_sceIoLseek32);
#ifdef HOOK_sceCtrlPeekBufferPositive_WITH_sceCtrlReadBufferPositive
		//This will be slow and should not be active for high performance programs...
		case 0x3A622550:
			return J_ASM(sceCtrlReadBufferPositive);
#endif
		case 0x737486F2:
			return J_ASM(_hook_scePowerSetClockFrequency);
		case 0x383F7BCC: // sceKernelTerminateDeleteThread
			return J_ASM(kill_thread);
#ifdef HOOK_sceKernelSelfStopUnloadModule_WITH_ModuleMgrForUser_8F2DF740
		case 0xD675EBB8:
			return J_ASM(_hook_sceKernelSelfStopUnloadModule);
#endif
#ifdef HOOK_sceKernelTrySendMsgPipe_WITH_sceKernelSendMsgPipe
		case 0x884C9F90:
			return J_ASM(_hook_sceKernelTrySendMsgPipe);
#endif
#ifdef HOOK_sceKernelTryReceiveMsgPipe_WITH_sceKernelReceiveMsgPipe
		case 0xDF52098F:
			return J_ASM(_hook_sceKernelTryReceiveMsgPipe);
#endif
#ifdef HOOK_sceKernelReceiveMsgPipe_WITH_sceKernelTryReceiveMsgPipe
		case 0x74829B76:
			return J_ASM(_hook_sceKernelReceiveMsgPipe);
#endif
		case 0xD97F94D8: // sceDmacTryMemcpy
			if (get_nid_index(0x617F3FE6) < 0)
				return J_ASM(memcpy);
			else
				return J_ASM(sceDmacMemcpy);
		case 0x94AA61EE:
			return J_ASM(_hook_sceKernelGetThreadCurrentPriority);
		case 0x24331850: // kuKernelGetModel
			return J_ASM(_hook_generic_ok);
#ifdef HOOK_POWERFUNCTIONS
		case 0xFEE03A2F: //scePowerGetCpuClockFrequency
		case 0xFDB5BFE9: //scePowerGetCpuClockFrequencyInt
			return J_ASM(_hook_scePowerGetCpuClockFrequencyInt);
		case 0x478FE6F5:// scePowerGetBusClockFrequency
		case 0xBD681969: //scePowerGetBusClockFrequencyInt
			return J_ASM(_hook_scePowerGetBusClockFrequency);
		case 0x8EFB3FA2:
			return J_ASM(_hook_scePowerGetBatteryLifeTime);
			return J_ASM(_hook_generic_ok);
		case 0x2085D15D:
			return J_ASM(_hook_scePowerGetBatteryLifePercent);
		case 0x28E12023: // scePowerBatteryTemp (0 degree)
		case 0xB4432BC8: // scePowerGetBatteryChargingStatus
		case 0x0AFD0D8B: // scePowerIsBatteryExists
		case 0x1E490401: // scePowerIsbatteryCharging
		case 0xD3075926: // scePowerIsLowBattery
		case 0x87440F5E: // scePowerIsPowerOnline
		case 0x04B7766E: // scePowerRegisterCallback (Assuming it's already done by the game)
		case 0xD6D016EF: // scePowerLock
		case 0xEFD3C963: // scePowerTick
		case 0xCA3D34C1: // scePowerUnlock
			return J_ASM(_hook_generic_ok);
#endif
#ifdef FORCE_HARDCODED_VRAM_SIZE
		case 0x1F6752AD:
			return J_ASM(_hook_sceGeEdramGetSize);
#endif
#ifdef HOOK_AUDIOFUNCTIONS
		case 0x38553111:
			return J_ASM(_hook_sceAudioSRCChReserve);
		case 0x5C37C0AE:
			return J_ASM(_hook_sceAudioSRCChRelease);
		case 0xE0727056:
			return J_ASM(_hook_sceAudioSRCOutputBlocking);
#endif
#ifdef HOOK_sceAudioOutputBlocking_WITH_sceAudioOutputPannedBlocking
		case 0x136CAF51:
			return J_ASM(_hook_sceAudioOutputBlocking);
#endif
#ifdef HOOK_sceAudioOutputPannedBlocking_WITH_sceAudioOutputBlocking
		case 0x13F592BC:
			return J_ASM(_hook_sceAudioOutputPannedBlocking);
#endif
#ifdef HOOK_sceAudioGetChannelRestLength_WITH_dummy
		case 0xb011922f:
			return J_ASM(_hook_generic_ok);
#endif
#ifdef HOOK_sceAudioGetChannelRestLen_WITH_dummy
		case 0xE9D97901:
			return J_ASM(_hook_generic_ok);
#endif
#ifdef HOOK_sceAudioGetChannelRestLen_WITH_sceAudioGetChannelRestLength
		case 0xE9D97901:
			return J_ASM(sceAudioGetChannelRestLength);
#endif
#ifdef HOOK_sceAudioOutput_WITH_sceAudioOutputBlocking
		case 0x8C1009B2:
#ifndef HOOK_sceAudioOutputBlocking_WITH_sceAudioOutputPannedBlocking
			return J_ASM(sceAudioOutputBlocking);
#else
			return J_ASM(_hook_sceAudioOutputBlocking);
#endif
#endif
#ifdef HOOK_sceKernelDcacheWritebackInvalidateAll_WITH_sceKernelDcacheWritebackAll
		case 0xB435DEC5:
			return J_ASM(sceKernelDcacheWritebackAll);
#endif
#ifdef HOOK_sceKernelSendMsgPipe_WITH_sceKernelTrySendMsgPipe
		case 0x876DBFAD:
			return J_ASM(sceKernelTrySendMsgPipe);
#endif
#ifdef HOOK_sceKernelGetThreadId
		case 0x293B45B8:
			return J_ASM(_hook_sceKernelGetThreadId);
#endif
#ifdef HOOK_sceAudioOutput2GetRestSample_WITH_sceAudioGetChannelRestLength
		case 0x647CEF33:
			return J_ASM(_hook_sceAudioOutput2GetRestSample);
#endif
#ifdef HOOK_mersenne_twister_rdm
		case 0xE860E75E:
			return J_ASM(_hook_sceKernelUtilsMt19937Init);
		case 0x06FB8A63:
			return J_ASM(_hook_sceKernelUtilsMt19937UInt);
#endif
#ifdef HOOK_sceDisplayWaitVblankStartCB_WITH_sceDisplayWaitVblankStart
		case 0x46F186C3:
			return J_ASM(sceDisplayWaitVblankStart);
#endif
#ifdef HOOK_sceKernelDcacheWritebackRange_WITH_sceKernelDcacheWritebackAll
		case 0x3EE30821:
			return J_ASM(sceKernelDcacheWritebackAll);
#endif
#ifdef HOOK_sceKernelDcacheWritebackInvalidateRange_WITH_sceKernelDcacheWritebackInvalidateAll
		case 0x34B9FA9E:
			return J_ASM(sceKernelDcacheWritebackInvalidateAll);
#endif
#ifdef HOOK_sceKernelDcacheWritebackAll_WITH_sceKernelDcacheWritebackRange
		case 0x79D1C3FA:
			return J_ASM(_hook_sceKernelDcacheWritebackAll);
#endif
#ifdef HOOK_sceKernelDcacheWritebackInvalidateAll_WITH_sceKernelDcacheWritebackInvalidateRange
		case 0xB435DEC5:
			return J_ASM(_hook_sceKernelDcacheWritebackInvalidateAll);
#endif
#ifdef HOOK_sceAudioOutputPanned_WITH_sceAudioOutputPannedBlocking
		case 0xE2D56B2D:
			return J_ASM(sceAudioOutputPannedBlocking);
#endif
#ifdef HOOK_sceKernelTerminateThread_WITH_sceKernelTerminateDeleteThread
		case 0x616403BA:
			return J_ASM(sceKernelTerminateDeleteThread);
#endif
#ifdef HOOK_sceKernelTotalFreeMemSize
		case 0xF919F628:
			return J_ASM(_hook_sceKernelTotalFreeMemSize);
#endif
#ifdef HOOK_sceDisplayGetVcount_WITH_dummy
		case 0x9C6EAAD7:
			return J_ASM(_hook_generic_ok);
#endif
#ifdef HOOK_Osk
		case 0xF6269B82: // sceUtilityOskInitStart
			return J_ASM(_hook_sceUtilityOskInitStart);
		case 0x3DFAEBA9: // sceUtilityOskShutdownStart
		case 0x4B85C861: // sceUtilityOskUpdate
		case 0xF3F76017: // sceUtilityOskGetStatus
			return J_ASM(_hook_generic_ok);
#endif
	}

	dbg_printf("Missing function: 0x%08X\n\n", nid);
	return 0;
}

/**
*  Setup a Default Nid for Firmware 6.60 where the Kernel Syscall Estimation doesn't work
*  Only ment to give a ok for function that we don't make custom version from
*  By Thecobra
**/
u32 setup_default_nid()
{
	return J_ASM(_hook_generic_error);
}
