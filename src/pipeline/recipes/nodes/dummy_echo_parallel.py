import subprocess
from lofarpipe.support.lofarnode import LOFARnode

class dummy_echo_node(LOFARnode):
    def run(self, filename, executable):
        import time, random
        time.sleep(random.randint(1,5))
        self.logger.info("Processing %s" % (filename))
        execute = [executable, filename]

        my_process = subprocess.Popen(execute, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        sout, serr = my_process.communicate()
        self.logger.info("stdout: " + sout)
        self.logger.info("stderr: " + serr)

        return filename
