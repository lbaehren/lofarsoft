function this = cc(p)
% constructor for command and control
this.p=p;
this.f=fopen(p.cc_file,'r');
this.nxt_slice=0;
this=class(this, 'cc');
