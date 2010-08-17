#!/usr/bin/env perl

###############################################################################
#

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;        
use warnings;
use Bpsr;
use Dada::client_master_control qw(%cfg);


#
# Global Variables
# 
%cfg = Bpsr::getConfig();

#
# Initialize module variables
#
$Dada::client_master_control::dl = 1;
$Dada::client_master_control::daemon_name = Dada::daemonBaseName($0);
$Dada::client_master_control::pwc_add = " -i ".$cfg{"PWC_IFACE"};


# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::client_master_control->main();

exit($result);

