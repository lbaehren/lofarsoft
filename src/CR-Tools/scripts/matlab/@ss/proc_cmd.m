function [this, rsp] = proc_cmd(this, cmd )
% PROC_CMD method for processing subband selector commands
if cmd(1)==this.p.cmd_ss_config
  disp('cmd_ss_config');
  assert(cmd(5)==this.p.nof_beamlets*this.p.nof_polarizations-1,'incorrect nof_bands');
  assert(length(cmd)==5+this.p.nof_beamlets*this.p.nof_polarizations);
  for ib=0:this.p.nof_beamlets-1
    for ip=0:this.p.nof_polarizations-1
      this.sel(1+ib,1+ip) = cmd(6+ib*this.p.nof_polarizations+ip);
    end
  end
end
rsp=[];