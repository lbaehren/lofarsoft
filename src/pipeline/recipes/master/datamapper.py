#                                                         LOFAR IMAGING PIPELINE
#
#                                 Map subbands on storage nodes to compute nodes
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from itertools import cycle
from tempfile import mkstemp
import cPickle as pickle

from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.clusterdesc import ClusterDesc, get_compute_nodes

class datamapper(LOFARrecipe):
    def go(self):
        self.logger.info("Starting datamapper run")
        super(datamapper, self).go()

        #      We build lists of compute-nodes per cluster and data-per-cluster,
        #          then match them up to schedule jobs in a round-robin fashion.
        # ----------------------------------------------------------------------
        clusterdesc = ClusterDesc(self.config.get('cluster', "clusterdesc"))
        if clusterdesc.subclusters:
            available_nodes = dict(
                (cl.name, cycle(get_compute_nodes(cl)))
                for cl in clusterdesc.subclusters
            )
        else:
            available_nodes = {
                clusterdesc.name: cycle(get_compute_nodes(clusterdesc))
            }

        data = []
        for filename in self.inputs['args']:
            subcluster = filename.split('/')[2]
            host = available_nodes[subcluster].next()
            data.append((host, filename))

        #                         Dump the generated mapping to a temporary file
        # ----------------------------------------------------------------------
        self.outputs['mapfile'] = mkstemp()[1]
        self.logger.debug("Mapfile is %s" % self.outputs['mapfile'])
        dumpfile = open(self.outputs['mapfile'], 'w')
        pickle.dump(data, dumpfile)
        dumpfile.close()

        return 0

if __name__ == '__main__':
    sys.exit(datamapper().main())



