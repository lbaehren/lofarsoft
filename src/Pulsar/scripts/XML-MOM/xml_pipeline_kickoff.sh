#!/bin/ksh 

USAGE="\nusage : xml_pipeline_kickoff.sh -infile obs_finished.xml -prefix prefix [ -splits number ] \n\n"\
"      -infile obs_finished.xml  ==> Specify the finished xml file name (i.e. Obs_B1254-10_HBA.xml) \n"\
"      -prefix run  ==> Output name prefix (i.e. Obs_20100730) \n"\
"      [-splits N]  ==> Number of output split files to contain the processing commands (i.e. 4) \n"\
"      [-survey]    ==> Indicates that all the observations are in survey mode;  changes pipeline args to:\n"\
"                       pulp.sh -id OBSID -o OBSID_red -p position -rfi   [without -all] \n"\
"      [-cep2]      ==> Indicates that processing will be on CEP2 cluster, distributed processing separates scripts.\n"\
"\n"\
"      Example:\n"\
"      xml_pipeline_kickoff.sh -infile Obs_B1254-10_HBA.xml -prefix Obs_20100730 -splits 4 \n"\
"      xml_pipeline_kickoff.sh -infile Obs_B1254-10_HBA.xml -prefix Obs_20100730 -splits 4 -survey \n"\
"      xml_pipeline_kickoff.sh -infile Obs_B1254-10_HBA.xml -prefix Obs_20100730 -survey \n"

infile=""
prefix=""
splits=1
survey=0
cep2=0

if [ $# -lt 4 ]                    
then
   print "$USAGE"    
   exit 1
fi

while [ $# -gt 0 ]
do
    case "$1" in
    -infile)    infile=$2; shift;;
	-prefix)    prefix=$2; shift;;
	-splits)    splits=$2; shift;;
	-survey)    survey=1;;
	-cep2)      cep2=1;;
	-*)
	    echo >&2 \
	    "$USAGE"
	    exit 1;;
	*)  break;;	
    esac
    shift
done


#temporary file
outfile=$prefix"_PipeExec"

if [ ! -f $infile ]
then
   echo "ERROR: $infile does not exist"
   exit 1
fi

if [ -f $outfile ]
then
   echo "WARNING: $outfile exists; overwriting!"
   rm -rf $outfile
fi 

# in one command, do the following steps:
# grab the observationId and name w/(.BA) string from the xml file
# add a "\" to the end of the line containing observationId
# paste the line with "\" and the next line together
# grep for lines which only contain observationId
# strip the actual ObsID and the Pulsar name (or Position) from this line
# create the run script for the shell script pipeline

# previous version to extract the OBSID and Target name
#egrep "observationId|\(.BA" $infile | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/\<observationId\>//' | sed 's/<>//' | sed 's/<.*Obs / /g' | sed 's/(.BA.*//g' | awk '{printf("pulp.sh -id L2011_%05d -p %s -o L2011_%05d_red -all -rfi\n", $1, $2, $1)}' > $outfile

if [[ $survey == 0 ]] && [[ $cep2 == 0 ]]
then
   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ if ( $3 == "Pos" ) printf("pulp.sh -id L%05d -p position -o L%05d_red -all -rfi\n", $1, $1); else if ( $2 == "Obs" ) printf("pulp.sh -id L2011_%05d -p %s -o L2011_%05d_red -all -rfi\n", $1, $3, $1) ; else printf("pulp.sh -id L2011_%05d -p %s -o L2011_%05d_red -all -rfi\n", $1, $2, $1)}' > $outfile
elif [[ $survey == 1 ]] && [[ $cep2 == 0 ]]
then
   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ printf("pulp.sh -id L2011_%05d -p position -o L2011_%05d_red -rfi\n", $1, $1)}' > $outfile
elif [[ $survey == 1 ]] && [[ $cep2 == 1 ]]
then
   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ printf("pulp.sh -id L%05d -p position -o L%05d_red -coh_only -raw /data/ -rfi\n", $1, $1)}' > $outfile
   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ printf("pulp.sh -id L%05d -p position -o L%05d_redIS -incoh_only -all -raw /data/ -rfi\n", $1, $1)}' >> $outfile
elif [[ $survey == 0 ]] && [[ $cep2 == 1 ]]
then
   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ if ( $3 == "Pos" ) printf("pulp.sh -id L%05d -p position -o L%05d_red -raw /data/ -coh_only -rfi\n", $1, $1); else if ( $2 == "Obs" ) printf("pulp.sh -id L%05d -p %s -o L%05d_red -all -raw /data/ -coh_only -rfi\n", $1, $3, $1) ; else printf("pulp.sh -id L%05d -p %s -o L%05d_red -raw /data/ -coh_only -rfi\n", $1, $2, $1)}' > $outfile
   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ if ( $3 == "Pos" ) printf("pulp.sh -id L%05d -p position -o L%05d_red -raw /data/ -incoh_only -rfi -all\n", $1, $1); else if ( $2 == "Obs" ) printf("pulp.sh -id L%05d -p %s -o L%05d_red -all -raw /data/ -incoh_only -rfi -all\n", $1, $3, $1) ; else printf("pulp.sh -id L%05d -p %s -o L%05d_red -raw /data/ -incoh_only -rfi -all\n", $1, $2, $1)}' >> $outfile
fi

cp $outfile $outfile.all

if [[ $cep2 == 0 ]]
then

	# perform the splitting up of the all files into multiples
	ii=0
	splits_list=""
	jj=$splits
	last=$jj
	while (( $jj > 0 ))
	do 
	    splits_list=`echo $jj $splits_list`
	    jj=`expr $jj - 1`
	done
	
	num_lines=`wc -l $outfile | awk '{print $1}'`
	
	modulo_files=`echo $num_lines $splits | awk '{print ($1 % $2)}'`
	if (( modulo_files != 0 ))
	then
	   echo "Warning: this is an uneven split; one file will contain more OBSIDs than the rest."
	fi
	
	div_files=`echo $num_lines $splits | awk '{printf("%d"), $1 / $2}'`
	
	# split the single file into multiples with the same number per split
	if (( $splits > 1 ))
	then
		for ii in $splits_list
		do
			head -$div_files $outfile > $outfile.$ii
			c=$div_files
			sed "1,$c d" $outfile > $outfile.tmp
			mv $outfile.tmp $outfile
		done
	fi
	
	# when there's an uneven split, concatinate the rest to the last split file
	if (( modulo_files != 0 ))
	then
	   cat $outfile >> $outfile.$ii
	   rm $outfile
	fi
else # if [[ $cep2 == 1 ]]
    ii=0
    rm -rf $outfile.all.sh
    while read line 
    do
        rm -rf $outfile.$ii
        obsid=`echo $line | sed 's/^.*-id //g' | awk '{print $1}'`  
        #create a shell script per line to run on locus nodes
        incoherent=`echo $line | grep incoh_only`
        if [[ $incoherent == "" ]]
        then
           echo 'cexec locus:0-99 "cd /data/LOFAR_PULSAR_ARCHIVE_locus*/; use LUS; ~alexov/'$line' -del"'  >> $outfile.$ii
           echo 'cexec locus:67 "cd /data/LOFAR_PULSAR_ARCHIVE_locus*/; mkdir -p /data/LOFAR_PULSAR_ARCHIVE_locus*/'${obsid}'_plots"' >> $outfile.$ii
           echo 'cexec locus:0-99  "scp /data/scratch/alexov/'${obsid}'_red/stokes/*/*th.png /data/scratch/alexov/'${obsid}'_red/stokes/*/*prepout /data/scratch/alexov/'${obsid}'_red/stokes/*/*/*th.png /data/scratch/alexov/'${obsid}'_red/stokes/*/*/*prepout '`whoami`'@locus068:/data/LOFAR_PULSAR_ARCHIVE_locus*/'${obsid}'_plots/"'   >> $outfile.$ii
           echo 'cexec locus:67 "cd /data/LOFAR_PULSAR_ARCHIVE_locus*/'${obsid}'_plots/; use LUS; ~alexov/thumbnail_combine.sh"'  >> $outfile.$ii
        else
           echo 'cexec locus:67 "cd /data/scratch/pulsar/; mkdir -p '${obsid}'"' >> $outfile.$ii
           echo 'cexec locus:0-99 "scp /data/${obsid}/*stokes '`whoami`'@locus068:/data/scratch/pulsar/'${obsid}'"'  >> $outfile.$ii
           echo 'cexec locus:67 "cd /data/LOFAR_PULSAR_ARCHIVE_locus*/; use LUS; ~alexov/'$line' -del"' >> $outfile.$ii
        fi
	    echo "./$outfile.$ii >& $outfile.$ii.log &" >> $outfile.all.sh
	    ii=`expr $ii + 1`
    done < $outfile.all
fi #end if [[ $cep2 == 0 ]]

rm $outfile

chmod 777 $outfile.*  $outfile.all.sh

if [[ $cep2 == 0 ]]
then
    echo "Single script '$outfile.all' contains all the processing commands in one file."
	if (( $splits > 1 ))
	then
	   echo "Run that file or, run the N split files:"
	   echo "`ls $outfile.[0-9]*`"
	   echo "in the processing locations."
	fi
else
   echo "Single script '$outfile.all.sh' contains all the processing commands in one file."
   echo "Copy that file and all the sub-scripts ($outfile.*) to activate processing."
fi

