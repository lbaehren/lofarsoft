"""
AntennaResponse documentation
=============================

"""

from pycrtools.tasks import Task
import pycrtools as cr

class AntennaResponse(Task):
    """AntennaResponse task documentation.
    """

    parameters = dict(
        direction = dict( default = (0, 0),
            doc = "Direction in degrees." ),
    )

    def run(self):
        """Run.
        """

        print "Running AntennaResponse..."

