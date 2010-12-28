#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                        Management of IPython cluster processes
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

import shlex
import subprocess
import threading
import logging
import os
import time

from contextlib import contextmanager

from lofarpipe.support.lofarexceptions import ClusterError
from lofarpipe.support.clusterdesc import ClusterDesc
from lofarpipe.support.clusterdesc import get_compute_nodes, get_head_node

class ClusterHandler(object):
    """
    ClusterHandler provides a convenient interface for setting up and tearing
    down an IPython cluser -- engines & controller -- over a network topology
    described by a clusterdesc file.
    """
    def __init__(self, config, logger=None):
        if not logger:
            logging.basicConfig()
            self.logger = logging.getLogger()
        else:
            self.logger = logger
        clusterdesc = ClusterDesc(config.get('cluster', 'clusterdesc'))
        self.head_node = get_head_node(clusterdesc)[0]
        self.compute_nodes = get_compute_nodes(clusterdesc)
        self.script_path = config.get('deploy', 'script_path')
        self.config = config

    def start_cluster(self, nproc=""):
        # Optional nproc argument specifies number of engines per node
        self.__start_controller()
        self.__start_engines(nproc)

    def stop_cluster(self):
        self.__stop_controller()
        self.__stop_engines()

    def __execute_ssh(self, host, command):
        ssh_cmd = shlex.split("ssh -x %s -- %s" % (host, command))
        subprocess.check_call(ssh_cmd)
        self.logger.info("  * %s" % (host))

    def __multinode_ssh(self, nodes, command):
        ssh_connections = [
            threading.Thread(
                target = self.__execute_ssh,
                args = (node, command)
            ) for node in nodes
        ]
        [thread.start() for thread in ssh_connections]
        [thread.join() for thread in ssh_connections]

    def __start_controller(self):
        self.logger.info("Starting controller:")
        controlpath = self.config.get('DEFAULT', 'runtime_directory')
        controller_ppath = self.config.get('deploy', 'controller_ppath')
        # Check that there isn't an existing pidfile
        if os.path.isfile(os.path.join(controlpath, "ipc.pid")):
            raise ClusterError("Controller already running")
        # Before starting, ensure that the old engine.furl isn't lying about
        # to cause confusion
        try:
            os.unlink(os.path.join(controlpath, 'engine.furl'))
        except OSError:
            pass
        self.__execute_ssh(self.head_node, "bash %s/ipcontroller.sh %s start %s" % (self.script_path, controlpath, controller_ppath))
        # Wait until an engine.furl file has been created before moving on to
        # start engines etc.
        while not os.path.isfile(os.path.join(controlpath, 'engine.furl')):
            time.sleep(1)
        self.logger.info("done.")

    def __stop_controller(self):
        self.logger.info("Stopping controller:")
        controlpath = self.config.get('DEFAULT', 'runtime_directory')
        controller_ppath = self.config.get('deploy', 'controller_ppath')
        self.__execute_ssh(self.head_node, "bash %s/ipcontroller.sh %s stop %s" % (self.script_path, controlpath, controller_ppath))
        os.unlink(os.path.join(controlpath, 'engine.furl'))
        self.logger.info("done.")

    def __start_engines(self, nproc):
        self.logger.info("Starting engines:")
        controlpath = self.config.get('DEFAULT', 'runtime_directory')
        engine_ppath = self.config.get('deploy', 'engine_ppath')
        engine_lpath = self.config.get('deploy', 'engine_lpath')
        with open(os.path.join(controlpath, 'engine.furl')) as furlfile:
            furl = furlfile.readline().strip()
        command = "bash %s/ipengine.sh %s start %s %s %s %s" % (self.script_path, controlpath, engine_ppath, engine_lpath, furl, str(nproc))
        self.__multinode_ssh(self.compute_nodes, command)
        self.logger.info("done.")

    def __stop_engines(self):
        self.logger.info("Stopping engines:")
        controlpath = self.config.get('DEFAULT', 'runtime_directory')
        command= "bash %s/ipengine.sh %s stop" % (self.script_path, controlpath)
        self.__multinode_ssh(self.compute_nodes, command)
        self.logger.info("done.")

@contextmanager
def ipython_cluster(config, logger, nproc=""):
    """
    Provides a context in which an IPython cluster is available. Designed for
    use within LOFAR pipeline recipes.

    THe optional nproc argument specifies the number of engines which will be
    started per compute node.
    """
    cluster_handler = ClusterHandler(config, logger)
    cluster_handler.start_cluster(nproc)
    # If an exception is raised during the pipeline run, make sure we catch it
    # and shut the cluster down cleanly.
    try:
        yield
    finally:
        cluster_handler.stop_cluster()
