#!/usr/bin/env perl

###############################################################################
#

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;        
use warnings;
use Apsr;
use Dada::client_disk_cleaner qw(%cfg);


#
# Global Variables
# 
%cfg = Apsr::getConfig();

#
# Initialize module variables
#
$Dada::client_disk_cleaner::dl = 1;
$Dada::client_disk_cleaner::daemon_name = Dada::daemonBaseName($0);

# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::client_disk_cleaner->main();

exit($result);

