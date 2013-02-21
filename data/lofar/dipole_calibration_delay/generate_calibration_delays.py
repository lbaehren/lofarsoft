"""Script to generate dipole calibration delay files from the LOFAR CalTables
LOFAR CalTables come from:
svn co https://svn.astron.nl/Station/trunk/CalTables

You need read permissions to access those.

The resulting files are used by tbbmd to populate the DIPOLE_CALIBRATION_DELAY field in
LOFAR TBB files."""

import numpy as np

from pycrtools import metadata as md
antenna_sets = [#"LBA_SPARSE_EVEN",
                #"LBA_SPARSE_ODD",
                #"LBA_X",
                #"LBA_Y",
                "HBA_DUAL",
                "HBA_DUAL_INNER",
                "HBA_JOINED",
                "HBA_JOINED_INNER",
                "HBA_ONE",
                "HBA_ONE_INNER",
                "HBA_ZERO",
                "HBA_ZERO_INNER",
                "LBA_INNER",
                "LBA_OUTER"
                ]


stations = ["CS001", "CS002", "CS003",
            "CS004", "CS005", "CS006",
            "CS007", "CS011", "CS013",
            "CS017", "CS021", "CS024",
            "CS026", "CS028", "CS030",
            "CS031", "CS032", "CS101",
            "CS103", "CS201", "CS301",
            "CS302", "CS401", "CS501",
            "RS106", "RS205", "RS208",
            "RS306", "RS307", "RS406",
            "RS503", "RS509"]#, "UK608",
#            "DE601", "DE602", "DE603",
#            "DE604", "DE605", "FR606",]

for antenna_set in antenna_sets:
    f = open("LOFAR_DIPOLE_CALIBRATION_DELAY_"+antenna_set, 'w')

    for station in stations:
        phasecal = md.getStationPhaseCalibration(station, antenna_set, caltable_location="/Users/acorstanje/Station/CalTables/"+station)

        phases = np.angle(phasecal)
        delays = (phases[:, 1] - phases[:, 0]) * (1024 / (2*np.pi)) * 5.0e-9 # delay in seconds, 5.0 ns = sample time

        print "LOFAR_DIPOLE_CALIBRATION_DELAY_"+antenna_set, station
        #positions = md.getAbsoluteAntennaPositions(station, md.mapAntennasetKeyword(antenna_set))

        for i in range(delays.shape[0]):
            f.write("{0:03}{1:03}{2:03} {3:e}\n".format(int(station[2:]), i/8, i, delays[i]))

    f.close()

