########################################################################
########################################################################
#  Global definitions for the lopestools have been moved to other files.
#  This file is mainly for backward compatibility and is just loading other files!!
#
#  The parameters in "globalconst" and "globalpar" are now
#  defined in globals-const.g and globals-par.g
# 
# (In other words a standard installation should work without changes
# from the cvs-server. Any changed pathnames etc. should be put into
# "globalconst.LocalGlobal" and _not_ put onto the cvs-server! See also
# "global-local.g.example".)
########################################################################
########################################################################

#include add_dir('globals-const.g')
include add_dir('globals-par.g')
include add_dir('globals-par-cr.g')

########################################################################
#
#Load local adjustments
#
########################################################################
tmp := stat(globalconst.LocalGlobal);
if (has_field(tmp,'type')&&(tmp.type=='ascii')) {include globalconst.LocalGlobal}

