from lofarpipe.support.lofarrecipe import BaseRecipe
from lofarpipe.support.utilities import get_parset
import lofarpipe.support.utilities as utilities

class vdsreader(BaseRecipe):
    """
    Read a GVDS file and return a list of the MS filenames referenced therein
    together with selected metadata.
    """
    def __init__(self):
        super(vdsreader, self).__init__()
        self.optionparser.add_option(
            '-g', '--g(v)ds-file',
            dest="gvds",
            help="G(V)DS file describing data to be processed"
        )

    def go(self):
        self.logger.info("Starting vdsreader run")
        super(vdsreader, self).go()

        try:
            gvds = get_parset(self.inputs['gvds'])
        except:
            self.logger.error("Unable to read G(V)DS file")
            raise

        self.logger.info("Building list of measurementsets")
        ms_names = [
            gvds["Part%d.FileName" % (part_no,)]
            for part_no in xrange(int(gvds["NParts"]))
        ]
        self.logger.debug(ms_names)

        self.outputs['data'] = ms_names
        try:
            self.outputs['start_time'] = gvds['StartTime']
            self.outputs['end_time'] = gvds['EndTime']
        except:
            self.logger.warn("Failed to read start/end time from GVDS file")
        try:
            self.outputs['pointing'] = {
                'type': gvds.getStringVector('Extra.FieldDirectionType')[0],
                'dec': gvds.getStringVector('Extra.FieldDirectionDec')[0],
                'ra': gvds.getStringVector('Extra.FieldDirectionRa')[0]
            }
        except:
            self.logger.warn("Failed to read pointing information from GVDS file")
        return 0
