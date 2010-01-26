#! /bin/csh

set SFUSER = `awk -F@ '{print $1}' CVS/Root`
set SFPATH = ${SFUSER},psrchive@web.sourceforge.net:htdocs/classes

set SFHTML = "Util MEAL psrchive"

echo "Installing $SFHTML as $SFUSER"

cd html

rsync -avz $SFHTML $SFPATH/

