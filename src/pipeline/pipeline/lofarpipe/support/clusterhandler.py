import shlex, subprocess, threading
from lofarpipe.support.clusterdesc import ClusterDesc
from lofarpipe.support.clusterdesc import get_compute_nodes, get_head_node

class ClusterHandler(object):
    def __init__(self, config):
        clusterdesc = ClusterDesc(config.get('cluster', 'clusterdesc'))
        self.head_node = get_head_node(clusterdesc)[0]
        self.compute_nodes = get_compute_nodes(clusterdesc)
        self.script_path = config.get('deploy', 'script_path')
        self.config = config

    def start_cluster(self):
        self.__start_controller()
        self.__start_engines()

    def stop_cluster(self):
        self.__stop_controller()
        self.__stop_engines()

    def __execute_ssh(self, host, command):
        ssh_cmd = shlex.split("ssh %s -- %s" % (host, command))
        subprocess.check_call(ssh_cmd)
        print "  *", host

    def __start_controller(self):
        print "Starting controller:"
        controlpath = self.config.get('DEFAULT', 'runtime_directory')
        controller_ppath = self.config.get('deploy', 'controller_ppath')
        self.__execute_ssh(self.head_node, "bash %s/ipcontroller.sh %s start %s" % (self.script_path, controlpath, controller_ppath))
        print "done."

    def __stop_controller(self):
        print "Stopping controller:"
        controlpath = self.config.get('DEFAULT', 'runtime_directory')
        controller_ppath = self.config.get('deploy', 'controller_ppath')
        self.__execute_ssh(self.head_node, "bash %s/ipcontroller.sh %s stop %s" % (self.script_path, controlpath, controller_ppath))
        print "done."

    def __start_engines(self):
        print "Starting engines:"
        controlpath = self.config.get('DEFAULT', 'runtime_directory')
        engine_ppath = self.config.get('deploy', 'engine_ppath')
        engine_lpath = self.config.get('deploy', 'engine_lpath')
        ssh_connections = [
            threading.Thread(
                target = self.__execute_ssh,
                args = (node, "bash %s/ipengine.sh %s start %s %s" % (self.script_path, controlpath, engine_ppath, engine_lpath))
            ) for node in self.compute_nodes
        ]
        [thread.start() for thread in ssh_connections]
        [thread.join() for thread in ssh_connections]
        print "done."

    def __stop_engines(self):
        print "Stopping engines:"
        controlpath = self.config.get('DEFAULT', 'runtime_directory')
        ssh_connections = [
            threading.Thread(
                target = self.__execute_ssh,
                args = (node, "bash %s/ipengine.sh %s stop" % (self.script_path, controlpath))
            ) for node in self.compute_nodes
        ]
        [thread.start() for thread in ssh_connections]
        [thread.join() for thread in ssh_connections]
        print "done."
