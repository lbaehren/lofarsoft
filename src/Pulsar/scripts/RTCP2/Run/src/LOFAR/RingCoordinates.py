#!/usr/bin/python

from math import sqrt

class RingCoordinates:
    def __init__(self, numrings, width):
        self.numrings = numrings
        self.width    = width

    def len_edge(self):
        """
          _
         / \ 
         \_/
         |.|
        """
        return self.width / sqrt(3)

    def len_width(self):
        """
          _
         / \ 
         \_/
        |...|
        """
        return 2 * self.len_edge()

    def len_height(self):
        """
         _  _
        / \ :
        \_/ _
             
        """
        return self.len_width()

    def delta_width(self):
        """
         _ 
        / \_
        \_/ \ 
          \_/
         |.|
        """
        return 1.5 * self.len_edge()

    def delta_height(self):
        """
         _
        / \_  -
        \_/ \ -
          \_/  
        """
        return 0.5 * self.len_width()

    def coordinates(self):
        if self.numrings == 0:
          return []

        coordinates = [(0,0)] # start with central beam

        # stride for each side, starting from the top, clock-wise
        dl = [0] * 6
        dm = [0] * 6

        #  _    
        # / \_  
        # \_/ \ 
        #   \_/ 
        dl[0] = self.delta_width()
        dm[0] = -self.delta_height()

        #  _  
        # / \ 
        # \_/ 
        # / \ 
        # \_/ 
        dl[1] = 0
        dm[1] = -self.len_height()

        #    _  
        #  _/ \ 
        # / \_/ 
        # \_/   
        dl[2] = -self.delta_width()
        dm[2] = -self.delta_height()

        #  _    
        # / \_  
        # \_/ \ 
        #   \_/ 
        dl[3] = -self.delta_width()
        dm[3] = self.delta_height()

        #  _  
        # / \ 
        # \_/ 
        # / \ 
        # \_/ 
        dl[4] = 0
        dm[4] = self.len_height()

        #    _  
        #  _/ \ 
        # / \_/ 
        # \_/   
        dl[5] = self.delta_width()
        dm[5] = self.delta_height()

        # ring 1-n: create the pencil beams from the inner ring outwards
        for r in xrange(1,self.numrings+1):
          # start from the top
          l = 0.0
          m = self.len_height() * r

          for side in xrange(6):
            # every side has length r
            for b in xrange(r):
              coordinates.append( (l,m) )
              l += dl[side]
              m += dm[side]

        return coordinates

