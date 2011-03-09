#!/usr/bin/perl -w
#
# Calculate the local sidereal time

# Vlad Kondratiev (c)
#
use Getopt::Long;

# Longitude
#$lambda = -79.8398333333; # GBT  79 deg 50' 23.40" W
#$lambda = 6.601882; # WSRT
$lambda = 6.869883; # LOFAR

$UTC = "";

# if there are no parameters in command line
# help is called and program normally exit
#&help($0) && exit if $#ARGV < 0;

# Parse command line
GetOptions ( "t=s" => \$UTC,   # - time
             "lon=f" => \$lambda, # - longitude (degrees)
             "h" => \$help);   # - to print the help

if ($help) { &help($0); exit 0; }

if ($UTC eq "") {
 $UTC = &get_curtime();
}

if ($lambda eq "") {
 &error ("Longitude is not defined!");
} 
$jd = `jd \"$UTC\" | grep JD | grep -v MJD | awk '{print \$2}' -`;
chomp $jd;

# get the sidereal time
$sid = &sidereal($jd, $lambda);

# converting LST to readable format
$h = sprintf ("%02d", int ($sid));
$m = sprintf ("%02d", int (($sid - $h) * 60.));
$s = ($sid - $h - $m/60.) * 3600.;
if ($s < 10.) { $s = "0" . $s; }
$LST = "$h:$m:$s";

# output
printf ("UTC: %s\n", &time2str($UTC));
printf ("LST: %s\n", &time2str($LST));

# help
sub help {
 local ($prg) = @_;
 $prg = `basename $prg`;
 chomp $prg;
 print "$prg: calculates LST for a given UTC time and given longitude\n";
 print "Usage: $prg [options]\n";
 print "        -t   TIME   - UTC time in format \"DD.MM.YYYY hh:mm:ss.sss\"\n";
 print "                      if no time is pointed than current UTC time will be used\n";
 print "        -lon LAMBDA - longitude in degrees (default - LOFAR)\n";
 print "                      Western longitudes are negative!\n";
 print "        -h          - print this help\n";
}


# calculates the sidereal time given the JD and longitude
sub sidereal {
 my ($jd, $lambda) = @_;
 $part = $jd - int ($jd);
 if ($part >= 0.5) { $jd0 = int($jd) + 0.5; $H = ($part - 0.5) * 24.; }
  else { $jd0 = int($jd) - 0.5; $H = ($part + 0.5) * 24.; }

 $D  = $jd  - 2451545.0;
 $D0 = $jd0 - 2451545.0; 
 $T = $D / 36525; 

 $gmst = 6.697374558 + 0.06570982441908 * $D0 + 1.00273790935 * $H + 0.000026 * $T * $T + $lambda/15.;
# $gmst = 18.697374558 + 24.06570982441908 * $D + $lambda/15.;

 return $gmst - (int($gmst/24.)) * 24.;
}

# get current UTC time
sub get_curtime {
 $sec = `date +%S`; chomp $sec;
 $min = `date +%M`; chomp $min;
 $hour = `date +%H`; chomp $hour;
 $day = `date +%d`; chomp $day;
 $month = `date +%m`; chomp $month;
 $year = `date +%Y`; chomp $year;
 $timezone = `date +%z`; chomp $timezone;
 $minadd = $timezone; $minadd =~ s/^[+-]?\d\d(\d\d)$/$1/;
 $houradd = $timezone; $houradd =~ s/^[+-]?(\d\d)\d\d$/$1/;
 if ($timezone =~ /^-/) {
  $min += $minadd;
  if ($min >= 60) { $min -= 60; $hour += 1; }
  $hour += $houradd;
  if ($hour >= 24) { $hour -= 24; $day += 1; }
 } else {
  $min -= $minadd;
  if ($min < 0) { $min += 60; $hour -= 1; }
  $hour -= $houradd;
  if ($hour < 0) { $hour += 24; $day -= 1; }
 }
 $min = sprintf ("%02d", $min);
 $hour = sprintf ("%02d", $hour);
 $day = sprintf ("%02d", $day);
 return "$day.$month.$year $hour:$min:$sec";
}

# gets time format string and leaves only 2 decimal digits of seconds
sub time2str {
 my ($time) = @_;
 $tmp = $time;
 $tmp =~ s/^(\d\d\:\d\d\:\d\d\.?\d?\d?).*$/$1/;
 return $tmp;
}
