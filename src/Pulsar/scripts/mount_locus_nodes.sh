#!/bin/sh

# cross mount all the locus disks for the next 10 minutes
foreach i ( `seq 1 100` )
    echo $i | awk '{ printf("ls /cep1/locus%3.3d_data\n",$1)}' 
end
