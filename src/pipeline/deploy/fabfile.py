from fabric.api import env, hosts, run, put, get, env, require
import fabsupport

script_path = '/home/swinbank/Work/lofar_pipes/deploy'

def controller_host(clusterdesc):
    clusterdesc = fabsupport.ClusterDesc(clusterdesc)
    env.hosts = [clusterdesc['HeadNode']]

def engine_hosts(clusterdesc):
    clusterdesc = fabsupport.ClusterDesc(clusterdesc)
    env.hosts = clusterdesc['ComputeNodes']

def run_controller(controlpath="/data/users/swinbank/pipeline_runtime"):
    require('hosts', provided_by=[controller_host])
    run("bash %s/ipcontroller.sh %s start" % (script_path, controlpath))

def stop_controller(controlpath="/data/users/swinbank/pipeline_runtime"):
    require('hosts', provided_by=[controller_host])
    run("bash %s/ipcontroller.sh %s stop" % (script_path, controlpath))

def run_engine(controlpath="/data/users/swinbank/pipeline_runtime"):
    require('hosts', provided_by=[engine_hosts])
    run("bash %s/ipengine.sh %s start" % (script_path, controlpath))

def stop_engine(controlpath="/data/users/swinbank/pipeline_runtime"):
    require('hosts', provided_by=[engine_hosts])
    run("bash %s/ipengine.sh %s stop" % (script_path, controlpath))
