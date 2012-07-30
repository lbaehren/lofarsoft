from Partitions import PartitionPsets
import socket
import os

class CommandClient:
    def __init__( self, partition, port = 4000 ):
        self.partition = partition
        self.server = PartitionPsets[partition][0]
        self.port = port

        self.fd = -1

        self.connect()

    def connect( self ):
        self.fd = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

        try:
          self.fd.connect( (self.server, self.port ) )
        except socket.error, msg:
          self.close()
          raise

    def send( self, str ):
        self.fd.send( "%s\n" % (str,) )

    def close( self ):
        self.fd.close()
        self.fd = -1

def sendCommand( partition, command ):
  cc = CommandClient( partition )
  cc.send( command )
  cc.close()
