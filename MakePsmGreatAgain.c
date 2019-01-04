//MakePSMGreatAgain
//Build a wall and make Sony pay for it!
//Probably with he money they stole from us
//by deleting our PSM Purchases 

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <taihen.h>
#include <vitasdkkern.h>

#include "licenseinfo.h"

#define printf ksceDebugPrintf

//e23e8c7b15a198 SUITE
//e23e8c93b8ed90 UNITY

int getFileSize(const char *file) {
	SceUID fd = ksceIoOpen(file, SCE_O_RDONLY, 0);
	if (fd < 0)
		return fd;
	int fileSize = ksceIoLseek(fd, 0, SCE_SEEK_END);
	ksceIoClose(fd);
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


int ReadFile(char *file, void *buf, int size) {
	SceUID fd = ksceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int read = ksceIoRead(fd, buf, size);

	ksceIoClose(fd);
	return read;
}


int WriteFile(char *file, void *buf, int size) {
	SceUID fd = ksceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = ksceIoWrite(fd, buf, size);

	ksceIoClose(fd);
	return written;
}


static int secureHook = -1;
static tai_hook_ref_t secureHook_ref;

static int runHook = -1;
static tai_hook_ref_t runHook_ref;

static int isAllowedSerialHook = -1;
static tai_hook_ref_t isAllowedSerial_ref;

int ret = 0;
int pid;

unsigned int SUITE_LICENSE_VALID[2] = {0x7b15a198,0xe23e8c};
unsigned int UNITY_LICENSE_VALID[2] = {0x93b8ed90,0xe23e8c};
char titleid[12];

int ksceRtcGetCurrentSecureTick_patched(unsigned int* timestamp) //fake valid time
{
	printf("SceRtcGetSecure called!\n");
	
	ret = TAI_CONTINUE(int, secureHook_ref,timestamp);

	pid = ksceKernelGetProcessId();
	ksceKernelGetProcessTitleId(pid,titleid,12);
	
	
	if(!strcmp(titleid,"PCSI00007")) //SUITE
	{
	printf("Spoofing time to %x%x\n",SUITE_LICENSE_VALID[1],SUITE_LICENSE_VALID[0]);

	timestamp[0] = SUITE_LICENSE_VALID[0];
	timestamp[1] = SUITE_LICENSE_VALID[1];
	return 0;
	}
	
	return ret;
}

int isAllowedSerial_patch(int arg1, int arg2, int arg3, int arg4) { //Tell f00d to fuck off
  TAI_CONTINUE(int, isAllowedSerial_ref,arg1,arg2,arg3,arg4);
  return 1; 
}


static SceUID _ksceKernelLaunchAppPatched(void *args)
{
    char *titleid = (char *)((uintptr_t *)args)[0];
    uint32_t flags = (uint32_t)((uintptr_t *)args)[1];
    char *path = (char *)((uintptr_t *)args)[2];
    void *unk = (void *)((uintptr_t *)args)[3];
	
    if ((flags == 0x1000000 && strstr(path, "PCSI00007")))
    {
								 
		
		ksceIoMkdir("ux0:/cache/PCSI00007/_System",6);
			
		if(!fileExists("ux0:/cache/PCSI00007/_System/Code"))
		{
			printf("Writing 'Code'\n");
			WriteFile("ux0:/cache/PCSI00007/_System/Code",&SUITE_CODE,sizeof(SUITE_CODE));
		}
		
		if(!fileExists("ux0:/cache/PCSI00007/_System/target_kdbg_list.dat"))
		{
			printf("Writing 'target_kdbg_list'\n");
			WriteFile("ux0:/cache/PCSI00007/_System/target_kdbg_list.dat",&SUITE_TARGET,sizeof(SUITE_TARGET));
		}
		
		if(!fileExists("ux0:/cache/PCSI00007/_System/vseed.dat"))
		{
			printf("Writing 'vseed'\n");
			WriteFile("ux0:/cache/PCSI00007/_System/vseed.dat",&SUITE_VSEED,sizeof(SUITE_VSEED));
		}
		
		//Write KConsole and PSMDrmBoot
		printf("Writing 'protected_kconsole_cache'\n");
		WriteFile("ux0:/cache/PCSI00007/_System/protected_kconsole_cache.dat",&SUITE_KCONSOLE, sizeof(SUITE_KCONSOLE));
		printf("Writing 'psmdrmboot'\n");
		WriteFile("ux0:/cache/PCSI00007/_System/psmdrmboot.dat",&SUITE_PSMDRMBOOT, sizeof(SUITE_PSMDRMBOOT));
		

		ret = TAI_CONTINUE(int, runHook_ref, titleid, flags, path, unk);
		
		return ret;
		
    }

    return TAI_CONTINUE(int, runHook_ref, titleid, flags, path, unk); // returns pid
}

static SceUID ksceKernelLaunchApp_patched(char *titleid, uint32_t flags, char *path, void *unk)
{
    uintptr_t args[4];
    args[0] = (uintptr_t)titleid;
    args[1] = (uintptr_t)flags;
    args[2] = (uintptr_t)path;
    args[3] = (uintptr_t)unk;

    return ksceKernelRunWithStack(0x4000, _ksceKernelLaunchAppPatched, args);
}


void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	
	runHook = taiHookFunctionExportForKernel(KERNEL_PID, &runHook_ref, "SceProcessmgr", 0x7A69DE86, 0x71CF71FD, ksceKernelLaunchApp_patched);
		if (runHook < 0)
			runHook = taiHookFunctionExportForKernel(KERNEL_PID, &runHook_ref, "SceProcessmgr", 0xEB1F8EF7, 0x68068618, ksceKernelLaunchApp_patched);

	secureHook = taiHookFunctionExportForKernel(KERNEL_PID,
								&secureHook_ref, 
								"SceRtc",
								0x0351D827, // SceRtcForDriver
								0x401C0954, // ksceRtcGetCurrentSecureTick
								ksceRtcGetCurrentSecureTick_patched);
								
	isAllowedSerialHook = taiHookFunctionExportForKernel(KERNEL_PID,
								&isAllowedSerial_ref, 
								"SceSblACMgr",
								0x9AD8E213, // SceSblACMgrForDriver
								0x062CAEB2, // isAllowedSerial
								isAllowedSerial_patch);

								  
	printf("RunHook: %x\n",runHook);
	printf("SecureHook: %x\n",secureHook);
	printf("isAllowedSerialHook: %x\n",isAllowedSerialHook);
	
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	return SCE_KERNEL_STOP_SUCCESS;
}
