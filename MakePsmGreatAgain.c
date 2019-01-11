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

int gameRunning = 0;
int hookReleased = 0;

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

static int rtcHook = -1;
static tai_hook_ref_t rtcHook_ref;

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
	
	if(!strcmp(titleid,"PCSI00009")) //UNITY
	{
		if(gameRunning)
		{
			printf("Releasing rtcHook\n");
			if(rtcHook >= 0)taiHookReleaseForKernel(rtcHook, rtcHook_ref);
			rtcHook = -1;
			hookReleased = 1;
		}
	}
	
	
	return ret;
}

int ksceRtcGetCurrentTick_patched(unsigned int* timestamp) //fake valid time
{
	
	ret = TAI_CONTINUE(int, secureHook_ref,timestamp);
	
	if(hookReleased) // sanity check
	{
		printf("WARN: Hook was NOT released properly!\n");
		return ret;
	}
	
	pid = ksceKernelGetProcessId();
	ksceKernelGetProcessTitleId(pid,titleid,12);
	

	if(!strcmp(titleid,"PCSI00009")) //UNITY (nice fail on license check lol https://twitter.com/SilicaDevs/status/1081758443495514112)
	{
	printf("Spoofing time to %x%x\n",UNITY_LICENSE_VALID[1],UNITY_LICENSE_VALID[0]);

	timestamp[0] = UNITY_LICENSE_VALID[0];
	timestamp[1] = UNITY_LICENSE_VALID[1];
	return 0;
	}
	
	return ret;
}

int isAllowedSerial_patch(int arg1, int arg2, int arg3, int arg4) { //Not acturally needed for reF00D but kept to remain backwards-compadible with RePatch compati packs.
  TAI_CONTINUE(int, isAllowedSerial_ref,arg1,arg2,arg3,arg4);
  return 1; 
}


static SceUID _ksceKernelLaunchAppPatched(void *args)
{
    char *titleid = (char *)((uintptr_t *)args)[0];
    uint32_t flags = (uint32_t)((uintptr_t *)args)[1];
    char *path = (char *)((uintptr_t *)args)[2];
    void *unk = (void *)((uintptr_t *)args)[3];
	
	printf("Loading SELF: %s\n",path);
	
    if ((flags == 0x1000000 && strstr(path, "PCSI00007"))) //SUITE
    {
		ksceIoMkdir("ux0:/cache",6);
		ksceIoMkdir("ux0:/cache/PCSI00007",6);
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

		//Add case to fix dots-tb's broken repatch plugin (TEMPORARY)
		if ((flags == 0x1000000 && strstr(path, "ux0:/patch/PCSI00007"))) 
		{
			char buf[1024];
			memset(buf,0x00,1024);
			char *NewPath;
			NewPath = path + sizeof("ux0:/patch/PCSI00007/") - 1;
			snprintf(buf,1024,"ux0:/repatch/PCSI00007/%s",NewPath);
			if(fileExists(buf))
			{
				ret = TAI_CONTINUE(int, runHook_ref, titleid, flags, buf, unk);
				return ret;
			}
		}
		
		if ((flags == 0x1000000 && strstr(path, "ux0:/app/PCSI00007")))
		{
			char buf[1024];
			memset(buf,0x00,1024);
			char *NewPath;
			NewPath = path + sizeof("ux0:/app/PCSI00007/") - 1;
			snprintf(buf,1024,"ux0:/repatch/PCSI00007/%s",NewPath);
			if(fileExists(buf))
			{
				ret = TAI_CONTINUE(int, runHook_ref, titleid, flags, buf, unk);
				return ret;
			}
		}
		
		ret = TAI_CONTINUE(int, runHook_ref, titleid, flags, path, unk);
		
		return ret;
		
    }
	
	if ((flags == 0x1000000 && strstr(path, "PCSI00009"))) //UNITY
    {
		ksceIoMkdir("ux0:/cache",6);
		ksceIoMkdir("ux0:/cache/PCSI00009",6);
		ksceIoMkdir("ux0:/cache/PCSI00009/_System",6);
		
		if(strstr(path,"eboot.bin")) //MAIN SELF
		{
			
			printf("Main Dev Suite running!\n");
			gameRunning = 0;
			hookReleased = 0;
			if(rtcHook <= 0)
			{
				rtcHook = taiHookFunctionExportForKernel(KERNEL_PID,
											&rtcHook_ref, 
											"SceRtc",
											0x0351D827, // SceRtcForDriver
											0xDEC408D4, // ksceRtcGetCurrentTick
											ksceRtcGetCurrentTick_patched);
				printf("RtcHook: %x\n",rtcHook);
			}
			}
		
		if(strstr(path,".self")){
		printf("Game Running!\n");
		gameRunning = 1;
		}
		
		ksceIoMkdir("ux0:/cache/PCSI00009/_System",6);
			
		if(!fileExists("ux0:/cache/PCSI00009/_System/Code"))
		{
			printf("Writing 'Code'\n");
			WriteFile("ux0:/cache/PCSI00009/_System/Code",&UNITY_CODE,sizeof(UNITY_CODE));
		}
		
		if(!fileExists("ux0:/cache/PCSI00009/_System/target_kdbg_list.dat"))
		{
			printf("Writing 'target_kdbg_list'\n");
			WriteFile("ux0:/cache/PCSI00009/_System/target_kdbg_list.dat",&UNITY_TARGET,sizeof(UNITY_TARGET));
		}
		
		if(!fileExists("ux0:/cache/PCSI00009/_System/vseed.dat"))
		{
			printf("Writing 'vseed'\n");
			WriteFile("ux0:/cache/PCSI00009/_System/vseed.dat",&UNITY_VSEED,sizeof(UNITY_VSEED));
		}
		
		//Write KConsole and PSMDrmBoot
		printf("Writing 'protected_kconsole_cache'\n");
		WriteFile("ux0:/cache/PCSI00009/_System/protected_kconsole_cache.dat",&UNITY_KCONSOLE, sizeof(UNITY_KCONSOLE));
		printf("Writing 'psmdrmboot'\n");
		
		WriteFile("ux0:/cache/PCSI00009/_System/psmdrmboot.dat",&UNITY_PSMDRMBOOT, sizeof(UNITY_PSMDRMBOOT));
		
		
										
		
		//Add case to fix dots-tb's broken repatch plugin (TEMPORARY)
		if ((flags == 0x1000000 && strstr(path, "ux0:/patch/PCSI00009"))) 
		{
			char buf[1024];
			memset(buf,0x00,1024);
			char *NewPath;
			NewPath = path + sizeof("ux0:/patch/PCSI00009/") - 1;
			snprintf(buf,1024,"ux0:/repatch/PCSI00009/%s",NewPath);
			if(fileExists(buf))
			{
				ret = TAI_CONTINUE(int, runHook_ref, titleid, flags, buf, unk);
				return ret;
			}
		}
		
		if ((flags == 0x1000000 && strstr(path, "ux0:/app/PCSI00009")))
		{
			char buf[1024];
			memset(buf,0x00,1024);
			char *NewPath;
			NewPath = path + sizeof("ux0:/app/PCSI00009/") - 1;
			snprintf(buf,1024,"ux0:/repatch/PCSI00009/%s",NewPath);
			if(fileExists(buf))
			{
				ret = TAI_CONTINUE(int, runHook_ref, titleid, flags, buf, unk);
				return ret;
			}
		}
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
	if(fileExists("ux0:/repatch/PCSI00009/eboot.bin") || fileExists("ux0:/repatch/PCSI00007/eboot.bin"))
	{
		isAllowedSerialHook = taiHookFunctionExportForKernel(KERNEL_PID,
								&isAllowedSerial_ref, 
								"SceSblACMgr",
								0x9AD8E213, // SceSblACMgrForDriver
								0x062CAEB2, // isAllowedSerial
								isAllowedSerial_patch);
		printf("isAllowedSerialHook: %x\n",isAllowedSerialHook);
	}
	printf("RunHook: %x\n",runHook);
	printf("SecureHook: %x\n",secureHook);
	
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	return SCE_KERNEL_STOP_SUCCESS;
}
