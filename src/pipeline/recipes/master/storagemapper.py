#                                                         LOFAR IMAGING PIPELINE
#
#                        Generate a mapfile for processing data on storage nodes
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from collections import defaultdict

from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.parset import Parset
import lofarpipe.support.lofaringredient as ingredient

class storagemapper(BaseRecipe):
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
        self.logger.info("Starting storagemapper run")
        super(storagemapper, self).go()

        #                     We just read the storage node name out of the path
        # ----------------------------------------------------------------------
        data = defaultdict(list)
        for filename in self.inputs['args']:
            host = filename.split('/')[3]
            data[host].append(filename)

        #                                 Dump the generated mapping to a parset
        # ----------------------------------------------------------------------
        parset = Parset()
        for host, filenames in data.iteritems():
            parset.addStringVector(host, filenames)

        parset.writeFile(self.inputs['mapfile'])
        self.outputs['mapfile'] = self.inputs['mapfile']

        return 0

if __name__ == '__main__':
    sys.exit(storagemapper().main())
