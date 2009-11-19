from fabric.api import env, hosts, run, put, get, env, require
from pipeline.support.clusterdesc import ClusterDesc
import os.path

from ConfigParser import SafeConfigParser as ConfigParser

# Support function
def _get_config(filename):
    config = ConfigParser()
    config.read(filename)
    return config

# These functions actually do the work
def head_node(configfile=os.path.expanduser('~/.pipeline.cfg')):
    clusterdesc = ClusterDesc(
        _get_config(configfile).get('cluster', 'clusterdesc')
    )
    env.hosts = clusterdesc.get('HeadNode')

def compute_nodes(configfile=os.path.expanduser('~/.pipeline.cfg')):
    clusterdesc = ClusterDesc(
        _get_config(configfile).get('cluster', 'clusterdesc')
    )
    env.hosts = clusterdesc.get('ComputeNodes')

def start_controller(configfile=os.path.expanduser('~/.pipeline.cfg')):
    require('hosts', provided_by=[head_node])
    controlpath = _get_config(configfile).get('DEFAULT', 'runtime_directory')
    script_path = _get_config(configfile).get('deploy', 'script_path')
    run("bash %s/ipcontroller.sh %s start" % (script_path, controlpath))

def stop_controller(configfile=os.path.expanduser('~/.pipeline.cfg')):
    require('hosts', provided_by=[head_node])
    controlpath = _get_config(configfile).get('DEFAULT', 'runtime_directory')
    script_path = _get_config(configfile).get('deploy', 'script_path')
    run("bash %s/ipcontroller.sh %s stop" % (script_path, controlpath))

def start_engine(configfile=os.path.expanduser('~/.pipeline.cfg')):
    require('hosts', provided_by=[compute_nodes])
    controlpath = _get_config(configfile).get('DEFAULT', 'runtime_directory')
    script_path = _get_config(configfile).get('deploy', 'script_path')
    run("bash %s/ipengine.sh %s start" % (script_path, controlpath))

def stop_engine(configfile=os.path.expanduser('~/.pipeline.cfg')):
    require('hosts', provided_by=[compute_nodes])
    controlpath = _get_config(configfile).get('DEFAULT', 'runtime_directory')
    script_path = _get_config(configfile).get('deploy', 'script_path')
    run("bash %s/ipengine.sh %s stop" % (script_path, controlpath))
