#define MEM_
#include "see.hpp"

/* This function gets the volume of free RAM
*/

unsigned long getAvailableMemory () {

 unsigned long volume;

// if system has these macroses, than using them
#ifdef _SC_AVPHYS_PAGES
 #ifdef _SC_PAGESIZE

/* in Linux the memory will be really freed only when the next application
 * will ask for it, otherwise command 'free' will print the small amount of free memory.
 * Thus, we will take the full memory and divide it by 3 (empirical coefficient) :)
 */
#if HAVE_SYSCONF  
   volume = ( sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) ) / 3;
#endif
  return volume;

 #endif
#endif


/* if macroses _SC_ are not determined, then we will use utsname structure
 * to determine the workstation system and its version
 * if it is Linux, then use 'sysinfo' (for different versions of Linux
 * sysinfo gives info differently)
 */
#ifdef __NO_MEMORY_MACROS__

#if HAVE_SYS_UTSNAME_H
struct utsname utsinfo;

# if HAVE_UNAME
   if (uname(&utsinfo) < 0) return 0;                   // if could not get info - exit
# endif

#endif

//if (strcasecmp(utsinfo.sysname, "Linux")) return 0;  // if it is not Linux - exit
#ifdef __YES_LINUX__

// obtain the kernel version
// if < 2.3.16, then sysinfo gives info in bytes
// if > 2.3.23 (i386) or >2.3.48 (all architectures), then sysinfo gives info in some units
//int a, b, c;
//sscanf(utsinfo.release, "%d.%d.%d", &a, &b, &c);
//unsigned long release = 65536*a + 256*b + c;

// check what is this system
bool intel = false;
#if HAVE_SYS_UTSNAME_H
  if (utsinfo.machine[0] == 'i' && utsinfo.machine[2] == '8' && utsinfo.machine[3] == '6') intel = true;
#endif

#if HAVE_SYS_SYSINFO_H
struct sysinfo sinfo;

#if HAVE_SYSINFO

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,3,16)
//if (release <= 2*65536 + 3*256 + 16) {
 if (!sysinfo(&sinfo)) { volume = sinfo.totalram / 3; return volume; }
  else return 0;
//}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,3,23)
 #if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,48)
//if (release >= 2*65536 + 3*256 + 48 ||
//    (release >= 2*65536 + 3*256 + 23 && intel) ) {
 if (intel) if (!sysinfo(&sinfo)) { volume = ( sinfo.totalram * sinfo.mem_unit ) / 3; return volume; }
             else return 0;	     
//}
 #endif
#endif


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,3,48)
 if (!sysinfo(&sinfo)) { volume = ( sinfo.totalram * sinfo.mem_unit ) / 3; return volume; }
  else return 0;  
#endif

#endif //#if HAVE_SYSINFO

#endif //#if HAVE_SYS_SYSINFO_H 

#endif //#ifdef __YES_LINUX__

#endif //#ifdef __NO_MEMORY_MACROS__

return 0;
}


