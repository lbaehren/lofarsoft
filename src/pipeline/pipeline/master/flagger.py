import sys
from pyraprunner import pyraprunner
from pipeline.support.sourcelist import SourceList

class flagger(pyraprunner):
    @staticmethod
    def remote_function(input, output, max_value):
        from pipeline.nodes.flagger import flag_data
        return flag_data(input, output, max_value)

    def __init__(self):
        super(flagger, self).__init__()
        self.optionparser.add_option(
            '-s', '--skymodel',
            dest="skymodel",
            help="initial sky model (in makesourcedb format)"
        )
        self.optionparser.add_option(
            '-n', '--n-factor',
            dest="n_factor",
            type="float"
            help="Custom factor for flagging threshold"
        )

    def _generate_arguments(self):
        if not self.inputs['skymodel']:
            self.inputs['skymodel'] = "%s/%s" % (
                self.config.get("layout", "parset_directory"),
                self.config.get("bbs", "skymodel")
            )
            self.logger.info("Using %s for %s skymodel" %
                (self.inputs['parset'], "flagger")
            )
        if not os.access(self.inputs['skymodel'], os.R_OK):
            raise IOError

        sl = SourceList(self.inputs['skymodel'])
        return self.inputs['n_factor'] * sum(float(s['I']) for s in sl)

if __name__ == '__main__':
    sys.exit(flagger().main())
