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
    ssh_cmd.extend(str(arg) for arg in arguments)
    return subprocess.Popen(
        ssh_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )

class RemoteCommandRecipeMixIn(object):
    """
    Mix-in for recipes to dispatch jobs using the remote command mechanism.
    """
    def _dispatch_compute_job(
        self, host, command, semaphore, loghost, logport, *arguments
    ):
        """
        Dispatch a command to be run on the given host.
        Set the recipe's error Event if it does not return 0.
        """
        semaphore.acquire()
        try:
            process = run_remote_command(
                host,
                command,
                {
                    "PYTHONPATH": self.config.get('deploy', 'engine_ppath'),
                    "LD_LIBRARY_PATH": self.config.get('deploy', 'engine_lpath')
                },
                loghost,
                str(logport),
                *arguments
            )
            sout, serr = process.communicate()
        finally:
            semaphore.release()
        if process.returncode != 0:
            self.error.set()
        return process.returncode
