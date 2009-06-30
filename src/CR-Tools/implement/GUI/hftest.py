

vf=FloatVec()
vf.append(2)
vf.append(3)

d=Data("LOPES")
d >> ("Data",vf)  >> ("SquarePUSH",_f("Square"),vf) >> ("PrintPUSH",_f("Print","Sys",TYPE.NUMBER))
d["Data"] << ("SquarePULL",_f("Square"),vf) << ("Square2PULL",_f("Square"),vf) << ("PrintPULL",_f("Print","Sys",TYPE.NUMBER))

d[1]=vf

vf=FloatVec()
vf.append(2)
vf.append(3)
d=Data("LOPES")
d[4].update()
vf.append(4)
d[1]=vf
d[6].update()
