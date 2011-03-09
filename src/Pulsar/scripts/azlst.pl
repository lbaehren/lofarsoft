#!/usr/bin/perl 
#
# script calculates the azimuth and hour angle (and LST)
# (actually the min and max values of HA and LST)
# of the source with pointed RA and DEC for pointed elevation (zenith angle)
#
use Math::Trig;
use Getopt::Long;

$pi = 3.14159265359;
$rad = $pi/180.;

# Latitude
#$phi = 38.4331194444; # GBT  38 deg 25' 59.23" N
#$phi = 52.915604; # WSRT
$phi = 52.915111; # LOFAR
# Longitude
#$lambda = -79.8398333333; # GBT  79 deg 50' 23.40" W
#$lambda = 6.601882; # WSRT
$lambda = 6.869883; # LOFAR


$ra = "";
$dec = "";
$EL = 30.;

# if there are no parameters in command line
# help is called and program normally exit
&help($0) && exit if $#ARGV < 0;

# Parse command line
GetOptions ( "el=f" => \$EL,   # - elevation (in deg)
             "ra=s" => \$ra,   # - RA of the source "hh:mm:ss.sss"
             "dec=s" => \$dec, # - DEC of the source "[+|-]dd:mm:ss.ssss"
             "lat=f" => \$phi, # - latitude (degrees)
             "lon=f" => \$lambda, # - longitude (degrees)
             "h" => \$help);   # - to print the help

if ($help) { &help($0); exit 0; }

if ($ra eq "" || $dec eq "") {
 &error ("No RA or DEC commanded!");
} 

if ($phi eq "") {
 &error ("Latitude is not defined!");
} 

if ($lambda eq "") {
 &error ("Longitude is not defined!");
} 

if ($EL eq "") {
 &error ("Elevation is not defined!");
}

$dec1 = &dec2rad ($dec);

# zenith angle
$ZA = 90. - $EL;
# hour angle (absolute value) in rad
# actually there are 2 values +- $HA
$HA = &get_HA ($ZA, $dec1, $phi);
$HAmin = -$HA;
$HAmax = $HA;
# in degrees (from South clockwise)
$AZmin = &get_AZ ($dec1, $phi, $HAmin);
$AZmax = &get_AZ ($dec1, $phi, $HAmax);


$ZA = sprintf ("%.1f", $ZA);
$EL = sprintf ("%.1f", $EL);

$alphah = &time2hour($ra);
$sidmin = `echo \"scale=20\n$alphah - ($HA / $rad / 15.)\" | bc -l`;
$sidmax = `echo \"scale=20\n$alphah + ($HA / $rad / 15.)\" | bc -l`;
if ($sidmin < 0.) { $sidmin += 24.; }
if ($sidmax >= 24.) { $sidmax -= 24.; }
$LSTmin = &hour2time ($sidmin);
$LSTmax = &hour2time ($sidmax);


print "\n";
print "Source: RA = $ra  DEC = $dec\n";
print "Site: LAT = $phi deg  LON = $lambda deg\n";
print "\n";
print "EL = $EL deg (ZA = $ZA deg)\n";
printf ("AZ = [ %.1f ; %.1f ] deg\n", $AZmin, $AZmax);
$hapres = $HA / $rad / 15.; 
$hapres_label = "h";
if (abs($hapres) < 1) { 
 $hapres *= 60.; 
 $hapres_label = "min"; 
 if (abs($hapres) < 1) {
  $hapres *= 60.; 
  $hapres_label = "sec"; 
 }
}
printf ("HA =  +/- %.2f %s  [Duration = %.2f %s]\n", $hapres, $hapres_label, 2 * $hapres, $hapres_label);
printf ("LST = [rise: %s] [set: %s]\n", $LSTmin, $LSTmax);
print "\n";



# help
sub help {
 local ($prg) = @_;
 $prg = `basename $prg`;
 chomp $prg;
 print "$prg - calculates AZ and LST of rise and set for a given source at given observatory\n";
 print "Usage: $prg [options]\n";
 print "        -el  EL     - elevation in degrees, default = 30 deg\n";
 print "        -ra  RA     - right assention of the source where RA is in \"hh:mm:ss.sss\"\n";
 print "        -dec DEC    - declination of the source where DEC is in \"[+\-]dd:mm:ss.sss\"\n";
 print "        -lat PHI    - latitude in degrees (default - LOFAR)\n";
 print "        -lon LAMBDA - longitude in degrees (default - LOFAR)\n";
 print "                      Western longitudes are negative!\n";
 print "        -h          - print this help\n";
}

# unique subroutine to output errors
# one input parameter (string): the error message
sub error {
 my ($error) = @_;
 print "Error: $error\n";
 exit 1;
}

# transform dec format into radians
sub dec2rad {
 my ($d) = @_;
 $deg = $d; $deg =~ s/^([+-]?\d\d)\:.*$/$1/;
 $deg *= 1;
 $mm = $d; $mm =~ s/^[+-]?\d\d\:(\d\d)\:.*$/$1/;
 $ss = $d; $ss =~ s/^[+-]?\d\d\:\d\d\:(\d\d.*)$/$1/;
 if ($deg < 0) {
  $newd = `echo \"($deg - $mm / 60. - $ss / 3600.) * $rad\" | bc -l`;
 } else {
  $newd = `echo \"($deg + $mm / 60. + $ss / 3600.) * $rad\" | bc -l`;
 }
 chomp $newd;
 return $newd;
}

# get hour angle (absolute value) in radians
# actually there are 2 values +- 
sub get_HA {
 my ($zen, $delta, $shirota) = @_;

 $ha = acos ( (cos ($zen * $rad) - sin ($delta) * sin ($shirota * $rad)) / (cos ($delta) * cos ($shirota * $rad)) );
 return $ha;
}

# get azimuth (in degrees, from South clockwise)
sub get_AZ {
 my ($delta, $shirota, $hour_angle) = @_;

 $azim = atan2 ( cos ($delta) * sin ($hour_angle), -sin ($delta) * cos ($shirota * $rad) + cos ($delta) * sin ($shirota * $rad) * cos ($hour_angle) );
 $azim /= $rad;
 $azim += 180.;
 while ($azim >= 360.) { $azim -= 360.; }
 return $azim; 
}

# transform time format into hours
sub time2hour {
 my ($time) = @_;
 $hh = $time; $hh =~ s/^(\d\d)\:.*$/$1/;
 $mm = $time; $mm =~ s/^\d\d\:(\d\d)\:.*$/$1/;
 $ss = $time; $ss =~ s/^\d\d\:\d\d\:(\d\d.*)$/$1/;
 $t = `echo \"scale=20\n($hh + $mm / 60. + $ss / 3600.)\" | bc -l`;
 chomp $t;
 return $t;
}

# transform hours into time format
sub hour2time {
 my ($hours) = @_;
 $hh = int ($hours);
 $mm = int ( 60. * ($hours - $hh) );
 $ss = (($hours - $hh) * 60. - $mm) * 60.;
 $time = sprintf ("%02d:%02d:%02d", $hh, $mm, $ss);
 return $time;
}
