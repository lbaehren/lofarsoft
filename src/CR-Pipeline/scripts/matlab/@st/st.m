function this = st(p, n)
% constructor for statistics
this.p=p;
this.pwr=zeros(p.nof_antennas, p.nof_polarizations, n);
this.count=0;
this=node(class(this, 'st'));