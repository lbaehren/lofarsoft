# Operating environment for the pulsar pipeline, pulp.

# These paths really only set up where the pulp PULSAR ARCHIVE
# output will go.
#
# K. Anderson
# UvA.
# 2010-08-04

import os

class pulpEnv:

    """ 
    Set some paths for pulp  pipeline processing archive.
    Nominally called, PULSAR_ARCHIVE
    """

    def __init__(self):

        self.stokes = 'incoherentstokes'
        self.arch   = '/net/sub5/lse015/data4/PULSAR_ARCHIVE'
        try:
            self.TEMPO  = os.environ["TEMPO"]
        except KeyError, err:
            raise KeyError, "$TEMPO not defined."
        try:
            self.PRESTO  = os.environ["PRESTO"]
        except KeyError, err:
            raise KeyError, "$PRESTO not defined"
        

        # arch131 = '/net/sub5/lse013/data1/PULSAR_ARCHIVE'
        # arch132 = '/net/sub5/lse013/data2/PULSAR_ARCHIVE'
        # arch123 = '/net/sub5/lse013/data3/PULSAR_ARCHIVE'
        #!arch134 = '/net/sub5/lse013/data4/PULSAR_ARCHIVE'
        
        # arch141 = '/net/sub5/lse014/data1/PULSAR_ARCHIVE'
        # arch142 = '/net/sub5/lse014/data2/PULSAR_ARCHIVE'
        # arch143 = '/net/sub5/lse014/data3/PULSAR_ARCHIVE'
        #!arch144 = '/net/sub5/lse014/data4/PULSAR_ARCHIVE'
        
        # arch151 = '/net/sub5/lse015/data1/PULSAR_ARCHIVE'
        # arch152 = '/net/sub5/lse015/data2/PULSAR_ARCHIVE'
        # arch153 = '/net/sub5/lse015/data3/PULSAR_ARCHIVE'
        #!arch154 = '/net/sub5/lse015/data4/PULSAR_ARCHIVE'


