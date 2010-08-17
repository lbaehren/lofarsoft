#!/usr/bin/env perl

use lib $ENV{"DADA_ROOT"}."/bin";

use strict;
use warnings;
use Apsr;
use Dada::server_auxiliary_manager qw(%cfg);
use File::Basename;

#
# Global Variable Declarations
#
%cfg = Apsr::getConfig();

# Ensure more than one copy of this daemon is not running
Dada::preventDuplicateDaemon(basename($0));


#
# Initialize module variables
#
$Dada::server_auxiliary_manager::dl = 1;
$Dada::server_auxiliary_manager::daemon_name = Dada::daemonBaseName($0);

# Autoflush STDOUT
$| = 1;

my $result = 0;
$result = Dada::server_auxiliary_manager->main();

exit($result);

