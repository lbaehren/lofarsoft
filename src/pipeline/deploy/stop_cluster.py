import sys
from ConfigParser import SafeConfigParser as ConfigParser
from lofarpipe.support.clusterhandler import ClusterHandler

config_file = sys.argv[1]

config = ConfigParser()
config.read(config_file)

clusterhandler = ClusterHandler(config)
clusterhandler.stop_cluster()

