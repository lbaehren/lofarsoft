##########################################################################
#  This file is part of the Transient Template Library.                  #
#  Copyright (C) 2010 Pim Schellart <P.Schellart@astro.ru.nl>            #
#                                                                        #
#  This library is free software: you can redistribute it and/or modify  #
#  it under the terms of the GNU General Public License as published by  #
#  the Free Software Foundation, either version 3 of the License, or     #
#  (at your option) any later version.                                   #
#                                                                        # 
#  This library is distributed in the hope that it will be useful,       #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#  GNU General Public License for more details.                          #
#                                                                        #
#  You should have received a copy of the GNU General Public License     #
#  along with this library. If not, see <http://www.gnu.org/licenses/>.  #
##########################################################################

# Make subpackages available when doing from ttl import *
__all__ = ['beamforming', 'coordinates', 'fft']

# Make content of subpackages available when calling from ttl import *
def extend_all(module):
    adict = {}
    for a in __all__:
        adict[a] = 1
    try:
        mall = getattr(module, '__all__')
    except AttributeError:
        mall = [k for k in module.__dict__.keys() if not k.startswith('_')]
    for a in mall:
        if a not in adict:
            __all__.append(a)

import beamforming
import coordinates
import fft

extend_all(beamforming)
extend_all(coordinates)
extend_all(fft)

# Make content of subpackages available when calling import ttl
from beamforming import *
from coordinates import *
from fft import *

