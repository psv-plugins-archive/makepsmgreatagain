# IDUSpoof
This is a plugin that spoofs IDU Mode (vshSysconIsIduMode) to allways return 0x1             
this has the effect of tricking whatever application's its configured for into thinking its running in IDU mode.             
in fact, if you do \*ALL it has the same effect as if you just enabled IDU mode!         
the advantage to this plugin is ofcource that you can specify exactly what apps idu is set for.

for example. if you want to enable the package installer. simply do          
\*NPXS10031           
ux0:tai/iduSpoof.suprx             
           
or enable IDU settings without IDU Mode:            
             
\*NPXS10015           
ux0:tai/iduSpoof.suprx           

though keep in mind henkaku overwrites idu settings with its own.           


!! WORKS ON 3.65 And 3.68!!           

Download: https://bitbucket.org/SilicaAndPina/iduspoof/downloads/iduSpoof.suprx