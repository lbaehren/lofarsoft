#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                           Run a remote command
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from collections import defaultdict
from threading import BoundedSemaphore

import os
import signal
import subprocess
import threading

from lofarpipe.support.pipelinelogging import log_process_output
from lofarpipe.support.utilities import spawn_process
from lofarpipe.support.clusterlogger import clusterlogger

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

def run_remote_command(config, logger, host, command, env, arguments=None):
    """
    Run command on host, passing it arguments from the arguments list and
    exporting key/value pairs from env(a dictionary).

    Returns an object with poll() and communicate() methods, similar to
    subprocess.Popen.

    This is a generic interface to potentially multiple ways of running
    commands (SSH, mpirun, etc). The appropriate method is chosen from the
    config block supplied (with SSH as a fallback).
    """
    try:
        method = config.get('remote', 'method')
    except:
        method = None

    if method=="paramiko":
        try:
            key_filename = config.get('remote', 'key_filename')
        except:
            key_filename = None
        return run_via_paramiko(logger, host, command, env, arguments, key_filename)
    elif method=="mpirun":
        return run_via_mpirun(logger, host, command, env, arguments)
    else:
        return run_via_ssh(logger, host, command, env, arguments)

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

class ProcessLimiter(defaultdict):
    """
    Wrap a bounded semaphore, giving a convenient way to keep tabs on the
    number of simultaneous jobs running on a given host.
    """
    def __init__(self, nproc=None):
        if nproc:
            super(ProcessLimiter, self).__init__(
                lambda: BoundedSemaphore(int(nproc))
            )
        else:
            class Unlimited(object):
                """
                Dummy semaphore for the unlimited case.
                Acquire and release always succeed.
                """
                def acquire(self):
                    return True
                def release(self):
                    return True
            super(ProcessLimiter, self).__init__(Unlimited)

class ComputeJob(object):
    """
    Container for information about a job to be dispatched to a compute node.
    """
    def __init__(self, host, command, arguments=[]):
        self.host = host
        self.command = command
        self.arguments = arguments

    def dispatch(self, logger, config, limiter, loghost, logport, error):
        """
        Dispatch this job to the relevant compute node.

        Note that error is an instance of threading.Event, which will be set
        if the remote job fails for some reason.
        """
        limiter[self.host].acquire()
        try:
            process = run_remote_command(
                config,
                logger,
                self.host,
                self.command,
                {
                    "PYTHONPATH": config.get('deploy', 'engine_ppath'),
                    "LD_LIBRARY_PATH": config.get('deploy', 'engine_lpath')
                },
                arguments=[loghost, logport] + self.arguments
            )
            sout, serr = process.communicate()
            serr = serr.replace("Connection to %s closed.\r\n" % self.host, "")
            log_process_output("SSH session", sout, serr, logger)
        except Exception, e:
            logger.exception("Failed to run remote process %s (%s)" % (self.command, str(e)))
            error.set()
        finally:
            limiter[self.host].release()
        if process.returncode != 0:
            logger.error(
                "Remote process %s failed (status: %d)" % (self.command, process.returncode)
            )
            error.set()
        return process.returncode

class RemoteCommandRecipeMixIn(object):
    """
    Mix-in for recipes to dispatch jobs using the remote command mechanism.
    """
    def _schedule_jobs(self, jobs, max_per_node=None):
        """
        Schedule a series of compute jobs.

        job_args is an interable, containing a series of lists of arguments,
        each of which will be dispatched to the compute nodes.
        """
        threadpool = []
        limiter = ProcessLimiter(max_per_node)
        if max_per_node:
            self.logger.info("Limiting to %d simultaneous jobs/node" % max_per_node)
        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            for job in jobs:
                threadpool.append(
                    threading.Thread(
                        target=job.dispatch,
                        args=(
                            self.logger, self.config, limiter,
                            loghost, logport, self.error
                        )
                    )
                )
            [thread.start() for thread in threadpool]
            self.logger.info("Waiting for compute threads...")
            [thread.join() for thread in threadpool]
