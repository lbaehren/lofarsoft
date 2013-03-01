#!/bin/csh

set name=cvs_backup

if ( ! $?SOURCEFORGE_CVS_BACKUP ) then
  echo
  echo "${name}: the SOURCEFORGE_CVS_BACKUP environment variable must be set"
  echo
  exit
endif

if ( "$1" == "" || "$1" == "-h" || "$1" == "--help" ) then
  echo
  echo "${name}: back up the CVS repository of a sourceforge project"
  echo
  echo "usage:"
  echo "  ${name} PROJECT"
  echo
  echo "where:"
  echo "  PROJECT is a sourceforge project name"
  echo
  echo "e.g. add a line like the following using crontab:"
  echo
  echo "SOURCEFORGE_CVS_BACKUP = /pulsar/sourceforge
  echo "00 00 * * * $SOURCEFORGE_CVS_BACKUP/cvs_backup.csh PROJECT"
  echo
  exit
endif

set project=$1

cd $SOURCEFORGE_CVS_BACKUP

##############################################################################
#
# rsync the current state of the repository
#
##############################################################################

if ( ! -d $project/current ) mkdir -p $project/current

cd $project/current

echo "Running rsync ${project}.cvs.sourceforge.net ..."

rsync -av "rsync://${project}.cvs.sourceforge.net/cvsroot/${project}/*" .


##############################################################################
#
# create a compressed archive
#
##############################################################################

set file=`date +%Y_%m_%d_cvs_backup.tar.gz`

echo "Creating $file ..."

tar zcf $file *

cd ..

##############################################################################
#
# catalog appropriately
#
##############################################################################

if ( ! -d past ) mkdir past

if ( `date +%d` == "01" ) then

  set dir=past/monthly
  if ( ! -d $dir ) mkdir -p $dir
  cp current/$file $dir/$file

  # delete all monthly backups older than one year
  rm `find $dir -ctime +370`

endif

if ( `date +%a` == "Sun" ) then

  set dir=past/weekly
  if ( ! -d $dir ) mkdir -p $dir
  cp current/$file $dir/$file

  # delete all weekly backups older than one month
  rm `find $dir -ctime +30`

endif

set dir=past/daily
if ( ! -d $dir ) mkdir -p $dir
mv current/$file $dir/$file

# delete all daily backups older than one week
rm `find $dir -ctime +5`

