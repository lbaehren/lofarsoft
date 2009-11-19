from fabric.api import env, hosts, run, put, get
import fabsupport

clusterdesc = fabsupport.ClusterDesc('lioff.clusterdesc')

@hosts(clusterdesc['HeadNode'])
def run_controller(controlpath="/data/users/swinbank/pipeline_runtime"):
    run("bash %s/ipython/ipcontroller.sh %s start" % (controlpath, controlpath))

@hosts(clusterdesc['HeadNode'])
def stop_controller(controlpath="/data/users/swinbank/pipeline_runtime"):
    run("bash %s/ipython/ipcontroller.sh %s stop" % (controlpath, controlpath))

@hosts(*clusterdesc['ComputeNodes'])
def run_engines(controlpath="/data/users/swinbank/pipeline_runtime"):
    run("bash %s/ipython/ipengine.sh %s start" % (controlpath, controlpath))

@hosts(*clusterdesc['ComputeNodes'])
def stop_engines(controlpath="/data/users/swinbank/pipeline_runtime"):
    run("bash %s/ipython/ipengine.sh %s stop" % (controlpath, controlpath))
