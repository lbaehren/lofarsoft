#!/bin/csh

set i=0

while ( $i < 10000 )

  set b=1

  while ( $b <= 13 )

    ./ibob_bramdump 169.254.128.$b 23 > bramdump.txt
    set lines = `wc -l bramdump.txt | awk '{print $1}'`

    if ( $lines != 2048 ) then
      echo $lines not equal 2048
      exit -1
    endif

    @ b = $b + 1

  end

  @ i = $i + 1

end

