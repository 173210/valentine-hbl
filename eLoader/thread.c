#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "scratchpad.h"

// Find a thread by name
SceUID find_thread(const char *name) 
{
	SceUID readbuf[256];
	int idcount, ret;
	SceKernelThreadInfo info;
	unsigned int attempt = 0;

	do
	{
		attempt++;
		ret = sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);
		if (ret < 0)
			reestimate_syscall(*(u32*)ADDR_HBL_STUBS_BLOCK_ADDR + 0x00a8); // sceKernelGetThreadmanIdList
	} while ((ret < 0) && (attempt <= MAX_REESTIMATE_ATTEMPTS));

	if (ret < 0)
		return ret;

	for(info.size=sizeof(info); idcount>0; idcount--)
	{
		attempt = 0;
		do
		{
			attempt++;
			ret = sceKernelReferThreadStatus(readbuf[idcount-1], &info);
			if (ret < 0)
				reestimate_syscall(*(u32*)ADDR_HBL_STUBS_BLOCK_ADDR + 0x0090); // sceKernelReferThreadStatus
		} while ((ret < 0) && (attempt <= MAX_REESTIMATE_ATTEMPTS));
			
		if(ret < 0)
		{
			DEBUG_PRINT(" sceKernelReferThreadStatus FAILED ", &ret, sizeof(ret));
			return ret;
		}
	
		if (strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

// Find a semaphore by name
SceUID find_sema(const char *name) 
{
	SceUID readbuf[256];
	int idcount, ret;
	SceKernelSemaInfo info;
	
	ret = sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Semaphore, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);

	if (ret < 0)
		return ret;
	
	for(info.size=sizeof(info); idcount>0; idcount--)
	{
		ret = sceKernelReferSemaStatus(readbuf[idcount-1], &info);
		if(ret < 0)
		{
			DEBUG_PRINT(" sceKernelReferSemaStatus FAILED ", &ret, sizeof(ret));
			return ret;
		}
		
		if(strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

// Find an event flag by name
SceUID find_evflag(const char *name) 
{
	SceUID readbuf[256];
	int idcount, ret;
	SceKernelEventFlagInfo info;
	
	ret = sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_EventFlag, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);

	if (ret < 0)
		return ret;

	for(info.size=sizeof(info); idcount>0; idcount--)
	{
		ret = sceKernelReferEventFlagStatus(readbuf[idcount-1], &info);
		if(ret < 0)
		{
			DEBUG_PRINT(" sceKernelReferEventFlagStatus FAILED ", &ret, sizeof(ret));
			return ret;
		}
		
		if(strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

/* Dumping code by Fanjita & noobz */
void DumpThreadInfo(SceUID xiid)
{
  int                 lrc;
  SceKernelThreadInfo ti;

  LOGSTR1("Info for thread ID %08lX\n", xiid);

  memset((void*)&ti, 0, sizeof(SceKernelThreadInfo));
  ti.size = sizeof(SceKernelThreadInfo);

  lrc=sceKernelReferThreadStatus(xiid, &ti);
  if (lrc>=0)
  {
    LOGSTR1("  Name:   '%s'\n", ti.name);
    LOGSTR2("  Size:   %08lX (expected: %08lX)\n", ti.size, sizeof(SceKernelThreadInfo));
    LOGSTR1("  Attr:   %08lX\n", ti.attr);
    LOGSTR1("  Status: %08lX\n", ti.status);
    LOGSTR1("  Entry:  %08lX\n", ti.entry);
    LOGSTR1("  Stack:  %08lX\n", ti.stack);
    LOGSTR1("  StkSiz: %08lX\n", ti.stackSize);
    LOGSTR1("  GP:     %08lX\n", ti.gpReg);
    LOGSTR1("  iniPri: %08lX\n", ti.initPriority);
    LOGSTR1("  curPri: %08lX\n", ti.currentPriority);
    LOGSTR1("  waitTy: %08lX\n", ti.waitType);
    LOGSTR1("  waitID: %08lX\n", ti.waitId);
    LOGSTR1("  wake #: %08lX\n", ti.wakeupCount);
    LOGSTR1("  exitSt: %08lX\n", ti.exitStatus);
  }
  else
  {
    LOGSTR1("ThreadStatus failed with %08lX\n", lrc);
  }
}

void DumpThreadmanObjects()
{
  int lidtype;
  int lids[256];
  int lnumids;
  int i,j;
  int lrc;

  LOGSTR0("DUMP THREADMAN OBJECTS:\n");
  LOGSTR0("=======================\n");

  for (lidtype = 1; lidtype < 12; lidtype++)
  {
    lnumids=0;
    sceKernelGetThreadmanIdList(lidtype, lids, sizeof(lids), &lnumids);
    LOGSTR2("Got %d objects of type %d\n", lnumids, lidtype);
    for (i=0; i<lnumids; i++)
    {
      switch (lidtype)
      {
        case 1:
          LOGSTR2("Thread %d has ID %08lX\n", i, lids[i]);
          DumpThreadInfo(lids[i]);
          break;
        case 2:
          LOGSTR2("Semaphore %d has ID %08lX\n", i, lids[i]);
          {
            SceKernelSemaInfo ti;

            memset((void*)&ti, 0, sizeof(SceKernelSemaInfo));
            ti.size = sizeof(SceKernelSemaInfo);

            lrc=sceKernelReferSemaStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name: '%s'\n", ti.name);
              LOGSTR2("  Size: %08lX (expected: %08lX)\n", ti.size, sizeof(SceKernelSemaInfo));
              LOGSTR1("  Attr: %08lX\n", ti.attr);
              LOGSTR1("  init: %08lX\n", ti.initCount);
              LOGSTR1("  curr: %08lX\n", ti.currentCount);
              LOGSTR1("  max:  %08lX\n", ti.maxCount);
              LOGSTR1("  numwait: %d\n", ti.numWaitThreads);
            }
          }
          break;
        case 3:
          LOGSTR2("Event Flag %d has ID %08lX\n", i, lids[i]);
          {
            SceKernelEventFlagInfo ti;

            memset((void*)&ti, 0, sizeof(SceKernelEventFlagInfo));
            ti.size = sizeof(SceKernelEventFlagInfo);

            lrc=sceKernelReferEventFlagStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name: '%s'\n", ti.name);
              LOGSTR1("  Attr: %08lX\n", ti.attr);
              LOGSTR1("  init: %08lX\n", ti.initPattern);
              LOGSTR1("  curr: %08lX\n", ti.currentPattern);
              LOGSTR1("  numwait: %d\n", ti.numWaitThreads);
            }
          }
          break;
        case 4:
          LOGSTR2("Mbox %d has ID %08lX\n", i, lids[i]);
          {
          /* TODO
            SceKernelMbxInfo ti;

            memset((void*)&ti, 0, sizeof(SceKernelMbxInfo));
            ti.size = sizeof(SceKernelMbxInfo);

            lrc=sceKernelReferMbxStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name:  '%s'\n", ti.name);
              LOGSTR1("  Attr:  %08lX\n", ti.attr);
              LOGSTR1("  #Wait: %08lX\n", ti.numWaitThreads);
              LOGSTR1("  #Msg:  %08lX\n", ti.numMessages);
              LOGSTR1("  Head:  %08lX\n", ti.firstMessage);
            }
            */
          }
          break;
        case 5:
          LOGSTR2("Vpl %d has ID %08lX\n", i, lids[i]);
          {
                    /* TODO
            int ti[128];

            memset((void*)&ti, 0, sizeof(int) * 128);
            ti[0] = 128 * sizeof(int);

            lrc=sceKernelReferVplStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name: '%s'\n", &(ti[1]));

              for (j = 0; j < ti[0]; j += 4)
              {
                LOGSTR2("  %08lX :  %08lX\n", j, ((unsigned long*)&ti)[j/4]);
              }
            }
            */
          }
          break;
        case 6:
          LOGSTR2("Fpl %d has ID %08lX\n", i, lids[i]);
          {
            int ti[128];

            memset((void*)&ti, 0, sizeof(int) * 128);
            ti[0] = 128 * sizeof(int);

            lrc=sceKernelReferFplStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name: '%s'\n", &(ti[1]));

              for (j = 0; j < ti[0]; j += 4)
              {
                LOGSTR2("  %08lX :  %08lX\n", j, ((unsigned long*)&ti)[j/4]);
              }
            }
          }
          break;
        case 7:
          LOGSTR2("Mpipe %d has ID %08lX\n", i, lids[i]);
          {
                    /* TODO
            int ti[128];

            memset((void*)&ti, 0, sizeof(int) * 128);
            ti[0] = 128 * sizeof(int);

            lrc=sceKernelReferMsgPipeStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name: '%s'\n", &(ti[1]));

              for (j = 0; j < ti[0]; j += 4)
              {
                LOGSTR2("  %08lX :  %08lX\n", j, ((unsigned long*)&ti)[j/4]);
              }
            }
            */
          }
          break;
        case 8:
          LOGSTR2("Callback %d has ID %08lX\n", i, lids[i]);
          {
                    /* TODO
            SceKernelCallbackInfo ti;

            memset((void*)&ti, 0, sizeof(SceKernelCallbackInfo));
            ti.size = sizeof(SceKernelCallbackInfo);

            lrc=sceKernelReferCallbackStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name:  '%s'\n", ti.name);
              LOGSTR1("  ThrID: %08lX\n", ti.threadId);
              LOGSTR1("  Func:  %08lX\n", ti.callback);
              LOGSTR1("  comm:  %08lX\n", ti.common);
              LOGSTR1("  not #: %08lX\n", ti.notifyCount);
              LOGSTR1("  Arg:   %08lX\n", ti.notifyArg);
            }
            */
          }
          break;
        case 9:
          LOGSTR2("ThreadEventHandler %d has ID %08lX\n", i, lids[i]);
          {
                    /* TODO
            int ti[128];

            memset((void*)&ti, 0, sizeof(int) * 128);
            ti[0] = 128 * sizeof(int);

            lrc=sceKernelReferThreadEventHandlerStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name: '%s'\n", &(ti[1]));

              for (j = 0; j < ti[0]; j += 4)
              {
                LOGSTR2("  %08lX :  %08lX\n", j, ((unsigned long*)&ti)[j/4]);
              }
            }
            */
          }
          break;
        case 10:
          LOGSTR2("Alarm %d has ID %08lX\n", i, lids[i]);
          {
                    /* TODO
            int ti[128];

            memset((void*)&ti, 0, sizeof(int) * 128);
            ti[0] = 128 * sizeof(int);

            lrc=sceKernelReferAlarmStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name: '%s'\n", &(ti[1]));

              for (j = 0; j < ti[0]; j += 4)
              {
                LOGSTR2("  %08lX :  %08lX\n", j, ((unsigned long*)&ti)[j/4]);
              }
            }
            */
          }
          break;
        case 11:
          LOGSTR2("VTimer %d has ID %08lX\n", i, lids[i]);
          {
                    /* TODO
            int ti[128];

            memset((void*)&ti, 0, sizeof(int) * 128);
            ti[0] = 128 * sizeof(int);

            lrc=sceKernelReferVTimerStatus(lids[i], &ti);
            if (lrc>=0)
            {
              LOGSTR1("  Name: '%s'\n", &(ti[1]));

              for (j = 0; j < ti[0]; j += 4)
              {
                LOGSTR2("  %08lX :  %08lX\n", j, ((unsigned long*)&ti)[j/4]);
              }
            }
            */
          }
          break;
      }
    }
  }

  /***************************************************************************/
  /* Interrupts                                                              */
  /***************************************************************************/
            /* TODO
  for (i = 0; i < 67; i++)
  {
    int subintr;
    PspIntrHandlerOptionParam ti;

    switch (i)
    {
      case PSP_GPIO_INT:
      case PSP_ATA_INT:
      case PSP_UMD_INT:
      case PSP_DMACPLUS_INT:
      case PSP_GE_INT:
      case PSP_VBLANK_INT:
        subintr = i;
        break;
      default:
        subintr = 0;
        break;
    }

    LOGSTR2("Check interrupt handler %d, %d\n", i, subintr);
    memset(&ti, 0, sizeof(PspIntrHandlerOptionParam));
    ti.size = sizeof(PspIntrHandlerOptionParam);

    lrc = QueryIntrHandlerInfo(i, subintr, &ti);
    if (lrc >= 0)
    {
      LOGSTR0("Found an intr handler\n");
      LOGSTR2("  Size:   %08lX (expected %08lX)\n", ti.size, sizeof(PspIntrHandlerOptionParam));
      LOGSTR1("  Entry:  %08lX\n", ti.entry);
      LOGSTR1("  Common: %08lX\n", ti.common);
      LOGSTR1("  GP:     %08lX\n", ti.gp);
    }
  }
  */
}