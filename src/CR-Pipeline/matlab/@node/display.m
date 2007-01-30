function display(this)
% displays a processing node object

for t=this
  disp(sprintf('%s::%s',class(t),class(t.object)));
end

