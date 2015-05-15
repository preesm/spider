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

1. Import projects you want to compile within your workspace. Note that there is 2 types of projects and they need to be imported differently : 
  *DSP project : "File"->"Import..." and select "Code Composer Studio/CCS Projects"
  *ARM project : "File"->"Import..." and select "General/Existing Projects into Workspace"

2. Set the various environnment variables:

  a. In CCS, go into "Window"->"Preferences" then "General/Workspace/Linked Resources", and add the following resource :
    * Name: "SPIDER_PATH" (without quotes) Value: "PATH_TO_YOUR_SPIDER_PATH". e.g : /home/user/spider_git/master
  
  b. Always in the "Preferences" window, go into "Code Composer Studio/Build/Variables" and add the following rows :

           Name              Type                                   Value                                  
 ------------------------- -------- ---------------------------------------------------------------------- 
  DSPLIB_PATH               Path     PATH_TO_YOUR_DSPLIB                                                   
  GCC_LINARO                Path     PATH_TO_YOUR_LINARO_CROSS_COMPILATION                                 
  PDK_PATH                  Path     PATH_TO_YOUR_PDK                                                      
  ROOTFS_PATH               Path     PATH_TO_YOUR_ROOT_FILESYSTEM (contains all .h headers)                
  SPIDER_DEVICE             String   YOUR_PLATEFORM_NAME (currently available : DEVICE_K2H or DEVICE_K2L)  
  SPIDER_K2ARMLIB_OUTPATH   Path     DOT_SO_LIB_OUTPUT_PATH                                                
  SPIDER_PATH               Path     PATH_TO_SPIDER_ROOT   

Then you should be able to compile the project on any and for any platform.

Last revision : 5/15/2015  
