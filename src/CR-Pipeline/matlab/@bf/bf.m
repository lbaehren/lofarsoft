function this = bf(p)
% BF beamformer constructor
p.nof_phasepol=p.nof_polarizations*p.nof_phases;
this.coeff=zeros(p.nof_antennas,p.nof_polarizations,p.nof_beamlets, p.nof_phases, ...
  p.nof_polarizations,p.nof_phases);  
for ip=1:p.nof_polarizations
    for iq=1:p.nof_phases
        this.coeff(:,ip,:,iq,ip,iq) = 1;
    end
end   
this.nxt_coeff=this.coeff;
this.p=p;
this=node(class(this, 'bf'));