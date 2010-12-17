#!/bin/ksh

#find all the th.png files and convert them into a list to paste together using "montage".

if [ -f combined.th.png ]
then 
   echo "WARNING: deleting previous version of results: combined.th.png"
   rm combined.th.png
fi

# find all the paths to the th.png files
find ./ -name "*.th.png" -print  > /tmp/$$_combine_col1.txt

# find all the paths to the th.png files and print just the RSP? location and the Pulsar name to the output file
#find ./ -name "*.th.png" -print  | sed -e 's/\// /g' -e 's/^.* //g' -e 's/.*_RSP/RSP/g' -e 's/\..*//g'  -e 's/_PSR//g' > /tmp/$$_combine_col2.txt

# find all the paths to the th.png files and print just the IS (incoherentstokes) or CS (stokes) 
#   and RSP? location and the Pulsar name to the output file
 find ./ -name "*.th.png" -print | sed -e 's/\/incoherentstokes\//_IS_/g' -e 's/\/stokes\//_CS_/g' -e 's/\//_/g' -e 's/^.*_IS/IS/g' -  's/^.*_CS/CS/g' -e 's/_L20.*//g' -e 's/_RSP._/&\\n/g' > /tmp/$$_combine_col2.txt
paste /tmp/$$_combine_col1.txt /tmp/$$_combine_col2.txt | awk '{print "-label \""$2"\" "$1" "}' | tr -d '\n' | awk '{print "montage -background none "$0" combined.th.png"}' > combine_png.sh
rm /tmp/$$_combine_col1.txt /tmp/$$_combine_col2.txt
wc_convert=`wc -l combine_png.sh | awk '{print $1}'`

if [[ $wc_convert > 0 ]]
then
   chmod 777 combine_png.sh
   echo "Executing the following comamnd: "
   cat combine_png.sh
   ./combine_png.sh
   echo ""
   echo "Results:  combined.th.png"
   echo ""
else
   echo ""
   echo "No thumbnail (*.th.png) files were found to combine."
   echo ""
fi

exit 0
