function [ this ] = spec( p, t, n )
%SPEC Summary of this function goes here
%  Detailed explanation goes here
pwr=zeros(p.nof_slices,n);
figure;
h=image(pwr');
mymap=jet(200);
i=200:-1:1;
mymap=mymap(i,:);
colormap(mymap);
colorbar('vert');
title(sprintf('spec: power (dB) - after %s',t));
xlabel('time (slice)');
ylabel('beamlet');
drawnow;
this.h=h;
this.p=p;
this.slice=0;
this.pwr=pwr;
this=node(class(this, 'spec'));