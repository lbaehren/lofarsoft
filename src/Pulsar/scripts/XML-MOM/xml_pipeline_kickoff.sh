#!/bin/ksh 

USAGE="\nusage : xml_pipeline_kickoff.sh -infile obs_finished.xml -prefix prefix [ -splits number ] \n\n"\
"      -infile obs_finished.xml  ==> Specify the finished xml file name (i.e. Obs_B1254-10_HBA.xml) \n"\
"      -prefix run  ==> Output name prefix (i.e. Obs_20100730) \n"\
"      [-splits N]  ==> Number of output split files to contain the processing commands (i.e. 4) \n"\
"      [-survey]    ==> Indicates that all the observations are in survey mode;  changes pipeline args to:\n"\
"                       pulp.sh -id OBSID -o OBSID_red -p position -rfi   [without -all] \n"\
"      [-cep2]      ==> Indicates that processing will be on CEP2 cluster, distributed processing separates scripts.\n"\
"      [-noxml]     ==> Indicates that you are not starting with an xml file, but pulp_cep2.sh parameters.\n"\
"      [-hoover_only]  ==> Usually used only with -noxml switch; runs only the hoover/collection part of 'coh_only' pulp_cep2 pipeline. \n"\
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
noxml=0
hoover_only=0

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
	-noxml)     noxml=1;;
	-hoover_only)     hoover_only=1;;
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

if [[ $noxml == 0 ]]
then
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
else 
    outfile=$infile
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

if [[ $noxml == 0 ]]
then

	if [[ $survey == 0 ]] && [[ $cep2 == 0 ]]
	then
	   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ if ( $3 == "Pos" ) printf("pulp.sh -id L%05d -p position -o L%05d_red -all -rfi\n", $1, $1); else if ( $2 == "Obs" ) printf("pulp.sh -id L2011_%05d -p %s -o L2011_%05d_red -all -rfi\n", $1, $3, $1) ; else printf("pulp.sh -id L2011_%05d -p %s -o L2011_%05d_red -all -rfi\n", $1, $2, $1)}' > $outfile
	elif [[ $survey == 1 ]] && [[ $cep2 == 0 ]]
	then
	   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ printf("pulp.sh -id L2011_%05d -p position -o L2011_%05d_red -rfi\n", $1, $1)}' > $outfile
	elif [[ $survey == 1 ]] && [[ $cep2 == 1 ]]
	then
	   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ printf("pulp.sh -id L%05d -p position -o L%05d_red -coh_only -raw /data/ -rfi\n", $1, $1)}' > $outfile
	   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ printf("pulp.sh -id L%05d -p position -o L%05d_redIS -incoh_only -all -raw \"/cep2/locus???_data/\" -rfi\n", $1, $1)}' >> $outfile
	elif [[ $survey == 0 ]] && [[ $cep2 == 1 ]]
	then
	   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ if ( $3 == "Pos" ) printf("pulp.sh -id L%05d -p position -o L%05d_red -raw /data/ -coh_only -rfi\n", $1, $1); else if ( $2 == "Obs" ) printf("pulp.sh -id L%05d -p %s -o L%05d_red -all -raw /data/ -coh_only -rfi\n", $1, $3, $1) ; else printf("pulp.sh -id L%05d -p %s -o L%05d_red -raw /data/ -coh_only -rfi\n", $1, $2, $1)}' > $outfile
	   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/<observationId>//' | sed 's/<>//' | sed 's/</ </g' | sed 's/>/> /g' | sed 's/(.*//g' | sed 's/<.*>//g' | awk '{ if ( $3 == "Pos" ) printf("pulp.sh -id L%05d -p position -o L%05d_red -raw \"/cep2/locus???_data/\" -incoh_only -rfi -all\n", $1, $1); else if ( $2 == "Obs" ) printf("pulp.sh -id L%05d -p %s -o L%05d_red -all -raw \"/cep2/locus???_data/\" -incoh_only -rfi -all\n", $1, $3, $1) ; else printf("pulp.sh -id L%05d -p %s -o L%05d_red -raw \"/cep2/locus???_data/\" -incoh_only -rfi -all\n", $1, $2, $1)}' >> $outfile
	fi

   cp $outfile $outfile.all
fi


if [[ $cep2 == 0 ]] && [[ $noxml == 0 ]]
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
    rm -rf $outfile.*.sh
    while read line 
    do
        obsid=`echo $line | sed 's/^.*-id //g' | awk '{print $1}'` 
        echo "Working on OBSID $obsid..."
        #create a shell script per line to run on locus nodes;
        #note that single and double quotes need to be VERY exact for cexec commands and wildcards;
        #a large amount of effort was put into making all the quotes correct to farm out processing;
        #be very careful when you change anything in this section!  you have been warned!
        incoherent=`echo $line | grep incoh_only`
        if [[ $incoherent == "" ]]
        then
           # CS stokes processing
           if [[ $hoover_only == 0 ]]
              echo 'cexec locus:0-99 "cd /data/LOFAR_PULSAR_ARCHIVE_locus*/; '$line' -del"'  >> $outfile.$obsid.CS.sh
           else
              echo '#cexec locus:0-99 "cd /data/LOFAR_PULSAR_ARCHIVE_locus*/; '$line' -del"'  >> $outfile.$obsid.CS.sh
           fi
           echo 'cexec hoover:0 cd /data/LOFAR_PULSAR_ARCHIVE_locus101/; rm -rf '${obsid}'_CSplots;  mkdir -p '${obsid}'_CSplots ; cd '${obsid}'_CSplots ; mount_locus_nodes.sh;  cat /cep2/locus???_data/LOFAR_PULSAR_ARCHIVE_locus???/'${obsid}'_red/*log >> '${obsid}'_combined.log  ;  cat /cep2/locus???_data/LOFAR_PULSAR_ARCHIVE_locus???/'${obsid}'_red/stokes/beam_process_node.txt >> beam_process_node.txt ; ls /cep2/locus???_data/LOFAR_PULSAR_ARCHIVE_locus???/'${obsid}'_red/*tar.gz | grep tar | sed -e "s/^/tar xvzf /g" > untar.sh; chmod 777 untar.sh ; ./untar.sh ; rm untar.sh' |  sed -e "s/:0 /:0 \'/" -e "s/rm untar.sh/rm untar.sh\'/"  >> $outfile.$obsid.CS.sh      
           echo 'cexec hoover:0 "cd /data/LOFAR_PULSAR_ARCHIVE_locus101/'${obsid}'_CSplots/; thumbnail_combine.sh; lofar_status_map.py"'  >> $outfile.$obsid.CS.sh
           echo 'cexec hoover:0 cd /data/LOFAR_PULSAR_ARCHIVE_locus101/'${obsid}'_CSplots/ ; tar cvzf '${obsid}'_combinedCS_nopfd.tar.gz `find ./ -type f \( -name \*.pdf -o -name \*.ps -o -name \*.inf -o -name \*.rfirep -o -name \*.png -name \*.parset \)`end' | sed -e "s/:0 /:0 \'/" -e "s/end/\'/" >> $outfile.$obsid.CS.sh
	       echo "./$outfile.$obsid.CS.sh > $outfile.$obsid.CS.log &" >> $outfile.all.sh
        else  
           # IS stokes processing
           echo 'cexec hoover:1 cd /data/LOFAR_PULSAR_ARCHIVE_locus102/; mount_locus_nodes.sh; '$line' -del' | sed -e "s/ cd/ \'cd/" -e "s/del/del\'/g" >> $outfile.$obsid.IS.sh  
	       echo "./$outfile.$obsid.IS.sh > $outfile.$obsid.IS.log &" >> $outfile.all.sh
        fi
	    ii=`expr $ii + 1`
    done < $outfile.all
fi #end if [[ $cep2 == 0 ]]

rm -rf $outfile 

chmod 777 $outfile.*  $outfile.all.sh

if [[ $cep2 == 0 ]] && [[ $noxml == 0 ]]
then
    echo "Single script '$outfile.all' contains all the processing commands in one file."
	if (( $splits > 1 ))
	then
	   echo "Run that file or, run the N split files:"
	   echo "`ls $outfile.[0-9]*`"
	   echo "in the processing locations."
	fi
elif [[ $noxml == 0 ]]
then
   rm $outfile.all
   echo "Single script '$outfile.all.sh' contains all the processing commands in one file."
   echo "Copy that file and all the sub-scripts ($outfile.OBSIDnumber*) to activate processing."
elif [[ $noxml == 1 ]]
then
   #run the processing
   echo "Running the following task(s) in the background;  log files are empty until tasks are completed:"
   cat $outfile.all.sh
   ./$outfile.all.sh
fi

