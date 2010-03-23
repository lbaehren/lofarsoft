import sys, os
from lofarpipe.support.pyraprunner import pyraprunner
from lofarpipe.support.sourcelist import SourceList

class flagger(pyraprunner):
    @staticmethod
    def remote_function(input, output, max_value):
        from lofarrecipe.nodes.flagger import flagger_node
        return flagger_node(loghost=loghost, logport=logport).run(input, output, max_value)

    def __init__(self):
        super(flagger, self).__init__()
        self.optionparser.add_option(
            '-s', '--skymodel',
            dest="skymodel",
            help="initial sky model (in makesourcedb format)"
        )
        self.optionparser.add_option(
            '--n-factor',
            dest="n_factor",
            type="float",
            help="Custom factor for flagging threshold"
        )

    def _generate_arguments(self):
        self.inputs['skymodel'] = os.path.join(
            self.config.get("layout", "parset_directory"),
            self.inputs['skymodel']
        )
        self.logger.info("Using %s for %s skymodel" %
            (self.inputs['skymodel'], "flagger")
        )
        if not os.access(self.inputs['skymodel'], os.R_OK):
            raise IOError

        sl = SourceList(self.inputs['skymodel'])
        return float(self.inputs['n_factor']) * sum(float(s['I']) for s in sl)

if __name__ == '__main__':
    sys.exit(flagger().main())
