function [ this, out_dat] = proc_dat(this, in_dat)
% process data

[this.object, out_dat] = proc_dat(this.object, in_dat);

if 0
disp( sprintf('%s.proc_dat([%s])=[%s]', ...
    class(this.object), ...
    sprintf('%i ',size(in_dat )), ...
    sprintf('%i ',size(out_dat)) ));
end

if prod(size(out_dat)) > 0
    if isreal(out_dat)
      dat=reshape(out_dat,size(out_dat,1),prod(size(out_dat))/size(out_dat,1));
    else
      dat=reshape([real(out_dat);imag(out_dat)],size(out_dat,1),2*prod(size(out_dat))/size(out_dat,1));
    end

    for i=1:size(out_dat,1)
      if i>length(this.fid)
        this.fid(i)=fopen(sprintf('%s_%i.txt',class(this.object),i),'w');
      end
    fprintf(this.fid(i),'%i\n',round(dat(i,:)));
  end
end