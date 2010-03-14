from __future__ import with_statement
from subprocess import check_call, CalledProcessError
from contextlib import closing

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe

class dummy_echo(LOFARrecipe):
    def __init__(self):
        super(dummy_echo, self).__init__()
        self.optionparser.add_option(
            '--executable',
            dest="executable",
            help="Executable to be run (ie, dummy_echo.sh)",
            default="/home/swinbank/sw/bin/dummy_echo.sh"
        )

    def go(self):
        self.logger.info("Starting dummy_echo run")
        super(dummy_echo, self).go()

        print self.inputs['args']
        check_call([self.inputs['executable'], self.inputs['args']])

        return 0


if __name__ == '__main__':
    sys.exit(bbs().main())
