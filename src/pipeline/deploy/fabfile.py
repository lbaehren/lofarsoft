from fabric.api import env, hosts, run, put, get, require
from lofarpipe.support.clusterdesc import ClusterDesc
from lofarpipe.support.clusterdesc import get_compute_nodes, get_head_node
import os.path

from ConfigParser import SafeConfigParser as ConfigParser

# Support function
def _get_config(filename):
    if not os.path.isfile(filename):
        raise IOError, "Configuration file not found"
    config = ConfigParser()
    config.read(filename)
    return config

# These functions actually do the work
def head_node(configfile='~/.pipeline.cfg'):
    clusterdesc = ClusterDesc(
        _get_config(
            os.path.expanduser(configfile)
        ).get('cluster', 'clusterdesc')
    )
    env.hosts = get_head_node(clusterdesc)

def compute_nodes(configfile='~/.pipeline.cfg'):
    clusterdesc = ClusterDesc(
        _get_config(
            os.path.expanduser(configfile)
        ).get('cluster', 'clusterdesc')
    )
    env.hosts = get_compute_nodes(clusterdesc)

def start_controller(configfile='~/.pipeline.cfg'):
    configfile = os.path.expanduser(configfile)
    require('hosts', provided_by=[head_node])
    controlpath = _get_config(configfile).get('DEFAULT', 'runtime_directory')
    controller_ppath = _get_config(configfile).get('deploy', 'controller_ppath')
    script_path = _get_config(configfile).get('deploy', 'script_path')
    run("bash %s/ipcontroller.sh %s start %s" % (script_path, controlpath, controller_ppath), shell=False)

def stop_controller(configfile='~/.pipeline.cfg'):
    configfile = os.path.expanduser(configfile)
    require('hosts', provided_by=[head_node])
    controlpath = _get_config(configfile).get('DEFAULT', 'runtime_directory')
    script_path = _get_config(configfile).get('deploy', 'script_path')
    run("bash %s/ipcontroller.sh %s stop" % (script_path, controlpath), shell=False)

def start_engine(configfile='~/.pipeline.cfg'):
    configfile = os.path.expanduser(configfile)
    require('hosts', provided_by=[compute_nodes])
    controlpath = _get_config(configfile).get('DEFAULT', 'runtime_directory')
    engine_ppath = _get_config(configfile).get('deploy', 'engine_ppath')
    engine_lpath = _get_config(configfile).get('deploy', 'engine_lpath')
    script_path = _get_config(configfile).get('deploy', 'script_path')
    run("bash %s/ipengine.sh %s start %s %s" % (script_path, controlpath, engine_ppath, engine_lpath), shell=False)

def stop_engine(configfile='~/.pipeline.cfg'):
    configfile = os.path.expanduser(configfile)
    require('hosts', provided_by=[compute_nodes])
    controlpath = _get_config(configfile).get('DEFAULT', 'runtime_directory')
    script_path = _get_config(configfile).get('deploy', 'script_path')
    run("bash %s/ipengine.sh %s stop" % (script_path, controlpath), shell=False)
