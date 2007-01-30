function this = wg(p)
% constructor for waveform generator
this.p=p;
this.phase=0;
this.freq=0.1;
this.ampl=0.1;
this.enable=1;

% WG read from file:
% s = load('wg.dat');
% i = find(s(:,1) == 0);
% s = s(i,2);
% i = find(s >= 2^17);
% s(i) = -(2^18 - s(i));
% this.dat= s;

this=node(class(this, 'wg'));