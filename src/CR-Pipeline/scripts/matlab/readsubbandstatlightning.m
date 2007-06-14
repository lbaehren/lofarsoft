%Used for plotting the subband statistics
clear datafiles;
clear all; close all;
norm_albert=0; % norm_albert voor norm zoals Albert, 0 voor norm als Stefan.
andre_norm=1;

cs='cs1';
%cs='cs8';
%cs='cs10';
%cs='cs16';
dirname = ['d:\data',cs,'\20070518\']

fs          = 160e6;      % Sample rate [MHz]
intperiod   = 10;         % interval between snapshots [sec]
nyquistzone = 1;          % Nyquist zone 1=0-100MHz,2=100-200MHz,3=300-400MHz
nch         = 512;        % # subbands for CEP
npoints     = 21000*nch;  % points to plot

%subbandoffset=255; %starting subband number
freqnop=512;
xas=1:1:512;
xas=(xas'/512)*(fs/2);
xas=xas-((fs/2)/(512));
xas=xas+(fs/2*(nyquistzone-1));

nsubbands=2*nch;
scalefactor=1*31.76*2^11; %DC-block * prefilter*bits 7.94  (FTS-20) 31.76 (FTS-10);
adcbits=12;

plot_los = 0;
plot_single=1;
plot_allcorr=1; %set to 0 for plotting only one correlation product for all measurements
                %set to 1 for plotting all correlation products for one
                %measurement

lsout = dir([dirname '*sst*.dat']);
[maxx dummy] = size(lsout);
nfiles = maxx;
maxnr=nfiles;

for cnt=1:nfiles
    datafiles(cnt,:) = lsout(cnt).name;
    disp(['file nr ' int2str(cnt) ' file name: ' datafiles(cnt,:)]);
end
offset = length(dirname);

meet_nr =	input('Measurement number [1]:');
if isempty(meet_nr)
   meet_nr=1;
end
  fid = fopen([dirname '\' datafiles(meet_nr,:)], 'r');
  data = fread(fid, [npoints 1],'double');
  fclose(fid);
  %redata = data(1:2:end);
  duration=length(data)/nch;
  time=1:duration;
  %imdata = data(2:2:end);
  %acmeet = reshape(redata, [nch,duration]) + i * reshape(imdata, [nch,duration]);
  acmeet = reshape(data, [nch,duration]);
  
  scaling=(scalefactor^2)*(fs/nsubbands)*intperiod;
  acmeet=acmeet/scaling; %acmeet is scaled now to ADC levels

  if ~plot_single
      figure(100);
      pcolor(time,xas,abs(acmeet));
      shading interp;
      caxis([5e-10 5e-9])
      colorbar;
      ylabel('Frequency');
      xlabel('Meas. number'); 
      title(['Power of ',dirname,'\' ,datafiles(meet_nr,:)]);
      str2=['print -djpeg rcu',num2str(meet_nr-1),'_tf'];
      eval(str2);

  end  
     
  number=19268;
  number2=19269;
  number3=19270;
  number4=19271;
  
      figure(3);
      plot(xas/1e6,10*log10(abs(acmeet(:,number))),'k');
      hold on;
      plot(xas/1e6,10*log10(abs(acmeet(:,number2))),'b');
      plot(xas/1e6,10*log10(abs(acmeet(:,number3))),'r');
      plot(xas/1e6,10*log10(abs(acmeet(:,number4))),'g');
      xlabel('Frequency (MHz)');
      ylabel('Power (dB)'); 
      %title([dirname datafiles(meet_nr,:),' time=',num2str(number)]);
      title([dirname datafiles(meet_nr,:)]);
      legend(num2str(number),num2str(number2),num2str(number3),num2str(number4));
      grid on;
      %ylim([63 65]);
      str2=['print -djpeg rcu',num2str(meet_nr-1),'_f'];
      eval(str2);
  
   
  sbb = 384;
  %freq = 60E6;
  %sbb = round(freq/(fs/1024));
  figure(5);
  plot(10*log10(abs(acmeet(sbb-32,:))),'b');
  hold on;
  plot(10*log10(abs(acmeet(sbb-16,:)))-1.5,'y');
  plot(10*log10(abs(acmeet(sbb,:))),'r');
  xlabel('Measurment Nr');
  ylabel('Power (dB)'); 
  legend('55 MHz (sbb=352)','57.5 MHz (sbb=366)','60 MHz (sbb=384)');
  title([dirname datafiles(meet_nr,:)]);
  grid on;
  %ylim([63 65]);
  str2=['print -djpeg rcu',num2str(meet_nr-1),'_f'];
  %eval(str2);
  
  
