#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                           Run a remote command
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from collections import defaultdict
from threading import BoundedSemaphore

import os
import signal
import subprocess

from lofarpipe.support.pipelinelogging import log_process_output
from lofarpipe.support.utilities import spawn_process

class ProcessLimiter(defaultdict):
    """
    Wrap a bounded semaphore, giving a convenient way to keep tabs on the
    number of simultaneous jobs running on a given host.
    """
    def __init__(self, nproc):
        super(ProcessLimiter, self).__init__(
            lambda: BoundedSemaphore(int(nproc))
        )

class ParamikoWrapper(object):
    """
    Sends an SSH command to a host using paramiko, then emulates a Popen-like
    interface so that we can pass it back to pipeline recipes.
    """
    def __init__(self, paramiko_client, command):
        self.returncode = None
        self.client = paramiko_client
        self.chan = paramiko_client.get_transport().open_session()
        self.chan.get_pty()
        self.chan.exec_command(command)
        self.stdout = self.chan.makefile('rb', -1)
        self.stderr = self.chan.makefile_stderr('rb', -1)

    def communicate(self):
        if not self.returncode:
            self.returncode = self.chan.recv_exit_status()
        stdout = "\n".join(line.strip() for line in self.stdout.readlines()) + "\n"
        stderr = "\n".join(line.strip() for line in self.stdout.readlines()) + "\n"
        return stdout, stderr

    def poll(self):
        if not self.returncode and self.chan.exit_status_ready():
            self.returncode = self.chan.recv_exit_status()
        return self.returncode

    def wait(self):
        if not self.returncode:
            self.returncode = self.chan.recv_exit_status()
        return self.returncode

    def kill(self):
        self.chan.close()

def run_remote_command(
    logger, host, command, environment, arguments=None, method=None, key_filename=None
):
    """
    Run command on host, passing it arguments from the arguments list and
    exporting key/value pairs from environment (a dictionary).

    Returns an object with poll() and communicate() methods, similar to
    subprocess.Popen.

    This is a generic interface to potentially multiple ways of running
    commands (SSH, mpirun, etc).
    """

    if method=="paramiko":
        return run_via_paramiko(logger, host, command, environment, arguments, key_filename)
    elif method=="mpirun":
        return run_via_mpirun(logger, host, command, environment, arguments)
    else:
        return run_via_ssh(logger, host, command, environment, arguments)

def run_via_mpirun(logger, host, command, environment, arguments):
    """
    Dispatch a remote command via mpirun.

    Return a Popen object pointing at the MPI command, to which we add a kill
    method for shutting down the connection if required.
    """
    logger.debug("Dispatching command to %s with mpirun" % host)
    mpi_cmd = ["mpirun", "-host", host]
    for key in environment.keys():
        mpi_cmd.extend(["-x", key])
    mpi_cmd.append("--")
    mpi_cmd.extend(command.split()) # command is split into (python, script)
    mpi_cmd.extend(str(arg) for arg in arguments)
    env = os.environ
    env.update(environment)
    process = spawn_process(mpi_cmd, logger, env=env)
    process.kill = lambda : os.kill(process.pid, signal.SIGKILL)
    return process

def run_via_ssh(logger, host, command, environment, arguments):
    """
    Dispatch a remote command via SSH.

    We return a Popen object pointing at the SSH session, to which we add a
    kill method for shutting down the connection if required.
    """
    logger.debug("Dispatching command to %s with ssh" % host)
    ssh_cmd = ["ssh", "-n", "-tt", "-x", host, "--", "/bin/sh", "-c"]

    commandstring = ["%s=%s" % (key, value) for key, value in environment.items()]
    commandstring.append(command)
    commandstring.extend(str(arg) for arg in arguments)
    ssh_cmd.append('"' + " ".join(commandstring) + '"')
    process = spawn_process(ssh_cmd, logger)
    process.kill = lambda : os.kill(process.pid, signal.SIGKILL)
    return process

def run_via_paramiko(logger, host, command, environment, arguments, key_filename):
    """
    Dispatch a remote command via paramiko.

    We return an instance of ParamikoWrapper.
    """
    logger.debug("Dispatching command to %s with paramiko" % host)
    import paramiko
    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    client.connect(host, key_filename=key_filename)
    commandstring = ["%s=%s" % (key, value) for key, value in environment.items()]
    commandstring.append(command)
    commandstring.extend(str(arg) for arg in arguments)
    return ParamikoWrapper(client, " ".join(commandstring))

class RemoteCommandRecipeMixIn(object):
    """
    Mix-in for recipes to dispatch jobs using the remote command mechanism.
    """
    def _dispatch_compute_job(
        self, host, command, semaphore, *arguments
    ):
        """
        Dispatch a command to be run on the given host.
        Set the recipe's error Event if it does not return 0.
        """
        semaphore.acquire()
        try:
            process = self.run_remote_command(
                self.logger,
                host,
                command,
                {
                    "PYTHONPATH": self.config.get('deploy', 'engine_ppath'),
                    "LD_LIBRARY_PATH": self.config.get('deploy', 'engine_lpath')
                },
                arguments=arguments
            )
            sout, serr = process.communicate()
            serr = serr.replace("Connection to %s closed.\r\n" % host, "")
            log_process_output("SSH session", sout, serr, self.logger)
        except Exception, e:
            self.logger.exception("Failed to run remote process %s (%s)" % (command, str(e)))
            self.error.set()
            return 1
        finally:
            semaphore.release()
        if process.returncode != 0:
            self.logger.error(
                "Remote process %s failed (status: %d)" % (command, process.returncode)
            )
            self.error.set()
        return process.returncode
