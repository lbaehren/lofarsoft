#!/usr/bin/env perl
# Author:   Andrew Jameson
# Created:  21 May 2008
# Modified: 21 May 2008
#
                                                                                                                   
#
# Include Modules
#
                                                                                                                   
use lib $ENV{"DADA_ROOT"}."/bin";

use strict;         # strict mode (like -Wall)
use IO::Socket::INET;
use IO::Socket;
use Socket qw(:all); 
use IO::Select;
use Time::HiRes qw(usleep ualarm gettimeofday tv_interval);
use File::Basename;
use Math::BigInt;
use Math::BigFloat;
use Dada;             # DADA Module for configuration options
use Bpsr;             # BPSR Module for configuration options

#
# Constants
#
use constant DEBUG_LEVEL     => 2;
use constant IBOB_IP         => "169.254.128.15";   # "169.254.128.2";
use constant IBOB_PORT       => "23";    # "23";
use constant IBOB_TERMINATOR => "\r";

use constant MAX_VALUE        => 1;
use constant AVG_VALUES       => 2;


#
# Global Variable Declarations
#
our $handle = 0;


#
# Local variable decalatrions
#
my @output = ();



#
# Register Signal handlers
#
$SIG{INT} = \&sigHandle;
$SIG{TERM} = \&sigHandle;
$SIG{PIPE} = \&sigPipeHandle;


#
# Main
#
logMessage(2, "Tring to connect to ".IBOB_IP.":".IBOB_PORT);
$handle = Dada::connectToMachine(IBOB_IP, IBOB_PORT);

if (!$handle) {

  logMessage(0, "Could not connect to ".IBOB_IP.":".IBOB_PORT);
  exit(1);

} else {

  logMessage(2, "Connected to ".IBOB_IP.":".IBOB_PORT);

  my $command = "";

  while ($command ne "quit") {

    print "Enter command: ";

    $command = <STDIN>;

    my $cleaned_command = $command;

    $cleaned_command =~ s/\r//;
    $cleaned_command =~ s/\n//;

    $command = $cleaned_command;

    print "STDIN: $command\n";
    @output = ();

    if ($command eq "config") {

      print $handle "regwrite reg_ip 0x0a000004".IBOB_TERMINATOR;
      
      print $handle "regwrite reg_10GbE_destport0 4001".IBOB_TERMINATOR;
      
      print $handle "write l xd0000000 xffffffff".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "setb x40000000".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "writeb l 0 x00000060".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);
      print $handle "writeb l 4 xdd47e301".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "writeb l 8 x00000000".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "writeb l 12 x0a000001".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "writeb b x16 x0f".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "writeb b x17 xa0".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "writeb l x3020 x00000060".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "writeb l x3024 xdd4724fb".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "writeb b x15 xff".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print $handle "write l xd0000000 x0".IBOB_TERMINATOR;
      @output = getResponse($handle, 0.1);
      printResponse(\@output);

      print STDERR "CONFIG DONE\n";

    } elsif ($command =~ m/^acclen (\d)+$/) {

      my ($cmd, $acclen) = split(" ",$command);
      setAccLen($handle, $acclen);

    } elsif ($command eq "acclen") {

      print $handle "regwrite reg_acclen 24".IBOB_TERMINATOR;
      print $handle "regwrite reg_sync_period 2560000".IBOB_TERMINATOR;

    } elsif ($command eq "rearm") {

      my $curr_time = time();
      my $prev_time = $curr_time;

      # A busy loop to continually get the time until
      # we are on a 1 second boundary;
      while ($curr_time < ($prev_time + 1)) {
        $curr_time = time();
      }
      my @t1 = gettimeofday();
      # Sleep for half a second
      Time::HiRes::usleep(400000);
      my @t2 = gettimeofday();

      # Issue to re-arm command
      print $handle "regwrite reg_arm 0".IBOB_TERMINATOR;
      logMessage(0, "regwrite reg_arm 0");
      print $handle "regwrite reg_arm 1".IBOB_TERMINATOR;
      logMessage(0, "regwrite reg_arm 1");
      @output = getResponse($handle, 0.1);
      #printResponse(\@output);

    } elsif ($command eq "getpol0") {

      logMessage(0, "bramdump scope_output1/bram");
      print $handle "bramdump scope_output1/bram".IBOB_TERMINATOR;

      @output = getResponse($handle, 0.5);
      printBramdump(\@output);

    } elsif ($command eq "getpol1") {
                          
      logMessage(0, "bramdump scope_output3/bram");
      print $handle "bramdump scope_output3/bram".IBOB_TERMINATOR;
   
      @output = getResponse($handle, 0.5);
      printBramdump(\@output);
    
    } elsif ($command eq "help") {

      print $handle "?";

      @output = getResponse($handle, 0.1);
      printResponse(\@output);

    } elsif ($command =~ m/^setscale (\d)+$/) {

      my @arr = split(/ /,$command);
      my $scale = $arr[1];
      logMessage(0, "Setting scaling factor to ".$scale);

      # Set the scaling coeffs for each pol.
      print $handle "regwrite reg_coeff_pol1 ".$scale.IBOB_TERMINATOR;
      print $handle "regwrite reg_coeff_pol2 ".$scale.IBOB_TERMINATOR;


    } elsif ($command eq "levelset_max") {

      # Turn the acc_len up to a nice high number
      # setAccLen($handle, 255);
      # setAccLen($handle, 25);

      # Get the MAX of all the frequency channels
      my $scale1 = 4096;    # pol 1
      my $scale2 = 4096;    # pol 2
     
      my $new_scale1;
      my $new_scale2;
      my $bit_select;

      my $j=0;

      for ($j=0; $j < 2; $j++ ) {

        #logMessage(2, "sleeping for 2 secs");
        #sleep(2);
        #logMessage(2, "sleeping over");
 
        ($new_scale1, $new_scale2, $bit_select) = getNewScaleCoeff($handle,$scale1, $scale2 ,MAX_VALUE);
        logMessage(1, "pol0: ".$scale1." => ".$new_scale1);
        logMessage(1, "pol1: ".$scale2." => ".$new_scale2);
        $scale1 = $new_scale1;
        $scale2 = $new_scale2;
        sleep(1);
      }

      #logMessage(2, "sleeping for 2 secs");
      #sleep(2);
      #logMessage(2, "sleeping over");

      logMessage(1, "Re-testing new scale factors");
      ($new_scale1, $new_scale2, $bit_select) = getNewScaleCoeff($handle,$scale1, $scale2 ,MAX_VALUE);
      logMessage(1, "pol0: ".$scale1." => ".$new_scale1);
      logMessage(1, "pol1: ".$scale2." => ".$new_scale2);

      # Now set the AccLen to the survey standard
      #setAccLen($handle, 24);

      #logMessage(1, "acc_len changed from 255 to 24");

      # And adjust the scale accordingly
      #my $factor1 = Math::BigFloat->new(255.0/24.0);
      #my $factor2 = Math::BigFloat->new(255.0/24.0);
      #$factor1->bsqrt();
      #$factor2->bsqrt();
      #$factor1 *= $new_scale1;
      #$factor2 *= $new_scale2;
      #$scale1 = $factor1->bfloor();
      #$scale2 = $factor2->bfloor();
    
      #logMessage(1, "Adjusted scale1 factor: ".$scale1);
      #logMessage(1, "Adjusted scale2 factor: ".$scale2);
      #logMessage(2, "sleeping for 2 secs");
      #sleep(2);
      #logMessage(2, "sleeping over");
      
      #($new_scale1, $new_scale2, $bit_select) = getNewScaleCoeff($handle,$scale1, $scale2 ,MAX_VALUE);
      #logMessage(1, "pol0: ".$scale1." => ".$new_scale1);
      #logMessage(1, "pol1: ".$scale2." => ".$new_scale2);

      # Now select the bits we want
      print $handle "regwrite reg_output_bitselect ".$bit_select.IBOB_TERMINATOR;

      print "LEVELS SET!\n"


    } elsif ($command eq "levelset_mean") {

      my $scale = 4096;
      my $new_scale;
      my $bit_select;

      setAccLen($handle, 255);

      ($new_scale, $bit_select) = getNewScaleCoeff($handle,$scale,AVG_VALUES);
      logMessage(1, $scale." => ".$new_scale);
                                                                                                  
    } elsif ($command eq "quit") {

      # Quit script

    } else {

    }

  }

  close($handle);

  logMessage(2, "Connection to ".IBOB_IP.":".IBOB_PORT." closed");

}


#
# Logs a message to the Nexus
#
sub logMessage($$) {
  (my $level, my $message) = @_;
  if ($level <= DEBUG_LEVEL) {
    my @t2 = gettimeofday();
    my $time = Dada::printDadaTime($t2[0]);
    my $subsec = substr(sprintf("%.3f",$t2[1]/1000000.0),2);
    print "[".$time.".".$subsec."] ".$message."\n";
  }
}


sub getResponse($$) {

  my ($handle, $timeout) = @_;

  my $done = 0;
  my $read_set = new IO::Select($handle);
  my @output = ();
  my $rh = 0;
  my $line = 0;
  my $i=0;

  my $old_buffer = "";

  while (!$done) {

    my ($readable_handles) = IO::Select->select($read_set, undef, undef, $timeout);
    $done = 1;

    my $buffer = "";
    my $to_process = "";

    foreach $rh (@$readable_handles) {

      $buffer = "";
      $to_process = "";

      # read 1024 bytes at a time
      recv($handle, $buffer, 1023, 0);

      $buffer =~ s/\r//g;

      # If we had unprocessed text from before
      if (length($old_buffer) > 0) {
        $to_process = $old_buffer.$buffer;
        $old_buffer = "";
      } else {
        $to_process = $buffer;
      }
     
      # Check if we have a complete line in the data to process
      if ($to_process =~ m/\n/) {

         my @arr = split(/\n/, $to_process);

         # If there is only a newline...
         if ($#arr < 0) {
           push(@output, $arr[0]);

         } else {
         
           for ($i=0; $i<$#arr; $i++) {
             push (@output, $arr[$i]);
           }

           # If the final character happened to be a newline,
           # we should output the final line, otherwise, add
           # it to the old_buffer
           if ($to_process =~ m/(.)*\n$/) {
             push (@output, $arr[$#arr]);
           } else {
             $old_buffer = $arr[$#arr];
           }
         }

      } else {
        $old_buffer .= $to_process;
      }

      $done = 0;

    }
  }
  if (length($old_buffer) >= 0) {
    push(@output,$old_buffer);
  }

  #for ($i=0;$i<=$#output;$i++) {
  #  print "[$i] ".$output[$i]."\n";
  #}
  
  return @output;

}


sub sigHandle($) {

  my $sigName = shift;
  print STDERR basename($0)." : Received SIG".$sigName."\n";

  print STDERR "Closing connection\n";
  close($handle);

  print STDERR basename($0)." : Exiting\n";

  exit 1;

}

sub printResponse(\@) {

  (my $outputRef) = @_;

  my @output = @$outputRef;

  my $i=0;
  print "response:\n";
  for ($i=0;$i<=$#output; $i++) {
    print "output[".$i."] = ".$output[$i]."\n";
  }

}

sub printBramdump(\@) {
                                                                                                  
  (my $outputRef) = @_;
                                                                                                  
  my @output = @$outputRef;
                                                                                                  
  my $i=0;
  my @arr = ();

  for ($i=0;$i<=512; $i++) {
    # print "output[".$i."] = ".$output[$i]."\n";
    if (length($output[$i]) > 0) {
      @arr = split(/ \/ | \-> /,$output[$i]);
      print $arr[1].": ".$arr[4].", ".$arr[3]."\n";
    }
  }
}

sub getGains(\@) {
                                                                                                  
  (my $outputRef) = @_;
                                                                                                  
  my @output = @$outputRef;
                                                                                                  
  my $i=0;
  my @arr = ();
  my @gains = ();
                                                                                                  
  for ($i=0;$i<=512; $i++) {
    # print "output[".$i."] = ".$output[$i]."\n";
    if (length($output[$i]) > 0) {
      @arr = split(/ \/ | \-> /,$output[$i]);
      push(@gains,$arr[4]);
    }
  }
  return @gains;
}

sub getSingleValue($\@) {
  
  my ($type, $valsRef) = @_;

  my @vals = @$valsRef;

  my $value = Math::BigInt->new(0);
  my $nvals = 0;
  my $i=0;

  # Just return the maximum value
  if ($type = MAX_VALUE) {

    for ($i=0; $i<=$#vals; $i++) {
      if ($vals[$i] > $value) {
        $value = $vals[$i];
      }
    }

  # Get the average value
  } elsif ($type == AVG_VALUES) {

    for ($i=0; $i<=$#vals; $i++) {
      $value += $vals[$i];
      $nvals++;
    }
    $value->bdiv($nvals);

  } else {

    logMessage(0, "UNKNOWN type requested");
    exit(1);

  }

  return int($value);
}

sub getNewScaleCoeff($$$$) {
  
  my ($handle, $scale1, $scale2, $type) = @_;

  logMessage(2, "getNewScaleCoeff(".$scale1.", ".$scale2.")");

  my @bitsel_min = qw(0 256 65536 16777216);
  my @bitsel_mid = qw(64 8192 2097152 536870912);
  my @bitsel_max = qw(255 65535 16777215 4294967295);
  my @output = ();

  # Set the scaling coeffs for each pol.
  logMessage(1, "regwrite reg_coeff_pol1 ".$scale1);
  print $handle "regwrite reg_coeff_pol1 ".$scale1.IBOB_TERMINATOR;
  logMessage(1, "regwrite reg_coeff_pol2 ".$scale2);
  print $handle "regwrite reg_coeff_pol2 ".$scale2.IBOB_TERMINATOR;

  sleep(2);

  # Get values for pol0
  print $handle "bramdump scope_output1/bram".IBOB_TERMINATOR;
  logMessage(1, "bramdump scope_output1/bram");
  @output = getResponse($handle, 0.5);
  my @pol1 = getGains(@output);
  my $pol1_val = getSingleValue($type, @pol1);

  # Get values for pol1
  @output = ();
  print $handle "bramdump scope_output3/bram".IBOB_TERMINATOR;
  logMessage(1, "bramdump scope_output3/bram");
  @output = getResponse($handle, 0.5);
  my @pol2 = getGains(@output);
  my $pol2_val = getSingleValue($type, @pol2);

  # Work out how to change the scaling factor to make the polarisations
  # roughly the same
  my $val;

  if ($type == MAX_VALUE) { # choose the lowest pol as the representative value (more bit selection downwardS)
    $val = Math::BigFloat->new( ($pol1_val > $pol2_val) ? $pol2_val : $pol1_val);
    logMessage(1, "Current Max vals : [".$pol1_val.", ".$pol2_val."] => ".$val);
  } elsif ($type == AVG_VALUES) {
    $val = Math::BigFloat->new( ($pol1_val + $pol2_val) / 2 );
    logMessage(1, "Current Avg vals : [".$pol1_val.", ".$pol2_val."] => ".$val);
  } else {
    logMessage(0, "Shouldn't get to here");
  }

  # Find which bit selection window we are currently sitting in
  my $i=0;
  my $current_window = 0;
  my $desired_window = 0;
  for ($i=0; $i<4; $i++) {
                                                                                                  
    # If average (max) value is in the lower half
    if (($val> $bitsel_min[$i]) && ($val <= $bitsel_mid[$i])) {
      $current_window = $i;
                                                                                                  
      if ($i == 0) {
        $desired_window = 0;
                                                                                                  
      } else {
        $desired_window = ($i-1);
      }
    }
                                                                                                  
    # If average (max)n value is in the upper half, simply raise to
    # the top of this window
    if (($val > $bitsel_mid[$i]) && ($val <= $bitsel_max[$i])) {
      $current_window = $i;
      $desired_window = $i;
    }
  }

  if ($desired_window == 3) {
    $desired_window = 2;
  }
   
  if (($pol1_val eq 0) || ($pol2_val eq 0)) {
    $pol1_val = 1;
    $pol2_val = 1;
  }
  
  my $max_val =  Math::BigFloat->new(($bitsel_max[$desired_window]+1) / 2);
  logMessage(2, "max_val  = ".$max_val);

  my $gain_factor1 = Math::BigFloat->new($max_val / $pol1_val);
  my $gain_factor2 = Math::BigFloat->new($max_val / $pol2_val);
  logMessage(2, "gain_factor0 = ".$gain_factor1);
  logMessage(2, "gain_factor1 = ".$gain_factor2);

  $gain_factor1->bsqrt();
  $gain_factor2->bsqrt();
  logMessage(2, "gain_factor0->sqrt() = ".$gain_factor1);
  logMessage(2, "gain_factor1->sqrt() = ".$gain_factor2);

  $gain_factor1 *= $scale1;
  $gain_factor2 *= $scale2;
  
  logMessage(2, "gain_factor0*scale0 = ".$gain_factor1);
  logMessage(2, "gain_factor1*scale1 = ".$gain_factor2);

  $gain_factor1->bfloor();
  $gain_factor2->bfloor();

  logMessage(1, "getNewScaleCoeff => (".$gain_factor1.", ".$gain_factor2.", ".$desired_window.")");

  return ($gain_factor1, $gain_factor2, $desired_window);
  
}


#
# Sets reg_acclen to (acc_len-1)
#
sub setAccLen($$) {

  my ($sock, $acc_len) = @_;

  my $n = 100;
  my $k = $acc_len * 512;
  my $x = 1024;
  my $result = $n*lcm($k,$x);

  logMessage(2, "setAccLen(".$acc_len.") => ".$result);
  logMessage(2, "regwrite reg_acclen ".($acc_len-1));
  print $handle "regwrite reg_acclen ".($acc_len-1).IBOB_TERMINATOR;
  logMessage(2, "regwrite reg_sync_period ".$result);
  print $handle "regwrite reg_sync_period ".$result.IBOB_TERMINATOR;

  return $result;
}

#
# Calculate the Greatest Common Denominator (GCD)
#
sub gcd {
  my ($a,$b)=@_;
  while ($a!=$b) {
    if ($a>$b) {
      $a-=$b;
    } else {
      $b-=$a;
    }
  }
  return $a;
}

#
# Calculate the Least Common Multiple (LCM)
#
sub lcm {
    my ($a,$b)=@_;
    return $a*$b/&gcd($a,$b);
}
    

