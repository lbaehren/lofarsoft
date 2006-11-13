function dbg_dump(filename, varargin)

fid = fopen(filename,'w');

for i = 1 : length(varargin)
    val = varargin{i};
    if (isnumeric(val))
        blitzprint(fid, val, 8);
    else
        fprintf(fid, val);
    end
end
fprintf(fid, '\n');
fclose(fid);

return;