//MakePsmGreatAgain
//We need to build a wall and make SONY pay for it (with all the money they stole from deleting our PSM Purchases!) 

#include <taihen.h>
#include <string.h>
#include <vitasdk.h>
#include "licenseinfo.h"
#define printf sceClibPrintf

static SceUID mountCacheHook;
static tai_hook_ref_t ref_mountCacheHook;

static SceUID mountCacheHook;
static tai_hook_ref_t ref_mountCacheHook;

static SceUID tickHook;
static tai_hook_ref_t ref_tickHook;

static SceUID networkHook;
static tai_hook_ref_t ref_networkHook;

int ret;
int UNITY = 0;
int SUITE = 0;


int getFileSize(const char *file) {
	SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
	if (fd < 0)
		return fd;
	int fileSize = sceIoLseek(fd, 0, SCE_SEEK_END);
	sceIoClose(fd);
	return fileSize;
}

int fileExists(const char *file) {
	if(getFileSize(file) <= 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int sceRtcGetCurrentNetworkTick_patch(SceRtcTick *ptick)
{
	ret = TAI_CONTINUE(int, ref_networkHook, ptick);
	printf("getCurrentNetworkTick %llx",ptick->tick);
	if(UNITY)
	{
		ptick->tick = UNITY_LICENSE_VALID;
	}
	else
	{
		ptick->tick = SUITE_LICENSE_VALID;
	}
	
	return ret;
}

int ReadFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);

	sceIoClose(fd);
	return read;
}


int WriteFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

void _start() __attribute__ ((weak, alias ("module_start")));

int sceAppUtilCacheMount_patch()
{
	ret = TAI_CONTINUE(int,ref_mountCacheHook);
	//Create Publishing License if it Doesnt Exist
	if(UNITY)
	{
	sceIoMkdir("cache0:/_System",6);
	
	if(!fileExists("cache0:/_System/Code"))
	{
		printf("Writing 'Code'\n");
		WriteFile("cache0:/_System/Code",&UNITY_CODE,sizeof(UNITY_CODE));
	}
	
	if(!fileExists("cache0:/_System/target_kdbg_list.dat"))
	{
		printf("Writing 'target_kdbg_list'\n");
		WriteFile("cache0:/_System/target_kdbg_list.dat",&UNITY_TARGET,sizeof(UNITY_TARGET));
	}
	
	if(!fileExists("cache0:/_System/vseed.dat"))
	{
		printf("Writing 'vseed'\n");
		WriteFile("cache0:/_System/vseed.dat",&UNITY_VSEED,sizeof(UNITY_VSEED));
	}
	
	//Write Protected KConsole and PSMDrmBoot
	printf("Writing 'protected_kconsole_cache'\n");
	WriteFile("cache0:/_System/protected_kconsole_cache.dat",&UNITY_KCONSOLE, sizeof(UNITY_KCONSOLE));
	printf("Writing 'psmdrmboot'\n");
	WriteFile("cache0:/_System/psmdrmboot.dat",&UNITY_PSMDRMBOOT, sizeof(UNITY_PSMDRMBOOT));
	}
	else
	{
	sceIoMkdir("cache0:/_System",6);
		
	if(!fileExists("cache0:/_System/Code"))
	{
		printf("Writing 'Code'\n");
		WriteFile("cache0:/_System/Code",&SUITE_CODE,sizeof(SUITE_CODE));
	}
	
	if(!fileExists("cache0:/_System/target_kdbg_list.dat"))
	{
		printf("Writing 'target_kdbg_list'\n");
		WriteFile("cache0:/_System/target_kdbg_list.dat",&SUITE_TARGET,sizeof(SUITE_TARGET));
	}
	
	if(!fileExists("cache0:/_System/vseed.dat"))
	{
		printf("Writing 'vseed'\n");
		WriteFile("cache0:/_System/vseed.dat",&SUITE_VSEED,sizeof(SUITE_VSEED));
	}
	
	//Write Protected KConsole and PSMDrmBoot
	printf("Writing 'protected_kconsole_cache'\n");
	WriteFile("cache0:/_System/protected_kconsole_cache.dat",&SUITE_KCONSOLE, sizeof(SUITE_KCONSOLE));
	printf("Writing 'psmdrmboot'\n");
	WriteFile("cache0:/_System/psmdrmboot.dat",&SUITE_PSMDRMBOOT, sizeof(SUITE_PSMDRMBOOT));
	}
	
	return ret;
}

int module_start(SceSize argc, const void *args) {
	char titleid[12];
	sceAppMgrAppParamGetString(0, 12, titleid, 256); //Which devagent is this?

	if(!strcmp(titleid,"PCSI00007")) //SUITE
	{
	SUITE = 1;
	UNITY = 0;
	}
	
	if(!strcmp(titleid,"PCSI00009")) //UNITY
	{
	SUITE = 0;
	UNITY = 1;
	}
	sceClibPrintf("Running on %s",titleid);
	mountCacheHook = taiHookFunctionImport(&ref_mountCacheHook, 
										  TAI_MAIN_MODULE,
										  TAI_ANY_LIBRARY,
										  0x0AA56143, // sceAppUtilCacheMount
										  sceAppUtilCacheMount_patch);
	ksceUdcdStopCurrentInternal(2);
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {

  // release hooks
  if (mountCacheHook >= 0) taiHookRelease(mountCacheHook, ref_mountCacheHook);


  return SCE_KERNEL_STOP_SUCCESS;
}
