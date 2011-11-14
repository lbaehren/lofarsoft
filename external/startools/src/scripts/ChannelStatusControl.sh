#!/bin/bash
# Thomas Asch
# 15.08.2007

# root
export ROOTSYS=$HOME/prog/root/
export PATH=$ROOTSYS/bin:$PATH
export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
export CPLUS_INCLUDE_PATH=$ROOTSYS/include:$CPLUS_INCLUDE_PATH

# system
export PATH=$HOME/prog/bin:$PATH

# STAR libRecEvent 
export STAR_DIR=$HOME/src/star-tools_prototype
export LD_LIBRARY_PATH=$STAR_DIR/lib:$LD_LIBRARY_PATH


DATABASE="radac_d$1"
#echo $DATABASE


echo "Channel Status Control of LOPES-STAR"
echo "------------------------------------"
echo $DATABASE
echo

# check for active run
ACTIVE_RUNS=`psql -c "SELECT COUNT(id) FROM runs WHERE is_active = true;" $DATABASE -h ipelopes1.ka.fzk.de -U web-radac |sed -n 3p`
test $ACTIVE_RUNS -ne 0
#echo $ACTIVE_RUNS

RUN_ID=0
RUN_ID=`psql -c "SELECT id FROM runs ORDER by id DESC LIMIT 1;" $DATABASE -h ipelopes1.ka.fzk.de -U web-radac | sed -n 3p` 
test $RUN_ID -ne 0
#echo $RUN_ID

NO_EVENTS=0
NO_EVENTS=`psql -c "SELECT count(id) FROM events WHERE run_id=$RUN_ID;" $DATABASE -h ipelopes1.ka.fzk.de -U web-radac | sed -n 3p`
#echo $NO_EVENTS

OUTPUT_FILE="/tmp/CSC_$DATABASE"

echo "run id  = " $RUN_ID
echo "active  = " $ACTIVE_RUNS
echo "#events = " $NO_EVENTS
echo 
if [ $NO_EVENTS -lt 1 ]
then
  echo "No entries found to analyse!"
else
  echo "starting readout"
  db2root -f $OUTPUT_FILE -c 0 -d $1 -r $RUN_ID > /dev/null
  echo "finished"
  echo

  INPUT="$OUTPUT_FILE""_00.root"

  echo "starting channel analysis"
  ChannelStatusControl -i $INPUT
  echo
  echo "finished"
  echo

  OUTPUT_FILE="$OUTPUT_FILE*"
  echo "deleting $OUTPUT_FILE"
  rm -rf $OUTPUT_FILE
  echo 
fi  
  
echo "done - TA"
exit
