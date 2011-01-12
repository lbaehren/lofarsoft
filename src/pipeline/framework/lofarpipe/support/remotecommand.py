#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                           Run a remote command
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from collections import defaultdict
from threading import BoundedSemaphore

import re
import os
import signal
import threading
import time

from lofarpipe.support.pipelinelogging import log_process_output
from lofarpipe.support.utilities import spawn_process
from lofarpipe.support.jobserver import job_server

# By default, Linux allocates lots more memory than we need(?) for a new stack
# frame. When multiplexing lots of threads, that will cause memory issues.
threading.stack_size(1048576)

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
    mpi_cmd = ["/usr/bin/mpirun", "-host", host]
    for key in environment.keys():
        mpi_cmd.extend(["-x", key])
    mpi_cmd.append("--")
    mpi_cmd.extend(command.split()) # command is split into (python, script)
    mpi_cmd.extend(str(arg) for arg in arguments)
    env = os.environ
    env.update(environment)
    process = spawn_process(mpi_cmd, logger, env=env)
    # mpirun should be killed with a SIGTERM to enable it to shut down the
    # remote command.
    process.kill = lambda : os.kill(process.pid, signal.SIGTERM)
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
    commandstring.extend(re.escape(str(arg)) for arg in arguments)
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
    commandstring.extend(re.escape(str(arg)) for arg in arguments)
    return ParamikoWrapper(client, " ".join(commandstring))

class ProcessLimiter(defaultdict):
    """
    Provide a dictionary-like structure of bounded semaphores with arbitrary
    keys.

    This gives a convenient way to keep tabs on the number of simultaneous
    jobs running on a given host.

    :param nproc: Bound value for semaphore (ie, maximum number of jobs)
    :type nproc: integer or none
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

    :param host: Target host for job
    :param command: Full path to command to be run on target host
    :param arguments: List of arguments which will be passed to command
    """
    def __init__(self, host, command, arguments=[]):
        self.host = host
        self.command = command
        self.arguments = arguments
        self.results = {}

    def dispatch(self, logger, config, limiter, id, jobhost, jobport, error, killswitch):

        """
        Dispatch this job to the relevant compute node.

        Note that error is an instance of threading.Event, which will be set
        if the remote job fails for some reason.
        """
        self.id = id
        limiter[self.host].acquire()
        try:
            if killswitch.isSet():
                logger.debug("Shutdown in progress: not starting remote job")
                error.set()
                return 1
            process = run_remote_command(
                config,
                logger,
                self.host,
                self.command,
                {
                    "PYTHONPATH": config.get('deploy', 'engine_ppath'),
                    "LD_LIBRARY_PATH": config.get('deploy', 'engine_lpath')
                },
                arguments=[id, jobhost, jobport]
            )
            # Wait for process to finish. In the meantime, if the killswitch
            # is set (by an exception in the main thread), forcibly kill our
            # job off.
            while process.poll() == None:
                if killswitch.isSet():
                    process.kill()
                else:
                    time.sleep(1)
            sout, serr = process.communicate()

            serr = serr.replace("Connection to %s closed.\r\n" % self.host, "")
            log_process_output("Remote command", sout, serr, logger)
        except Exception, e:
            logger.exception("Failed to run remote process %s (%s)" % (self.command, str(e)))
            error.set()
            return 1
        finally:
            limiter[self.host].release()
        if process.returncode != 0:
            logger.error(
                "Remote process %s failed (status: %d)" % (self.command, process.returncode)
            )
            error.set()
        return process.returncode

def threadwatcher(threadpool, logger, killswitch):
    """
    Start and watch a pool of threads. If an exception is thrown during
    processing, set the killswitch so that all threads can shut down cleanly,
    then join all the threads to wait for them to finish.

    :param threadpool: Pool of threads to handle
    :param logger: Logger
    :type logger: logging.Logger or descendant
    :param killswitch: Indication for threads to abort
    :type killswitch: threading.Event
    """
    # If we receive a SIGTERM, shut down processing.
    signal.signal(signal.SIGTERM, killswitch.set)
    try:
        # Start all the threads, but don't just join them, as that
        # blocks all exceptions in the main thread. Instead, we wake
        # up every second to handle exceptions.
        [thread.start() for thread in threadpool]
        logger.info("Waiting for compute threads...")

        while True in [thread.isAlive() for thread in threadpool]:
            time.sleep(1)
    except:
        # If something throws an exception (normally a
        # KeyboardException, ctrl-c) set the kill switch to tell the
        # comput threads to terminate, then wait for them.
        logger.warn("Processing interrupted: shutting down")
        killswitch.set()
    finally:
        # Always make sure everything has finished. Note that if an exception
        # is thrown before all the threads have started, they will not all be
        # alive (and hence not join()-able).
        [thread.join() for thread in threadpool if thread.isAlive()]

class RemoteCommandRecipeMixIn(object):
    """
    Mix-in for recipes to dispatch jobs using the remote command mechanism.
    """
    def _schedule_jobs(self, jobs, max_per_node=None):
        """
        Schedule a series of compute jobs. Blocks until completion.

        :param jobs: iterable of :class:`~lofarpipe.support.remotecommand.ComputeJob` to be scheduled
        :param max_per_node: maximum number of simultaneous jobs on any given node
        :type max_per_node: integer or none
        :rtype: dict mapping integer job id to :class:`~lofarpipe.support.remotecommand.ComputeJob`
        """
        threadpool = []
        jobpool = {}
        limiter = ProcessLimiter(max_per_node)
        killswitch = threading.Event()

        if max_per_node:
            self.logger.info("Limiting to %d simultaneous jobs/node" % max_per_node)

        with job_server(self.logger, jobpool, self.error) as (jobhost, jobport):
            self.logger.debug("Job dispatcher at %s:%d" % (jobhost, jobport))
            for job_id, job in enumerate(jobs):
                jobpool[job_id] = job
                threadpool.append(
                    threading.Thread(
                        target=job.dispatch,
                        args=(
                            self.logger, self.config, limiter, job_id,
                            jobhost, jobport, self.error, killswitch
                        )
                    )
                )
            threadwatcher(threadpool, self.logger, killswitch)
        return jobpool
