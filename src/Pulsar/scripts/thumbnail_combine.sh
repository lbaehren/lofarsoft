#!/bin/ksh

#find all the th.png files and convert them into a list to paste together using "montage".
#generally used in the Pulsar Pipeline, at the top level OBSID_red (processing location)
#optional input argument is the pipeline logfile, where STDOUT statements are repeated

# version 2.0

log="NONE"

if [ $# -eq 1 ]                    # get the log file name
then
   log=$1
fi

if [ $log != "NONE" ]
then
   echo "thumbnail_combine.sh script is writing to the following log file: $log"
   echo "thumbnail_combine.sh script is writing to the following log file: $log" >> $log
fi

if [ -f combined.th.png ]
then 
   echo "WARNING: deleting previous version of results: combined.th.png"
   if [ $log != "NONE" ]
   then
      echo "WARNING: deleting previous version of results: combined.th.png" >> $log
   fi
   rm combined.th.png
fi

if [ -f combined.png ]
then 
   echo "WARNING: deleting previous version of results: combined.png"
   if [ $log != "NONE" ]
   then
      echo "WARNING: deleting previous version of results: combined.png" >> $log
   fi
   rm combined.png
fi

if [ -f combined.pdf ]
then 
   echo "WARNING: deleting previous version of results: combined.pdf"
   if [ $log != "NONE" ]
   then
      echo "WARNING: deleting previous version of results: combined.pdf" >> $log
   fi
   rm combined.pdf
fi

if [ -f combined.th.pdf ]
then 
   echo "WARNING: deleting previous version of results: combined.th.pdf"
   if [ $log != "NONE" ]
   then
      echo "WARNING: deleting previous version of results: combined.th.pdf" >> $log
   fi
   rm combined.th.pdf
fi

if [ -f chi-squared.txt ]
then 
   echo "WARNING: deleting previous version of results: chi-squared.txt"
   if [ $log != "NONE" ]
   then
      echo "WARNING: deleting previous version of results: chi-squared.txt" >> $log
   fi
   rm chi-squared.txt
fi

# find all the paths to the th.png files
find ./ -iname "*.th.png" ! -iname "*status*png" ! -iname "*_nomask*png" ! -iname "*_B.png" ! -iname "*_DFTp.png" ! -iname "*_GTpf*" ! -iname "*_YFp.png" -print | sort -d > /tmp/$$_combine_col1.txt
wc_col1=`wc -l /tmp/$$_combine_col1.txt | awk '{print $1}'`
if [[ $wc_col1 < 1 ]]
then
   echo ""
   echo "No thumbnail (*.th.png) files were found to combine."
   echo ""

   if [ $log != "NONE" ]
   then
      echo "No thumbnail (*.th.png) files were found to combine." >> $log
   fi
fi

find ./ -iname "*prepout" ! -iname "*_nomask*" ! -iname "*_B.png" ! -iname "*_DFTp.png" ! -iname "*_GTpf*" ! -iname "*_YFp.png" -print -exec grep -i squared {} \; | sed 's/prepout/prepout \\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | sort -d | sed 's/^.*= //g'  > /tmp/$$_combine_col3.txt
wc_col3=`wc -l /tmp/$$_combine_col3.txt | awk '{print $1}'`
if [[ $wc_col3 < 1 ]]
then
   echo ""
   echo "No presto (*.prepout) files were found to extract Chi-Squared values."
   echo ""

   if [ $log != "NONE" ]
   then
      echo "No presto (*.prepout) files were found to extract Chi-Squared values." >> $log
   fi
fi


# find all the paths to the th.png files and print just the RSP? location and the Pulsar name to the output file
#find ./ -name "*.th.png" -print  | sed -e 's/\// /g' -e 's/^.* //g' -e 's/.*_RSP/RSP/g' -e 's/\..*//g'  -e 's/_PSR//g' > /tmp/$$_combine_col2.txt

# find all the paths to the th.png files and print just the IS (incoherentstokes) or CS (stokes) 
#   and RSP? location and the Pulsar name to the output file


#find ./ -name "*.th.png" -print | sort | sed -e 's/\/incoherentstokes\//_IS_/g' -e 's/\/stokes\//_CS_/g' -e 's/\//_/g' -e 's/^.*_IS_/IS_/g' -e 's/^.*_CS_/CS_/g' -e 's/_L20.*//g' -e 's/_RSP._/&\\n/g' -e 's/_beam_./\\n&\\n/g'  > /tmp/$$_combine_col2.txt

#is_IS=`grep "incoherentstokes/" /tmp/$$_combine_col1.txt`
#if [[ $is_IS != "" ]]
#then
#   STOKES="IS"
#else
#   STOKES="CS"
#fi

#find ./ -name "*.th.png" -print | sort | sed -e "s/\// /g" -e "s/_/ /g" -e "s/^.*L20..//g" -e "s/\..*//g" | awk -v STOKES=$STOKES '{print STOKES"_"$2"\\n_"$4}' > /tmp/$$_combine_col2.txt


#assume anything without long paths which contain incoherentstokes or stokes, is CS data for now
find ./ -iname "*.th.png" ! -iname "*status*png" ! -iname "*_nomask*.th.png" ! -iname "*_B.png" ! -iname "*_DFTp.png" ! -iname "*_GTpf*" ! -iname "*_YFp.png" -print | sort -d | sed -e "s/\// /g" -e "s/_/ /g" -e "s/incoherentstokes/ IS_/g" -e 's/stokes/ CS_/g' -e "s/\.pfd.*//g" -e "s/\.//g" -e "s/_ .*L20.. /_ /g" -e "s/^.* L20.. / CS_ /g" -e "s/RSP/_RSP/g" | awk '{if (NF == 7) print $1$2"\\n_"$7; else if (NF == 5) print $1"_"$3"\\n"$5;  else print $1"_"$2"\\n_"$5}' > /tmp/$$_combine_col2.txt

find ./ -iname "*.th.png" ! -iname "*status*png" ! -iname "*_nomask*.th.png" ! -iname "*_B.png" ! -iname "*_DFTp.png" ! -iname "*_GTpf*" ! -iname "*_YFp.png" -print | sort -d |  sed -e "s/^.*beam/beam/g" -e "s/\/.*//g" -e "s/\.//g" | awk '{print "\\n"$1}' > /tmp/$$_combine_col4.txt

# create a shell script which needs to be run to create the combined plots
# create an ascii file which stores the results Chi-Squared for data statistics
has_beams=`grep beam /tmp/$$_combine_col4.txt`
if [[ $has_beams != "" ]]
then
   paste /tmp/$$_combine_col1.txt /tmp/$$_combine_col4.txt /tmp/$$_combine_col2.txt /tmp/$$_combine_col3.txt | awk '{print "-label \""$3" "$2"\\nChiSq = " $4"\" "$1" "}' | tr -d '\n' | awk '{print "montage -background none "$0" combined.png"}' > combine_png.sh
   paste /tmp/$$_combine_col1.txt /tmp/$$_combine_col4.txt /tmp/$$_combine_col2.txt /tmp/$$_combine_col3.txt |  sed 's/\\n//g' | awk '{print "file="$1" beam="$2" obs="$3" chi-sq="$4}'   > chi-squared.txt
   paste /tmp/$$_combine_col1.txt /tmp/$$_combine_col4.txt /tmp/$$_combine_col2.txt /tmp/$$_combine_col3.txt | sed 's/\.pfd\.th\.png/\.pfd\.pdf/g'  | awk '{print "-label \""$3" "$2"\\nChiSq = " $4"\" "$1" "}' | tr -d '\n' | awk '{print "montage "$0" -geometry 100% -rotate 90 -adjoin -tile 1x1 -pointsize 12 combined.pdf"}' > combine_pdf.sh
else
   paste /tmp/$$_combine_col1.txt /tmp/$$_combine_col2.txt /tmp/$$_combine_col3.txt | awk '{print "-label \""$2"\\nChiSq = " $3"\" "$1" "}' | tr -d '\n' | awk '{print "montage -background none -pointsize 10.2 "$0" combined.png"}' > combine_png.sh
   paste /tmp/$$_combine_col1.txt /tmp/$$_combine_col2.txt /tmp/$$_combine_col3.txt |  sed 's/\\n//g' | awk '{print "file="$1" obs="$2" chi-sq="$3}' > chi-squared.txt
   paste /tmp/$$_combine_col1.txt /tmp/$$_combine_col2.txt /tmp/$$_combine_col3.txt | sed 's/\.pfd\.th\.png/\.pfd\.pdf/g' | awk '{print "-label \""$2"\\nChiSq = " $3"\" "$1" "}' | tr -d '\n' | awk '{print "montage "$0" -geometry 100% -rotate 90 -adjoin -tile 1x1 -pointsize 12 combined.pdf"}' > combine_pdf.sh
fi

#delete intermediate files
rm /tmp/$$_combine_col1.txt /tmp/$$_combine_col2.txt /tmp/$$_combine_col3.txt /tmp/$$_combine_col4.txt

#make sure there is text in the new shell script
wc_convert=`wc -l combine_png.sh | awk '{print $1}'`

if [[ $wc_convert > 0 ]]
then
   chmod 777 combine_png.sh combine_pdf.sh
   echo "Executing the following comamnd: "

   if [ $log != "NONE" ]
   then
      echo "Executing the following comamnd: " >> $log
      cat combine_png.sh >> $log
      cat combine_pdf.sh >> $log
   fi

   cat combine_png.sh
   ./combine_png.sh
   convert -resize 200x140 -bordercolor none -border 150 -gravity center -crop 200x140-0-0 +repage combined.png combined.th.png
   chmod 775 combined.png combined.th.png
   echo ""
   echo "Results:  combined.png (large scale) and combined.th.png (thumbnail for the web summaries)"
   echo ""

   cat combine_pdf.sh
   ./combine_pdf.sh
   convert -resize 200x140 -bordercolor none -border 150 -gravity center -crop 200x140-0-0 +repage combined.pdf combined.th.pdf
   chmod 775 combined.pdf combined.th.pdf
   echo ""
   echo "Results:  combined.pdf (large scale) and combined.th.pdf (thumbnail for the web summaries)"
   echo ""

   if [ $log != "NONE" ]
   then
      echo "Results:  combined.png (large scale) and combined.th.png (thumbnail for the web summaries)" >> $log
      echo "Results:  combined.pdf (large scale) and combined.th.pdf (thumbnail for the web summaries)" >> $log
   fi

else
   echo ""
   echo "No thumbnail (*.th.png) files were found to combine."
   echo ""

   if [ $log != "NONE" ]
   then
      echo "No thumbnail (*.th.png) files were found to combine." >> $log
   fi

fi

exit 0
