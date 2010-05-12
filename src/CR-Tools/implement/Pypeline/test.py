np=4; nc=2
tcoeffs=Vector(float,nc,fill=0)
tcov=Vector(float,nc*nc)
tpowers=Vector(range(nc))
ty=Vector(float,np)
ty.fillrange(1.0,1.0)
x=Vector(float,np)
x.fillrange(0.0,1.0)
tx=Vector(float,np*nc)
tx.linearfitpolynomialx(x,tpowers)
tw=x.new()
tw.fill(1)
print "chi2=",tcoeffs.linearfit(tcov,tx,ty,tw)
res=x.new()
res.fill(0)
res.polynomial(x,tcoeffs,tpowers)
print "x=",x
print "tx=",tx
print "tw=",tw
print "tcoeffs=",tcoeffs
print "ty=",ty
print "res=",res

