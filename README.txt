=========================
SPIDER Scheduling Library
=========================

Spider is composed of a set of 2 libraries :
- a DSP library, that is static
- an ARM library, that is Linux ".so" dynamic compliant

== COMPILATION GUIDE

_REQUIREMENTS :
-A Linux host to proceed to the cross compilation process.
-CCS Studio (starting from v5)
-Linaro (or equivalent) ARM toolchain, for cross compilation.
-Texas Instruments MCSDK, including DSP lib, PDK, linux sysroot.
 
_GUIDE :

0. It is strongly advised to create a new workspace.

1. Set the various environnment variables in the "launch_env.sh" files :
           Name              Type                                      Value                                    
 ------------------------- -------- --------------------------------------------------------------------------- 
  ROOTFS_PATH               Path     Path to your PDK folder (that should contains all the                      
                                     .h headers in a subfolder /usr/lib).                                       
  PDK_PATH                  Path     Path to your PDK folder.                                                   
  DSPLIB_PATH               Path     Path to your DSP Lib folder.                                               
  GCC_LINARO                Path     Path to your Linaro compilation toolchain (should                          
                                     contains binaries like "arm-linux-gnueabihf-***")                          
  SPIDER_PATH               Path     Path to your master spider root.                                           
  SPIDER_DEVICE             String   Define for your platform (currently available : DEVICE_K2H or DEVICE_K2L)  
  PDK_DEVICE_SUBFOLDER      Path     This corresponds to the name of the folder that differs between            
                                     platforms, in order to load proper DSP's .lib files.                       
  SPIDER_K2ARMLIB_OUTPATH   Path     Folder where ARM .so files will be output.                                 
  SPIDER_K2DSPLIB_OUTPATH   Path     Folder where static DSP .lib files will be output.                         
  CCS_PATH                  Path     Path to your Code Composer binary.

2.  In CCS, go into "Window"->"Preferences" then "General/Workspace/Linked Resources", and add the following resource :
    * Name: "SPIDER_PATH" (without quotes) Value: "PATH_TO_YOUR_SPIDER_PATH". e.g : /home/user/spider_git/master


3. Import projects you want to compile within your workspace. Note that there is 2 types of projects and they need to be imported differently : 
Then you should be able to compile the project on any and for any platform.


Last revision : 5/22/2015  
