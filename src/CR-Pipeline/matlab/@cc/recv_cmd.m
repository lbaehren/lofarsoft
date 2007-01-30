function [this,cmd,nxt_slice]=recv_cmd(this,slice)
% receive command and control commands
  cmd=cell(0);
  i=1;
  n=1;
  while(slice==this.nxt_slice & ~feof(this.f))
    s=fgets(this.f);
    if s==-1
      continue;
    end
    if strncmp(s,'%SLICE ',7) 
      this.nxt_slice=sscanf(s(8:length(s)),'%i');
    elseif strncmp(s,'%GAP',4)
      i=1;
      n=n+1;
    elseif ~strncmp(s,'#',1)
      cmd{n}(i)=sscanf(s,'%i');
      i=i+1;
    end
  end
  nxt_slice=this.nxt_slice;