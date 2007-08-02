
#% $Id$ 

#
# Copyright (C) 2006
# ASTRON (Netherlands Foundation for Research in Astronomy)
# and The MeqTree Foundation
# P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

include 'autoflag.g'

infile:=0;
# clipping threshold
limm:=1e5;
# uv clip limit
limuv:=70;


### parse args
for( a in argv )
{
  print 'arg: ',a;
  if( a =~ s/ms=// )
    infile:= a;
  else if( a =~ s/minuv=// )
    limuv:=as_integer(a);
  else if( a =~ s/minclip=// )
    limm:=as_float(a);

}

print spaste("Preprocessing:::",infile);


af:=autoflag(infile)
af.setdata()
cliprec:=[=]
cliprec[1] := [expr='ABS XX',min=1e-6,max=limm]
cliprec[2] := [expr='ABS YY',min=1e-6,max=limm]
cliprec[3] := [expr='ABS XY',min=1e-6,max=limm]
cliprec[4] := [expr='ABS YX',min=1e-6,max=limm]
cliprec[5] := [expr='UVD',min=limuv]
af.setselect(clip=cliprec)
af.run(reset=T)
af.done()

## median flag
af:=autoflag(infile)
af.setdata()
af.settimemed(thr=6,hw=5, expr='ABS XX')
af.settimemed(thr=6,hw=5, expr='ABS XY')
af.settimemed(thr=6,hw=5, expr='ABS YX')
af.settimemed(thr=6,hw=5, expr='ABS YY')
af.run()
af.done()


## special flags, depenging on the measurement set
#af:=autoflag(infile)
#af.setdata()
#af.setselect(ant=15)
#af.run()
#af.done()


## open with imager
include 'imager.g'
ii:=imager(infile);
ii.done()


print spaste("Done Preprocessing:::",infile);
exit
