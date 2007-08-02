#!/usr/bin/perl
#This script will add the ASTRON copyright to python/shell/perl script files
# $Id: addcopyright.pl 3746 2006-06-21 22:55:27Z sarod $

# check for command line
if (scalar(@ARGV) ==0) {
   &usage;
   exit(1);
}


#### patterns
$skip_lines='^\s$|^#.*$';
$old_copyright='^# GNU General Public License .*$';
$tmpfile_name="./.tmpfile";

for ($argnum=0; $argnum<=$#ARGV; $argnum++) {
printf "Adding ASTRON GPL to @ARGV[$argnum]\n";
# open files
open(IN,"< @ARGV[$argnum]") || die "can't open file @ARGV[$argnum] for reading\n";
open(OUTFILE,"> $tmpfile_name") || die "cant' open file $tmpfile_name for writing\n";

$written_header=0;
# read file
while (defined($sline = <IN>)) {
  if (!$written_header ) {
    if ($sline =~ /($skip_lines)/ ) {
     # print to output

     print OUTFILE $sline;
     if ( $sline =~ m/($old_copyright)/ ) {
      # copyright already exists
      printf "GPL already present in @ARGV[$argnum]\n";
      $written_header=1;
     }
    } else {
     # write the header here
     &print_copyright();
     $written_header=1;

     print OUTFILE $sline;
    } 
  } else {
    # we have already written header, copy input to output

     print OUTFILE $sline;
  }

}

# now we are at the end of file, if we still have not
# added the header, do so now

if (!$written_header ) {
     &print_copyright();
}

close (IN) || die "can't close input file $!";
close (OUTFILE) || die "can't close output file $!";

# copy new file to original location
$cmd="/bin/cp $tmpfile_name @ARGV[$argnum]";
system($cmd)==0  or die "can't update file @ARGV[$argnum] for reading\n";

}

# remove any temporary files created
unlink($tmpfile_name) or die "can't delete $tmpfile_name: $!\n";

##########################################################
#### help
sub usage {
  printf "Usage: $0 [input files]\n";
  printf "[input files] are the files you need to add copyright to\n";
}


##########################################################
#### print copyright
sub print_copyright {
  print OUTFILE "\n";
  print OUTFILE "#% \$Id\$ \n";
  print OUTFILE "\n";

  print OUTFILE "#
# Copyright (C) 2006
# ASTRON (Netherlands Foundation for Research in Astronomy)
# and The MeqTree Foundation
# P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg\@astron.nl
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
#\n";

  print OUTFILE "\n";
}
