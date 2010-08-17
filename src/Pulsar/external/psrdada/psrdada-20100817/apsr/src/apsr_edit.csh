#!/bin/csh -f

if ( "$2" == "" ) then

cat <<EOF

apsr_edit.csh - use with loopssh to edit \$1* in /lfs/data0/apsr/scratch

e.g. loopssh apsr 0 15 apsr_edit.csh 2008-10-11-04:30:10 NBIT=8

EOF

  exit 0

endif

echo "Running dada_edit -c $2 /lfs/data0/apsr/scratch/$1"'*'

cd /lfs/data0/apsr/scratch/
dada_edit -c $2 $1*

