function this = ss(p)
% SS constructor for subband selection node
%   SS(p) instantiates a processing node for subband selection, 
%   with the parameters in the structure p.

this.p=p;
for ib=0:p.nof_beamlets-1
  for ip=0:p.nof_polarizations-1
      this.sel(1+ib,1+ip) = ib*p.nof_polarizations+ip;
  end
end
this=node(class(this, 'ss'));