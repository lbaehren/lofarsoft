#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                           Run a remote command
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from collections import defaultdict
from threading import BoundedSemaphore

import subprocess


class ProcessLimiter(defaultdict):
    def __init__(self, nproc):
        super(ProcessLimiter, self).__init__(
            lambda: BoundedSemaphore(int(nproc))
        )

def run_remote_command(host, command, environment, *arguments):
    """
    Run command on host, passing it arguments from the arguments list and
    exporting key/value pairs from environment (a dictionary).

    Returns an instance of subprocess.Popen connected to the remove command.

    This is a generic interface to potentially multiple ways of running
    commands (SSH, mpirun, etc).
    """
    return run_via_ssh(host, command, environment, *arguments)

def run_via_ssh(host, command, environment, *arguments):
    """
    Run a remote command via SSH.
    """
    ssh_cmd = ["ssh", "-n", "-t", "-x", host, "--"]

    for key, value in environment.iteritems():
        ssh_cmd.append("%s=%s" % (key, value))

    ssh_cmd.append(command)
    ssh_cmd.extend(arguments)
    return subprocess.Popen(ssh_cmd)
