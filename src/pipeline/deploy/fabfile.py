from fabric.api import env, hosts, run, put, get, env, require
from pipeline.support.clusterdesc import ClusterDesc

from ConfigParser import SafeConfigParser as ConfigParser
from pipeline import __path__ as config_path
config = ConfigParser()
config.read("%s/pipeline.cfg" % (config_path[0],))

script_path = config.get('deploy', 'script_path')

def head_node(clusterdesc=config.get('cluster', 'clusterdesc')):
    clusterdesc = ClusterDesc(clusterdesc)
    env.hosts = clusterdesc.get('HeadNode')

def compute_nodes(clusterdesc=config.get('cluster', 'clusterdesc')):
    clusterdesc = ClusterDesc(clusterdesc)
    env.hosts = clusterdesc.get('ComputeNodes')

def start_controller(controlpath=config.get('DEFAULT', 'runtime_directory')):
    require('hosts', provided_by=[head_node])
    run("bash %s/ipcontroller.sh %s start" % (script_path, controlpath))

def stop_controller(controlpath=config.get('DEFAULT', 'runtime_directory')):
    require('hosts', provided_by=[head_node])
    run("bash %s/ipcontroller.sh %s stop" % (script_path, controlpath))

def start_engine(controlpath=config.get('DEFAULT', 'runtime_directory')):
    require('hosts', provided_by=[compute_nodes])
    run("bash %s/ipengine.sh %s start" % (script_path, controlpath))

def stop_engine(controlpath=config.get('DEFAULT', 'runtime_directory')):
    require('hosts', provided_by=[compute_nodes])
    run("bash %s/ipengine.sh %s stop" % (script_path, controlpath))
