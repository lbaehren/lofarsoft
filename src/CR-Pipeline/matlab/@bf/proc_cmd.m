function [this, rsp] = proc_cmd(this, cmd )
% PROC_CMD beamformer command processor
if cmd(1) == this.p.cmd_bf_config 
  % antenna
  ia = cmd(5);
  % polarization
  ip = floor(cmd(6)/this.p.nof_phases);
  % phase
  iq = mod(cmd(6),this.p.nof_phases);
  
  % some basic checks
  assert(ia >=0 & ia<p.nof_antennas,     'antenna out of range');
  assert(ip >=0 & ip<p.nof_polarizations,'polarization out of range');
  assert(iq >=0 & iq<p.nof_phases,       'phase out of range');
  assert(length(cmd)==p.nof_beamlets*p.nof_polarizations*p.nof_phases*2+6,'invalid length');
  
  for ib=0:p.nof_beamlets-1
    for ip2=0:p.nof_polarizations-1
      for iq2=0:p.nof_phases-1
        index = (ib*p.nof_polarization+ip)*p.nof_phases+p.iq;
        this.nxt_coeff(ia+1,ip+1,ib+1,iq+1,ip2+1,iq2+1) = ...
          256*cmd(7+index)+cmd(8+index);
      end
    end
  end   
elseif cmd(1) == this.p.cmd_bf_enable
  disp('cmd_bf_enable');
  % swap the active coefficient set and the new set
  tmp = this.coeff;
  this.coeff=this.nxt_coeff;
  this.nxt_coeff=tmp;
end
rsp=[];