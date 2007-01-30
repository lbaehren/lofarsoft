function [ this, rsp] = proc_cmd(this, cmd)
% process commands

[this.object, rsp] = proc_cmd(this.object, cmd);

if 0
  disp( sprintf('  %s.proc_msg([%s])=[%s]', ...
        class(this.object), ...
        sprintf('%i ',size(cmd)), ...
        sprintf('%i ',size(rsp)) ));
end
