/*
     arch.h     R. Hughes-Jones  The University of Manchester

     define the architecture IA32 or IA64 and byte swapping


*/
/*
   Copyright (c) 2002,2003,2004 Richard Hughes-Jones, University of Manchester
   All rights reserved.

   Redistribution and use in source and binary forms, with or
   without modification, are permitted provided that the following
   conditions are met:

     o Redistributions of source code must retain the above
       copyright notice, this list of conditions and the following
       disclaimer. 
     o Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials
       provided with the distribution. 

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/
/*
     Date: 01/Sep/03 
     Version 1.0                           
     Modification: 

      Version libhj-4.0.3 
        rich  29 Jun 05 Add AMD64

      Version 3.2-6      
        rich  30 Jan 04 Move IA32 / IA64 deffinition to -D
	                Correct IA64 word lengths 
                        Add i8swap macro 

*/

#ifndef IA_ARCHITECTURE
#define IA_ARCHITECTURE true


/* #define IA32 and IA64 defined by ARCH environmental variable 
   and passed by -D  */

/* #define BYTE_SWAP_ON true */

/* ------------- 32 bit or 64 bit architecture ----------- */
#ifdef AMD64
/* for AMD 64 bit architecture */
typedef int int32;
typedef long int64;
#define LONG_FORMAT "l"
#endif    /* ifdef AMD64 */

#ifdef IA64
/* for IA64 architecture */
typedef int int32;
typedef long int64;
#define LONG_FORMAT "l"
#endif    /* ifdef IA64 */

#ifdef IA32
/* for IA32 architecture */
typedef int int32;
typedef long long int64;
#define LONG_FORMAT "L"
#endif    /* ifdef IA32 */

/* ----------------- byte swapping ----------------------- */

#ifdef BYTE_SWAP_ON
#define  i4swap( value)         \
        ( (value &0xff) <<24 | (value &0xff00)<<8 | (value &0xff0000)>>8 | (value &0xff000000) >>24 )

#define  i8swap( value)         \
        ( (value &0xff) <<56 | (value &0xff00)<<40 | (value &0xff0000)<<24 | (value &0xff000000) <<8 | \
          (value &0xff00000000) >>8 | (value &0xff0000000000)>>24 | (value &0xff000000000000)>>40 | (value &0xff00000000000000)>>56 )
#else
#define  i4swap
#define  i8swap
#endif      /* ifdef BYTE_SWAP_ON */


#endif      /* IA_ARCHITECTURE */
