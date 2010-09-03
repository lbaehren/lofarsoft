#                                                         LOFAR IMAGING PIPELINE
#
#                                 Map subbands on storage nodes to compute nodes
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from itertools import cycle
from collections import defaultdict

from lofarpipe.support.lofarrecipe import BaseRecipe
from lofarpipe.support.clusterdesc import ClusterDesc, get_compute_nodes
from lofarpipe.support.parset import Parset
import lofarpipe.support.lofaringredient as ingredient

class datamapper(BaseRecipe):
    inputs = {
        'mapfile': ingredient.StringField(
            '--mapfile',
            help="Filename for output mapfile (clobbered if exists)"
        )
    }

    outputs = {
        'mapfile': ingredient.FileField()
    }

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

        data = defaultdict(list)
        for filename in self.inputs['args']:
            subcluster = filename.split('/')[2]
            try:
                host = available_nodes[subcluster].next()
            except KeyError, key:
                self.logger.error("%s is not a known cluster" % str(key))
                raise

            data[host].append(filename)

        #                                 Dump the generated mapping to a parset
        # ----------------------------------------------------------------------
        parset = Parset()
        for host, filenames in data.iteritems():
            parset.addStringVector(host, filenames)

        self.outputs['mapfile'] = self.inputs['mapfile']
        parset.writeFile(self.outputs['mapfile'])

        return 0

if __name__ == '__main__':
    sys.exit(datamapper().main())
