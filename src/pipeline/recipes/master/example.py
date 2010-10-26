import subprocess, sys

from lofarpipe.support.lofarrecipe import LOFARrecipe

class example(LOFARrecipe):
    inputs = {
        'executable': ingredients.ExecField(
            '--executable'
            help="Command to run"
            default="/bin/ls"
        )
    }

    def go(self):
        self.logger.info("Starting example recipe run")
        super(example, self).go()

        self.logger.info("This is a log message")

        return 0

#        execute = [self.inputs['executable']]
#        execute.extend(self.inputs['args'])
#
#        my_process = subprocess.Popen(execute, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
#        sout, serr = my_process.communicate()
#        self.logger.info("stdout: " + sout)
#        self.logger.info("stderr: " + serr)
#
#        if my_process.returncode == 0:
#            return 0
#        else:
#            return 1


if __name__ == '__main__':
    sys.exit(example().main())
