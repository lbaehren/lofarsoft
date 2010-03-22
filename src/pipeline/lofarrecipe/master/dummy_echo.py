import subprocess
from lofarpipe.support.lofarrecipe import LOFARrecipe

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

        execute = [self.inputs['executable']]
        execute.extend(self.inputs['args'])

        my_process = subprocess.Popen(execute, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        sout, serr = my_process.communicate()
        self.logger.info("stdout: " + sout)
        self.logger.info("stderr: " + serr)

        return 0


if __name__ == '__main__':
    sys.exit(dummy_echo().main())
