#!/bin/csh -f

if ( "$1" == "" ) then

cat <<EOF

  apsr_rm.csh - use with loopssh to move \$1* from 

    /lfs/data0/apsr/rawdata/unprocessable 

  to

    /lfs/data0/apsr/rawdata

  e.g. loopssh apsr 0 15 apsr_reprocess.csh 2008-10-17

EOF

  exit 0

endif

echo "Moving /lfs/data0/apsr/rawdata/unprocessable/$1"'*'
cd /lfs/data0/apsr/rawdata/unprocessable
mv $1* ../

