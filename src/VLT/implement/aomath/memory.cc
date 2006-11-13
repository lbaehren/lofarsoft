/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: memory.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  16/01/02  created 
*/

/************************************************************************
*   NAME
*   
* 
*   SYNOPSIS
*
*   
*   PARENT CLASS
*
* 
*   DESCRIPTION
*
*
*   PUBLIC METHODS
*
*
*   PUBLIC DATA MEMBERS
*
*
*   PROTECTED METHODS
*
*
*   PROTECTED DATA MEMBERS
*
*
*   PRIVATE METHODS
*
*
*   PRIVATE DATA MEMBERS
7*
*
*   FILES
*
*   ENVIRONMENT
*
*   COMMANDS
*
*   RETURN VALUES
*
*   CAUTIONS 
*
*   EXAMPLES
*
*   SEE ALSO
*
*   BUGS   
* 
*------------------------------------------------------------------------
*/

#define _POSIX_SOURCE 1

static char *rcsId="@(#) $Id: memory.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include <aomath/memory.h>

memoryPool::memoryPool(int size)
{
#ifdef HAVE_VXWORKS
    printf("aomath: memoryPool::memoryPool(size)\n");
    memoryBlock = (char*)::malloc(size);
    if (memoryBlock) {
       partition = memPartCreate(memoryBlock,size);
    } else {
       printf("Cannot create memory pool (%d)\n",size);
       partition = NULL;
    }
#else
#endif
    constructed = true;
}

memoryPool::~memoryPool()
{
#ifdef HAVE_VXWORKS
    ::free(memoryBlock);
    partition = NULL;
#else
#endif
    // memory partition not freed
}

void *memoryPool::malloc(int size)
{
#ifdef HAVE_VXWORKS
    void *m = memPartAlloc(partition,size);
    if (m==0) {
       printf("Memory partition %p full!! (ask to allocate %d bytes)\n",
	      (char*)partition,size);
       show(1);
    }
#else
    void *m=::malloc(size);
#endif
    return m;
}

void memoryPool::free(void *ptr)
{
#ifdef HAVE_VXWORKS
    memPartFree(partition,(char*)ptr);
#else
    ::free(ptr);
#endif
}

void memoryPool::show(int type)
{
#ifdef HAVE_VXWORKS
    memPartShow(partition,type);
#endif
}
/*___oOo___*/
