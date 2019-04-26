//MakePSMGreatAgain
//Build a wall and make Sony pay for it!
//Probably with he money they stole from us
//by deleting our PSM Purchases 

#include <psp2/kernel/modulemgr.h>
#include <taihen.h>
#include <string.h>
#include <vitasdk.h>
#include "licenseinfo.h"

static SceUID CacheMounted;
static tai_hook_ref_t CacheMounted_ref;

static SceUID SuiteCheckDrm;
static tai_hook_ref_t SuiteCheckDrm_ref;

static SceUID UnityCheckDrm;
static tai_hook_ref_t UnityCheckDrm_ref;

static SceUID SecurityCritical;
static tai_hook_ref_t SecurityCritical_ref;

static SceUID PeekPositive;
static tai_hook_ref_t PeekPositive_ref;

static SceUID LoadModuleHook = -1;
static tai_hook_ref_t LoadModuleHook_ref;


char titleid[12];


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

static int sceCtrlPeekBufferPositive_patched(int port, SceCtrlData *ctrl, int count) {
    int ret = TAI_CONTINUE(int, PeekPositive_ref, port, ctrl, count);
	
	if((ctrl->buttons & (SCE_CTRL_START|SCE_CTRL_UP)) == (SCE_CTRL_START|SCE_CTRL_UP))
	{
		sceAppMgrSetInfobarState(0, 0, 0);
		sceAppMgrLoadExec("app0:/eboot.bin", 0, 0);
	}
	
	return ret;
}   

int sceAppUtilCacheMount_patched(){
	int ret;
	ret = TAI_CONTINUE(int, CacheMounted_ref);
	
	sceIoMkdir("cache0:/_System",6);
		
	if(!fileExists("cache0:/_System/Code"))
	{
		sceClibPrintf("Writing 'Code'\n");
		if(!strcmp(titleid,"PCSI00007"))
			WriteFile("cache0:/_System/Code",&SUITE_CODE,sizeof(SUITE_CODE));
		else
			WriteFile("cache0:/_System/Code",&UNITY_CODE,sizeof(UNITY_CODE));
	}
	
	if(!fileExists("cache0:/_System/target_kdbg_list.dat"))
	{
		sceClibPrintf("Writing 'target_kdbg_list'\n");
		if(!strcmp(titleid,"PCSI00007"))
			WriteFile("cache0:/_System/target_kdbg_list.dat",&SUITE_TARGET,sizeof(SUITE_TARGET));
		else
			WriteFile("cache0:/_System/target_kdbg_list.dat",&UNITY_TARGET,sizeof(UNITY_TARGET));
	}
	
	if(!fileExists("cache0:/_System/vseed.dat"))
	{
		sceClibPrintf("Writing 'vseed'\n");
		if(!strcmp(titleid,"PCSI00007"))
			WriteFile("cache0:/_System/vseed.dat",&SUITE_VSEED,sizeof(SUITE_VSEED));
		else
			WriteFile("cache0:/_System/vseed.dat",&UNITY_VSEED,sizeof(UNITY_VSEED));
	}
	
	if(!fileExists("cache0:/_System/protected_kconsole_cache.dat"))
	{
		sceClibPrintf("Writing 'protected_kconsole_cache'\n");
		if(!strcmp(titleid,"PCSI00007"))
			WriteFile("cache0:/_System/protected_kconsole_cache.dat",&SUITE_KCONSOLE, sizeof(SUITE_KCONSOLE));
		else
			WriteFile("cache0:/_System/protected_kconsole_cache.dat",&UNITY_KCONSOLE, sizeof(UNITY_KCONSOLE));
	}

	if(!fileExists("cache0:/_System/psmdrmboot.dat"))
	{
		sceClibPrintf("Writing 'psmdrmboot'\n");
		if(!strcmp(titleid,"PCSI00007"))
			WriteFile("cache0:/_System/psmdrmboot.dat",&SUITE_PSMDRMBOOT, sizeof(SUITE_PSMDRMBOOT));
		else
			WriteFile("cache0:/_System/psmdrmboot.dat",&UNITY_PSMDRMBOOT, sizeof(UNITY_PSMDRMBOOT));
	}

	return ret;
}



int ret0 (int *args)
{
	return 0;
}


SceUID sceKernelLoadStartModule_patched(char *path, SceSize args, void *argp, int flags, SceKernelLMOption *option, int *status)
{

	
	SceUID ret;
	ret = TAI_CONTINUE(SceUID, LoadModuleHook_ref, path, args, argp, flags, option, status);

	if(strstr(path,"libmono.suprx")) //PSM
	{
		
				SecurityCritical = taiHookFunctionExport(&SecurityCritical_ref, 
														  "SceLibMono",
														  TAI_ANY_LIBRARY,
														  0x02A867BC, //mono_security_enable_core_clr 
														  ret0);
				sceClibPrintf("SecurityCritical: %x\n",SecurityCritical);
	}
	
	if(strstr(path,"libpsm.suprx")) //PSM
	{
		
				PeekPositive = taiHookFunctionImport(&PeekPositive_ref, 
										  "SceLibPsm",
										  TAI_ANY_LIBRARY,
										  0xA9C3CED6, // sceCtrlPeekBufferPositive
										  sceCtrlPeekBufferPositive_patched);
				sceClibPrintf("PeekPositive: %x\n",PeekPositive);
	}
	
	if(strstr(path,"libScePsmEdata.suprx")) //PSM Unity
	{
		
				UnityCheckDrm = taiHookFunctionOffset(&UnityCheckDrm_ref, 
														ret,
														0,
														0x5a62, //PsmDrmBootCheck
														1, 
														ret0);
				sceClibPrintf("UnityCheckDrm: %x\n",UnityCheckDrm);
	}
	
	return ret;
}


void _start() __attribute__ ((weak, alias ("module_start"))); 
void module_start(SceSize argc, const void *args) {

	sceAppMgrAppParamGetString(0, 12, titleid, 256);
	
	if(!strcmp(titleid,"PCSI00007"))
	{
		tai_module_info_t tai_info;
		tai_info.size = sizeof(tai_module_info_t);
		taiGetModuleInfo("ScePsmDev", &tai_info);

		SuiteCheckDrm = taiHookFunctionOffset(&SuiteCheckDrm_ref, 
												tai_info.modid,
												0,
												0xbe2, //SuiteCheckDrm
												1, 
												ret0);
		sceClibPrintf("SuiteCheckDrmHook: %x\n",SuiteCheckDrm);
	}
	
	if(!strcmp(titleid,"PCSI00009"))
	{
		tai_module_info_t tai_info;
		tai_info.size = sizeof(tai_module_info_t);
		taiGetModuleInfo(TAI_MAIN_MODULE, &tai_info);
		if(strstr(tai_info.name, "Unity"))
		{
			PeekPositive = taiHookFunctionImport(&PeekPositive_ref, 
						  TAI_MAIN_MODULE,
						  TAI_ANY_LIBRARY,
						  0x15F81E8C, // sceCtrlPeekBufferPositive2
						  sceCtrlPeekBufferPositive_patched);
			sceClibPrintf("PeekPositive: %x\n",PeekPositive);
		}
	}

	if(!strcmp(titleid,"PCSI00007") || !strcmp(titleid,"PCSI00009")) // PSM Runtime & PSM Unity Runtime
	{
		sceClibPrintf("Silca: I like to see girls die :3\n");
		sceClibPrintf("Loaded!\n");
		sceClibPrintf("Running on %s\n",titleid);

		CacheMounted = taiHookFunctionImport(&CacheMounted_ref, 
								  TAI_MAIN_MODULE,
								  TAI_ANY_LIBRARY,
								  0x0AA56143, //sceAppUtilCacheMount
								  sceAppUtilCacheMount_patched);

								  
     	LoadModuleHook = taiHookFunctionImport(&LoadModuleHook_ref, 
								  TAI_MAIN_MODULE,
								  TAI_ANY_LIBRARY,
								  0x2DCC4AFA, //sceKernelLoadStartModule
								  sceKernelLoadStartModule_patched);
								  
		sceClibPrintf("LoadModuleHook: %x\n",LoadModuleHook);
	}
	

}

int module_stop(SceSize argc, const void *args) {

  // release hooks
	if (CacheMounted >= 0) taiHookRelease(CacheMounted, CacheMounted_ref);
	if (SuiteCheckDrm >= 0) taiHookRelease(SuiteCheckDrm, SuiteCheckDrm_ref);
	if (LoadModuleHook >= 0) taiHookRelease(LoadModuleHook, LoadModuleHook_ref);
	if (SecurityCritical >= 0) taiHookRelease(SecurityCritical, SecurityCritical_ref);
	
  return SCE_KERNEL_STOP_SUCCESS;
}
