% rsp board simulation model

clear all;
clear classes;
close all;

% generics
p.nof_antennas      = 1;
p.nof_subbands      = 512;
p.nof_beamlets      = 128;
p.nof_phases        = 2;
p.nof_polarizations = 2;
p.fft_size          = p.nof_phases*p.nof_subbands;
p.pf1_ipol          = p.nof_subbands;
p.pf1_nof_taps      = 16;
p.pf1_coeff_w       = 0;
p.pf1_coeff_file    = 'pf1_coeff_fft%d_taps%d_ipol%d_width%d.txt';
p.cc_file           = 'cc.dat';
p.st_nof_samples    = 32;
p.st_nof_moments    = 2;
p.st_in_data_w      = 18;
p.st_wm2_data_w     = 48;
p.st_m2_data_w      = 32;
p.nof_samples       = p.fft_size;

p.nof_slices        = 64;

p.cmd_ss_config     = 1;
p.cmd_wg_config     = 2;
p.cmd_wg_disable    = 3;
p.cmd_bf_config     = 4;
p.cmd_bf_enable     = 5;

i_cc=cc(p);

% instantiate processing chain
chain=[wg(p), pf1(p), pft(p), st(p, p.nof_subbands), spec(p,'pft',p.nof_subbands), ss(p), bf(p), st(p,p.nof_beamlets) ];

dat=[];

t_start=cputime;
eof=0;
slice=0;
nxt_slice=0;


for slice=1:p.nof_slices
  disp(sprintf('slice=%i',slice));
  % process data
  for n=1:length(chain)
    [chain(n), dat] = proc_dat(chain(n), dat);
  end
  % process commands
  % [i_cc,cmd,nxt_slice]=recv_cmd(i_cc,slice);  
  %for i=1:length(cmd)
  %  for n=1:length(chain)
     % process control messages
     % [chain(n), rsp] = proc_cmd(chain(n), cmd{i});
     %send control responses
     %[i_cc    , rsp] = send_rsp(i_cc    , rsp);
  %  end
  %end
end
t_stop=cputime;
disp(sprintf('total processing time: %f seconds',t_stop-t_start));
fclose('all');
