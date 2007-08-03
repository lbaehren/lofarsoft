#!/usr/bin/tcsh 

set script    = 'calc_start.g'
set start     = 1
set end       = 99999
set incr      = 200
set prefix    = 'lopesscript-out'
set suffix    = '.sos'
set input     = ''

foreach i ( $argv )
  switch ( $1 )
    case "-script":
      set last    = $1
      shift;breaksw
    case "-f":
      set last    = $1
      shift;breaksw
    case "-s":
      set last    = $1
      shift;breaksw
    case "-e":
      set last    = $1
      shift;breaksw
    case "-i":
      set last    = $1
      shift;breaksw
    case "-pref":
      set last    = $1
      shift;breaksw
    case "-lc":
      set last    = $1
      shift;breaksw
    case "[0-9A-Za-z/~.]*":
      if( "$last" == '-script' ) then
        set script = $1
        set last =
        shift;breaksw
      endif
      if( "$last" == '-f' ) then
        set input = $1
        set last =
        shift;breaksw
      endif
      if( "$last" == '-s' ) then
        set start = $1
        set last =
        shift;breaksw
      endif
      if( "$last" == '-e' ) then
        set end = $1
        set last =
        shift;breaksw
      endif
       if( "$last" == '-i' ) then
        set incr = $1
        set last =
        shift;breaksw
      endif
      if( "$last" == '-pref' ) then
        set prefix = $1
        set last =
        shift;breaksw
      endif
      if( "$last" == '-lc' ) then
        setenv LOPESCODEDIR $1
        set last =
        shift;breaksw
      endif
      default:
      echo "illegal argument $1"
  endsw
end


while ( $start <= $end  ) 
  @ stop = $start + $incr - 1
  aips++ $script  $input  $start $stop $prefix$start$suffix
  @ start = $start + $incr
end

echo "done!"



