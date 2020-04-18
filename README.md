# MakePsmGreatAgain
Modifications to the PSM Developer applications

Features:     
+ Never-Ending Publishing License. - While MakePsmGreatAgain is active, you dont have to "Refresh License" with PSM+     
+ USB Serial Mode Re-Enabled - Can connect PC to Development Assistant via USB      
+ PSMPlus Publishing Keys - Apps signed with PSMPlus Keys will work all consoles with MakePsmGreatAgain     
+ Exit with Start+Up - While a PSM app is running, You can press Start+Up to return back to Developer Assistant     
+ All applications are "SecurityCritcal" - Enables ALL .NET functions, including ones marked 'unsafe'     
+ app.info "project_name" check patched (runtime only) - Enables games to run regardless of if "project_name" is set to "*" or "_PSM_DEFAULT_"
Where gonna Build a wall and make Sony pay for it!    
Probably with he money they stole from us    
by deleting our PSM Purchases    
    
(Note: As PSM Dev is revoked in f00d you need reF00D or rePatch with a compati pack in order to run it)     

# Installation
```
*PCSI00007
ux0:/tai/MakePsmGreatAgain.suprx
*PCSI00009
ux0:/tai/MakePsmGreatAgain.suprx
```
(use ur0 if prefered)

# FAQ     
    
[How do i install PSM Dev?](https://www.youtube.com/watch?v=CuxaVTyAVn8)    
[How do i install the PSM SDK?](https://www.youtube.com/watch?v=KoZ1xVNTjUc)    
    
# V1.5 -
Patched app.info "project_name" check. - normally PSM Dev will only boot games whos project_name matches that of
the installed keys. v1.5 patches this check, but ONLY at runtime. install-time it still doesn't work.
so you'll have to manually install psdp's with this setting to ux0:/cache/PCSI00009/7 directly

Download: https://bitbucket.org/SilicaAndPina/makepsmgreatagain/downloads/MakePsmGreatAgain-v1.5.suprx    

# V1.4 -
Switched to Userland Plugin (.suprx)    
Now enables "SecurityCritical" on all apps in PSM Dev (FOR ULTIMATE C# CAPIBILITYS!) - thx doubletoker     
You can now use Start + UP to exit back to the PSM Developer application.     
Probably only reF00D works with it now xD        
    
Download: https://bitbucket.org/SilicaAndPina/makepsmgreatagain/downloads/MakePsmGreatAgain-v1.4.suprx    
    
# V1.3 -
Now works with reF00D plugin by FAPS Team (as well as repatch) 

Download: https://bitbucket.org/SilicaAndPina/makepsmgreatagain/downloads/MakePsmGreatAgain-v1.3.skprx

# v1.2 -
Infinite Publishing License for PSM Dev for Unity (PCSI00009)  
PSM Dev for Unity, app/game launching acturally working now (fixed bugs that dots-tb didnt want to fix)

Download: https://bitbucket.org/SilicaAndPina/makepsmgreatagain/downloads/MakePsmGreatAgain-v1.2.skprx

# v1.0 -
Automatically creates publishing license (if not allready created) at PSM Dev bootup (still need psm+ for development!)    
Re-Enable's SceUsbSerial for ALL applications (yes, if u want to use it on your own homebrew, YOU CAN!)   
Spoofs SecureTick to allways be within the valid period for the PsmDrmBoot and KConsole Cache (Publsihing License will never expire)   
Note: Only PSM Dev Suite supported in v1.0  

Download: https://bitbucket.org/SilicaAndPina/makepsmgreatagain/downloads/MakePsmGreatAgain.skprx  
