#! /usr/bin/env python

############################### BSD License ################################
# Copyright (c) 2010, Pim Schellart                                        #
# All rights reserved.                                                     #
#                                                                          #
# Redistribution and use in source and binary forms, with or without       #
# modification, are permitted provided that the following conditions are   #
# met:                                                                     #
#                                                                          #
#     * Redistributions of source code must retain the above copyright     #
#       notice, this list of conditions and the following disclaimer.      #
#     * Redistributions in binary form must reproduce the above copyright  #
#       notice, this list of conditions and the following disclaimer in    #
#       the documentation and/or other materials provided with the         #
#       distribution.                                                      #
#     * Neither the name of the Radboud University Nijmegen nor the        #
#       names of its contributors may be used to endorse or promote        #
#       products derived from this software without specific prior written #
#       permission.                                                        #
#                                                                          #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS      #
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT        #
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  #
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Pim Schellart BE    #
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR      #
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF     #
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS #
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN  #
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)  #
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF   #
# THE POSSIBILITY OF SUCH DAMAGE.                                          #
############################################################################

import sys
import pyfits
import numpy as np
from optparse import OptionParser

# Check for kapteyn package which is not in the USG repository.
try:
    from kapteyn import wcs
    from kapteyn import celestial
except ImportError:
    print "To run this program you need to have the kapteyn package installed."
    sys.exit(1)

# Parse commandline arguments
parser=OptionParser(usage="usage: %prog [options] filename.fits", version="%prog 1.0")
parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="verbose output")
(options, args)=parser.parse_args()

if not args:
    parser.print_help()
    sys.exit(0)

# Open 3-dimensional FITS file
try:
    hdr = pyfits.getheader(args[0])
except IOError:
    print "Error: Input file is not a valid FITS file"
    sys.exit(1)

# Create Projection object
proj3 = wcs.Projection(hdr, usedate=True)

# 2D projection
proj2 = proj3.sub([1,2])

# Create pixel grid
x = np.arange(1, hdr['NAXIS1']+1)
y = np.arange(1, hdr['NAXIS2']+1)
X, Y = np.meshgrid(x,y)

pixel = (X.ravel(), Y.ravel())

# Get WCS world coordinates for pixels
world = proj2.toworld(pixel)

# Reshape grid back to 2*NX*NY
wx = np.asarray(world[0]).reshape(hdr['NAXIS1'], hdr['NAXIS2'])
wy = np.asarray(world[1]).reshape(hdr['NAXIS1'], hdr['NAXIS2'])

coordinates = np.dstack((wx,wy))

# Give some more information
if options.verbose:
    print 'Grid shape:', coordinates.shape

    corners = [(1, 1),
               (1, hdr['NAXIS1']),
               (hdr['NAXIS2'], 1),
               (hdr['NAXIS2'], hdr['NAXIS1'])]

    print "Coordinates:"
    for c in corners:
        print c, '=', celestial.lon2hms(c[0]), celestial.lat2dms(c[1])

# Save grid as binary numpy array
np.save(args[0].rstrip('.fits')+'.npy', coordinates)

