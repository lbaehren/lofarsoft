#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                     Cluster description handler
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

import os.path
import lofar.parameterset
from lofarpipe.support.lofarexceptions import ClusterError

class ClusterDesc(object):
    """
    Wrap a clusterdesc file, providing a more convenient, Pythonic interface
    for accessing its contents.
    """
    def __init__(self, filename):
        self.filename = filename
        self.parameterset = lofar.parameterset.parameterset(self.filename)
        self.name = self.parameterset.getString('ClusterName')
        self.keylist = []
        with open(filename, 'r') as file:
            for line in file.readlines():
                if len(line.strip()) == 0 or line.strip()[0] == '#': continue
                self.keylist.append(line.split('=')[0].strip())
        try:
            subclusters = self.parameterset.get("SubClusters").expand().getStringVector()
            self.subclusters = [
                ClusterDesc(
                    os.path.join(os.path.dirname(self.filename), subcluster)
                )
                for subcluster in subclusters
            ]
        except RuntimeError:
            self.subclusters = []

    def get(self, key, recursive=True):
        values = []
        if key in self.keylist:
            values.extend(self.parameterset.get(key).expand().getStringVector())
        if recursive:
            for subcluster in self.subclusters:
                values.extend(subcluster.get(key, recursive=True))
        return values

    def keys(self, recursive=True):
        values = self.keylist[:]
        if recursive:
            for subcluster in self.subclusters:
                values.extend(subcluster.keys(recursive=True))
        return list(set(values))

    def __str__(self):
        return "ClusterDesc: " + self.name

def get_compute_nodes(clusterdesc):
    """
    Return a list of all compute nodes defined (under the key "Compute.Nodes")
    in the ClusterDesc class object clusterdesc.
    """
    try:
        return clusterdesc.get('Compute.Nodes')
    except:
        raise ClusterError("Unable to find compute nodes in clusterdesc.")

def get_head_node(clusterdesc):
    """
    Return the head node, defined by the key "Head.Nodes" in the ClusterDesc
    class object clusterdesc.

    Always return the first head node, even if there are several defined.
    """
    try:
        return [clusterdesc.get('Head.Nodes')[0]]
    except:
        raise ClusterError("Unable to find head node in clusterdesc.")
