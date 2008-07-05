"""Copyright (C) 2003 Association of Universities for Research in Astronomy (AURA)

print numarray.__LICENSE__    for the terms of use

or see LICENSE.txt in the numarray installation directory.
"""

__LICENSE__ = """\
Copyright (C) 2001,2002,2003  Association of Universities for Research in Astronomy (AURA)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    3. The name of AURA and its representatives may not be used to
      endorse or promote products derived from this software without
      specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AURA ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AURA BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

"""            

import os as _os

from numarrayall import *
from numinclude import version as __version__
from numinclude import hasUInt64 as _hasUInt64

import _conv
import _sort
import _bytes
import _ufunc
import _ufuncBool
import _ufuncInt8
import _ufuncUInt8
import _ufuncInt16
import _ufuncUInt16
import _ufuncInt32
import _ufuncUInt32
import _ufuncFloat32
import _ufuncFloat64
import _ufuncComplex32
import _ufuncComplex64
import _ndarray
import _numarray
import _chararray
import _objectarray
import memory
import _converter
import _operator
import _numerictype
import libnumarray
import libnumeric
import _ufuncInt64

if _hasUInt64:
	import _ufuncUInt64



