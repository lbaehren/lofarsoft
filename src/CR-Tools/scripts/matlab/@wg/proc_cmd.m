function [this, rsp] = proc_cmd(this, cmd )
% process waveform generator commands
if cmd(1)==this.p.cmd_wg_disable
    disp('cmd_wg_disable');
    this.enable=0;
elseif cmd(1)==this.p.cmd_wg_config
    disp('cmd_wg_config')
    config(enable,freq,ampl, 
    this.enable = 1;
    this.freq   = (cmd(5)+256*cmd(6)) / 65536;
    this.ampl   = (cmd(7)+256*cmd(8)) / 65536;
    disp(sprintf('  wg.freq  = %f',this.freq));
    disp(sprintf('  wg.ampl  = %f',this.ampl));
end
rsp=[];