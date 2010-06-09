import sys
from ConfigParser import SafeConfigParser as ConfigParser
from lofarpipe.support.clusterhandler import ClusterHandler

config_file = sys.argv[1]
num_engines = sys.argv[2]

config = ConfigParser()
config.read(config_file)

clusterhandler = ClusterHandler(config)
clusterhandler.start_cluster(num_engines)

