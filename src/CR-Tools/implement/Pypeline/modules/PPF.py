"""Polyphase filters.
"""

import pycrtools as cr

class PPF():
    """Class documentation"""

    def __init__(self):
        """Constructor"""
        
        weights_filename=cr.LOFARSOFT+'/src/CR-Tools/data/ppfWeights16384.dat'
        # Initialize arrays and values
        weights=cr.hArray(float,[16,1024])
        weights.readdump(weights_filename)

        if weights[0,0]==0.0:
            print "Obtaining Kaiser coefficient from file"
            # Reading of weights failed
            f=open(cr.LOFARSOFT+'/src/CR-Tools/data/Coeffs16384Kaiser-quant.dat')
            weights.setDim([16*1024])
            f.seek(0)
            for i in range(0,16*1024):
                weights[i]=float(f.readline())
            weights.setDim([16,1024])
            weights.writedump(weights_filename)
        

        self.weights=weights
        self.buffer=None#cr.hArray(float,[16,1024])
        self.startrow=0
        self.total_rows_added=0

    def add(self,input):
        if not self.buffer:
            assert input.getSize() == 1024 or input.getDim()[1] == 1024
            
            size=input.getSize() 
            firstdim=size/1024    
            self.buffer=cr.hArray(float,[16,firstdim,1024])
            weights2=self.buffer.new()
            weights2[...].copy(self.weights[...])
            self.weights=weights2

        
       # print "adding data in row",self.startrow
        self.buffer[self.startrow].copy(input)
        input.fill(0)
        self.startrow += 1
        if self.startrow > 15:
            self.startrow = 0
        input.muladd2(self.weights[0:16-self.startrow],self.buffer[self.startrow:16])
        if self.startrow != 0:
	    input.muladd2(self.weights[16-self.startrow:16],self.buffer[0:self.startrow])
           
           # print "multiplying weight",row,"with buffer",(row+self.startrow)%16
         
        self.total_rows_added+=1
        if self.total_rows_added < 16:
            input.fill(0)
            return False
        
        return True



class iPPF():
    """Class documentation"""

    def __init__(self):
        """Constructor"""
        
        weights_filename=cr.LOFARSOFT+'/src/CR-Tools/data/ippfWeights16384.dat'
        # Initialize arrays and values
        weights=cr.hArray(float,[16,1024])
        weights.readdump(weights_filename)
	
	
        if weights[0,0]==0.0:
            print "Obtaining inverse PPF coefficient from file"
            # Reading of weights failed
            f=open(cr.LOFARSOFT+'/src/CR-Tools/data/ppf_inv.dat')
            f.seek(0)
            for i in range(0,16):
                for j in range(0,1024):
                    weights[i,j]=float(f.readline())           
            #weights.setDim([16,1024])
            weights.writedump(weights_filename)
        
        self.weights=weights
        self.buffer=None#cr.hArray(float,[16,1024])
        self.startrow=0
        self.total_rows_added=0

    def add(self,input):
        if not self.buffer:
            assert input.getSize() == 1024 or input.getDim()[1] == 1024
            
            size=input.getSize() 
            firstdim=size/1024    
            self.buffer=cr.hArray(float,[16,firstdim,1024])
            weights2=self.buffer.new()
            weights2[...].copy(self.weights[...])
            self.weights=weights2
        
        self.buffer[self.startrow].copy(input)
        input.fill(0)
        
        #print "adding data in row",self.startrow
        self.startrow += 1
        if self.startrow > 15:
            self.startrow = 0
        input.muladd2(self.weights[0:16-self.startrow],self.buffer[self.startrow:16])
        if self.startrow != 0:
	    input.muladd2(self.weights[16-self.startrow:16],self.buffer[0:self.startrow])
           # print "multiplying weight",row,"with buffer",(row+self.startrow)%16
        self.total_rows_added+=1
        if self.total_rows_added < 16:
            input.fill(0)
            return False
        
        return True

# Execute doctests if module is executed as script
if __name__=='__main__':
    import doctest
    doctest.testmod()

