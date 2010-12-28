"""
This is a very simple pipeline definition for demonstration purposes only.
"""
import sys
from lofarpipe.support.control import control

class demo(control):
    def pipeline_logic(self):
        self.run_task("dummy_echo", self.inputs['args'])

if __name__ == '__main__':
    sys.exit(demo().main())
