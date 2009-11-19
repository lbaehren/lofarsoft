# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities

class copier(LOFARrecipe):
    """
    Copy files to compute nodes.
    """
    def __init__(self):
        super(copier, self).__init__()
        self.optionparser.add_option(
            '--destination',
            dest="destination",
            help="Destination directory on compute nodes"
        )

    def go(self)
        self.logger.info("Starting copier run")
        tc, mec = self._get_cluster()

        mec.execute('import shutil')

        self.logger.info("Compiling list of output destinations")
        destinations = [
            os.path.join(
                self.inputs['destination'],
                os.path.basename(file)
            )
            for file in selt.inputs['args']
        ]
        self.logger.debug(destinations)

        self.logger.info("Copying files on cluster")
        try:
            tc.map(
                lambda x: shutil.copytree(x[0], x[1]),
                zip(self.inputs['args'], destinations)
            )
        except:
            return 1

        self.outputs['ms_names'] = destinations

        return 0


