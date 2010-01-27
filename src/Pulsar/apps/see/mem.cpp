#define MEM_
#include "see.hpp"

/* Эта функция получает объем свободной опереативной памяти
*/

unsigned long getAvailableMemory () {

 unsigned long volume;

// если в системе определены эти макросы, то работаем через них
#ifdef _SC_AVPHYS_PAGES
 #ifdef _SC_PAGESIZE

  /* в Linux'е память реально будет освобождаться только когда
     другое приложение ее востребует, иначе по команде free свободной памяти очень мало.
     Поэтому, будем брать полную память и делить ее на 3 (эмпирический коэффициент)
  */
#if HAVE_SYSCONF  
   volume = ( sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) ) / 3;
#endif
  return volume;

 #endif
#endif


// если макросы _SC_... не определены, то будем работать через структуру utsname,
// чтобы определить систему и ее version
// если это Linux, то используем sysinfo (для разных версий Linux
// sysinfo выдает инфо по-разному)
#ifdef __NO_MEMORY_MACROS__

#if HAVE_SYS_UTSNAME_H
struct utsname utsinfo;

# if HAVE_UNAME
   if (uname(&utsinfo) < 0) return 0;                   // если не смогли получить инфо - выходим
# endif

#endif

//if (strcasecmp(utsinfo.sysname, "Linux")) return 0;  // если это не Linux - выходим
#ifdef __YES_LINUX__

// получаем версию ядра
// если меньше 2.3.16 то sysinfo возвращает инфо в байтах
// если больше 2.3.23 (i386) или 2.3.48 (all architectures) то возвращает в определенных юнитах
//int a, b, c;
//sscanf(utsinfo.release, "%d.%d.%d", &a, &b, &c);
//unsigned long release = 65536*a + 256*b + c;

// определяем, какая это машина
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


