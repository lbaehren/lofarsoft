/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: MAStdFilter.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  04/07/02  created 
*/

/************************************************************************
*   NAME
*   
* 
*   SYNOPSIS
*   
* 
*   DESCRIPTION
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

#if defined(HAVE_VLT)
#include "vltPort.h"
#endif

static char *rcsId="@(#) $Id: MAStdFilter.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */

#include <string.h>
#include <stdio.h>
#include <limits.h>

/*
 * Local Headers 
 */

#include <aomath/MAStdFilter.h>

