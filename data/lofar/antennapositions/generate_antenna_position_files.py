"""Script to generate antenna position files using metadata module.

The resulting ITRF position files are used by tbbmd to write antenna
positions to the TBB files."""

from pycrtools import metadata as md

antenna_sets = ["LBA_SPARSE_EVEN",
                "LBA_SPARSE_ODD",
                "LBA_X",
                "LBA_Y",
                "HBA_DUAL",
                "HBA_JOINED",
                "HBA_ONE",
                "HBA_ZERO",
                "LBA_INNER",
                "LBA_OUTER"
                ]

# Please add and check the following datasets
#                "HBA_DUAL_INNER",
#                "HBA_JOINED_INNER",
#                "HBA_ONE_INNER",
#                "HBA_ZERO_INNER",

stations = ["CS001", "CS002", "CS003", 
            "CS004", "CS005", "CS006", 
            "CS007", "CS011", "CS013", 
            "CS017", "CS021", "CS024", 
            "CS026", "CS028", "CS030", 
            "CS031", "CS032", "CS101", 
            "CS103", "CS201", "CS301", 
            "CS302", "CS401", "CS501", 
            "RS106", "RS205", "RS208",
            "RS305", "RS306", "RS307",
            "RS406", "RS407", "RS503",
            "RS508", "RS509",
            "DE601", "DE602", "DE603",
            "DE604", "DE605", "FR606",
            "SE607", "UK608"]

for antenna_set in antenna_sets:
    f = open("LOFAR_ITRF_"+antenna_set, 'w')

    for station in stations:
        print "LOFAR_ITRF_"+antenna_set, station
        try:
            positions = md.getAbsoluteAntennaPositions(station, md.mapAntennasetKeyword(antenna_set))
        
            for i in range(positions.shape[0]):
                f.write("{0:03}{1:03}{2:03} {3:7.5f} {4:7.5f} {5:7.5f}\n".format(int(station[2:]), i/8, i, positions[i][0], positions[i][1], positions[i][2]))

        except:
            print "have no input data for station", station

    f.close()
    
