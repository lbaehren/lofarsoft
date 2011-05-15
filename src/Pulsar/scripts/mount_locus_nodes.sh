#!/bin/ksh

# cross mount all the locus disks for the next 10 minutes

ii=1
echo "Opening the mount points for locus nodes 1->100; please wait."
while (( $ii <= 100 ))
do
     `echo $ii | awk '{ printf("ls /cep2/locus%3.3d_data\n",$1)}' >> /dev/null`
     ii=`expr $ii + 1`

done
