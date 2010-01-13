#Definitions (for better readability)
#Run this using
#
# execfile("crprototype.py")
#
execfile("hfplot.py")

#A simple example of operator overloading
#----------------------------------------
def Vec_iadd(self,vec):
    "Provides the += operator for addig two vectors in place. \
    vec1+=vec2 2ill add all elements in vec2 to thw corresponding elements in vec1\
    and store the result in vec1."
    hAddVecs(vec,self)
    return self


ComplexVec.__iadd__=Vec_iadd
#----------------------------------------

global idata,fdata,cdata,sdata,output_vector
offsets=IntVec()
idata=IntVec()
fdata=FloatVec()
cdata=ComplexVec()
cdata2=ComplexVec()
sdata=StringVec()
output_vector=ComplexVec()
Datatype="Fx"
Antenna=1
Blocksize=1024
Block=0
Stride=0
Shift=0

#------------------------------------------------------------------------------------
#Begin
#------------------------------------------------------------------------------------

datareader_ptr=hOpenFile("/Users/falcke/LOFAR/usg/data/lopes/example.event", offsets)
hReadFile(cdata,datareader_ptr,"CalFFT",Antenna,Blocksize,Block,Stride,Shift,offsets)
hAbsVec(cdata)

#Copy first block to output vector
output_vector.extend(cdata)

#Now read in the other blocks and add them to the output_vector
for Block in range(1,64): 
    hReadFile(cdata,datareader_ptr,"CalFFT",Antenna,Blocksize,Block,Stride,Shift,offsets)
    hAbsVec(cdata) # Take absolute value of new block IN PLACE 
    output_vector += output_vector # Add it to output vector

hCloseFile(datareader_ptr)

#Convert Complex Vector to real (using abs) for plotting
hAbsComplexVec(output_vector,fdata)

#Make plot with mathgl 
gr=mglGraph(mglGraphZB,800,600) # Create mgl Graphobject
qw=hplot(gr,fdata) # Plot it into simple Qt Widget

