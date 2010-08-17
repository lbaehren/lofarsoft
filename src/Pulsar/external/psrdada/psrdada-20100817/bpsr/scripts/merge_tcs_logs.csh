#!/bin/bash -f

#
# merges info from $2/$1=<UTC>_bpsr.log into $2/<beam>/obs.start
#

dir=$1
file=$2

cd $dir

for beam in `seq 1 13` ; do
  beam=`echo $beam | awk '{printf("%02d",$1)}'`
  grep -e 'beam_all' -e "beam_$beam" $file > tcs.$beam
  cat tcs.$beam >> $beam/obs.start
  ref_beam=`grep refbeam $file | head -n 1 | awk '{printf("%02d",$3)}'`

  xml_file="$beam/"`echo $file | sed -e 's:\(.*\)_bpsr.log:\1.psrxml:g'`
  centre_xml_file="$ref_beam/"`echo $file | sed -e 's:\(.*\)_bpsr.log:\1.psrxml:g'`

echo "xf $xml_file cxf $centre_xml_file"

  psrxml_modify_key $xml_file receiver_beam $beam


  utc=`echo $file | sed -e 's:\(.*\)_bpsr.log:\1:g' | \
      sed -e 's%\(....-..-..\)-\(..:..:..\)%\1T\2Z%g'`

  psrxml_modify_key $xml_file utc $utc

  localtime=`echo $file | sed -e 's:\(.*\)_bpsr.log:\1:g' | \
      sed -e 's%\(....-..-..\)-\(..:..:..\)%\1 \2+00%g'`
  localtime=`date -d "$localtime" +%FT%T%:::z`
  psrxml_modify_key $xml_file local_time $localtime

  lst=`grep lst $file | head -n 1 | awk '{printf("%f", $3/3.141592654)}'`
  lst_h=`echo $lst | awk '{printf("%02d",$1*12.0)}'`
  lst_m=`echo $lst $lst_h | awk '{printf("%02d",($1*12-$2)*60)}'`
  lst_s=`echo $lst $lst_h $lst_m | awk '{printf("%04.1f",(($1*12-$2)*60 - $3)*60)}'`
  lst=$lst_h":"$lst_m":"$lst_s
  psrxml_modify_key $xml_file lst $lst

  tobs=`grep obsval $file | head -n 1 | awk '{print $3}'`
  tunit=`grep obsunit $file | head -n 1 | awk '{print $3}'`
  if test x$tunit = xCYCLES ; then
    tobs=`echo $tobs | awk '{print $1 * 10.0}'`
  fi

  psrxml_modify_key $xml_file requested_obs_time $tobs "units='s'"



  az=`grep az $file | head -n 1 | awk '{printf("%f", 180.0*$3/3.141592654)}'`
  el=`grep el $file | head -n 1 | awk '{printf("%f", 180.0*$3/3.141592654)}'`
  psrxml_modify_key $xml_file az $az "units='degrees'" start_telescope_position
  psrxml_modify_key $xml_file el $el "units='degrees'" start_telescope_position

  az=`grep az $file | tail -n 1 | awk '{printf("%f", 180.0*$3/3.141592654)}'`
  el=`grep el $file | tail -n 1 | awk '{printf("%f", 180.0*$3/3.141592654)}'`
  psrxml_modify_key $xml_file az $az "units='degrees'" end_telescope_position
  psrxml_modify_key $xml_file el $el "units='degrees'" end_telescope_position

  para=`grep para $file | head -n 1 | awk '{printf("%f", 180.0*$3/3.141592654)}'`
  psrxml_modify_key $xml_file start_paralactic_angle $para "units='degrees'"

  para=`grep para $file | tail -n 1 | awk '{printf("%f", 180.0*$3/3.141592654)}'`
  psrxml_modify_key $xml_file end_paralactic_angle $para "units='degrees'"
 
  observer=`grep observer $file | head -n 1 | awk '{print $3}'`
  psrxml_modify_key $xml_file observer_name $observer

  programme=`grep project $file | head -n 1 | awk '{print $3}'`
  psrxml_modify_key $xml_file observing_programme $programme

#  paralactify=`grep beam_shift $file | head -n 1 | \
#    awk '{if($3=="T")print "TRUE"; else print "FALSE"}'`
#  psrxml_modify_key $xml_file paralactic_angle_tracking $paralactify

  ra=`grep ' ra ' $beam/obs.start | head -n 1 | awk '{printf("%f",180.0*$3/3.141592654)}'`
  dec=`grep dec $beam/obs.start | head -n 1 | awk '{printf("%f",180.0*$3/3.141592654)}'`

  glgb=`eq2gal.py $ra $dec`
  grid_id=`echo $glgb | awk '{printf("G%05.1f%+05.1f",$1,$2)}'`


  psrxml_modify_key $xml_file ra $ra "units='degrees'" start_coordinate
  psrxml_modify_key $xml_file dec $dec "units='degrees'" start_coordinate

  ra=`grep ' ra ' $beam/obs.start | tail -n 1 | awk '{printf("%f",180.0*$3/3.141592654)}'`
  dec=`grep dec $beam/obs.start | tail -n 1 | awk '{printf("%f",180.0*$3/3.141592654)}'`
  psrxml_modify_key $xml_file ra $ra "units='degrees'" end_coordinate
  psrxml_modify_key $xml_file dec $dec "units='degrees'" end_coordinate


  ra=`grep ' ra ' $ref_beam/obs.start | tail -n 1 | awk '{printf("%f",180.0*$3/3.141592654)}'`
  dec=`grep dec $ref_beam/obs.start | tail -n 1 | awk '{printf("%f",180.0*$3/3.141592654)}'`
  psrxml_modify_key $xml_file ra $ra "units='degrees'" requested_coordinate
  psrxml_modify_key $xml_file dec $dec "units='degrees'" requested_coordinate

  psrxml_modify_key $xml_file position_error 0.000001
  psrxml_modify_key $xml_file position_epoch J2000


  src_name_centre_beam=`grep "<source_name>" $centre_xml_file \
    | sed -e 's:<source_name>\(.*\)</source_name>:\1:'`
  psrxml_modify_key $xml_file source_name_centre_beam $src_name_centre_beam


  
  if test x$beam != x$ref_beam ; then
    psrxml_modify_key $xml_file source_name $grid_id
  fi

  psrxml_modify_key $xml_file pulsarhunter_code PARKES "" telescope
  psrxml_modify_key $xml_file sigproc_code 4 "" telescope
  psrxml_modify_key $xml_file tempo_code 7 "" telescope
  psrxml_modify_key $xml_file x "-4554231.5" "" telescope
  psrxml_modify_key $xml_file y "2816759.1" "" telescope
  psrxml_modify_key $xml_file z "-3454036.3" "" telescope
  psrxml_modify_key $xml_file zenith_limit "30.25" "units='degrees'" telescope
  psrxml_modify_key $xml_file lattitude "-32.999962" "units='degrees'" telescope
  psrxml_modify_key $xml_file longitude "148.263333" "units='degrees'" telescope

  psrxml_modify_key $xml_file name "MULTI" "" receiver
  psrxml_modify_key $xml_file feed_polarisation_basis "LINEAR" "" receiver
  psrxml_modify_key $xml_file feed_handedness "RIGHT" "" receiver
  psrxml_modify_key $xml_file cal_phase "0" "units='degrees'" receiver
  psrxml_modify_key $xml_file feed_symetry "-90" "units='degrees'" receiver

  psrxml_modify_key $xml_file name "BPSR" "" backend
  psrxml_modify_key $xml_file sideband "LOWER" "" backend
#  psrxml_modify_key $xml_file cross_phase "STANDARD" "" backend
  psrxml_modify_key $xml_file sigproc_code "10" "" backend














  rm tcs.$beam
done

