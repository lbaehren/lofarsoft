filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
datafile=crfile(filename)
datafile["blocksize"]=2**14
fx=datafile["Fx"]
fft=hArray(complex,dimensions=fx)
fft.convert(fx)

