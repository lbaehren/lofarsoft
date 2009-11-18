def hOpen(Filename):
    iptr=0;date=0; observatory=""; filesize=0; nofAntennas=0; maxBlocksize=0; Filetype=""; AntennaIDs=IntVec();  Offsets=IntVec();
    hOpenFile(iptr,Filename,date,observatory,filesize,nofAntennas, maxBlocksize,Filetype,AntennaIDs, Offsets);
    return (iptr,(Filename,date,observatory,filesize,nofAntennas, maxBlocksize,Filetype,AntennaIDs, Offsets))

