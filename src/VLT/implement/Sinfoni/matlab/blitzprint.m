% function to write a variable to file, exactly the way Blitz outputs
% a multi-dimensional array, using a fixed (predefined) precision.
function blitzprint(fid, val, prec)

if (nargin < 2)
    return
end
if (nargin < 3)
    prec = 6;
end

sz = size(val);
dims = ndims(val);

% Output format specifier for printing data. Complex numbers are
% printed the C++ way, e.g. (0.3,-3.4). Each number is printed 
% with a minimal width of 9.
if (isreal(val))
    fmt = sprintf('%%.%dg', prec);
else
    fmt = sprintf('(%%.%dg,%%.%dg)', prec, prec);
end

% If val is a scalar, just print its value and return.
if (length(val) == 1)
    if (isreal(val))
        fprintf(fid, '%9s', sprintf(fmt, val));
    else
        fprintf(fid, '%9s', sprintf(fmt, real(val), imag(val)));
    end
    return;
end

% If val is a vector, print its length, else print all dimensions.
if (min(sz) == 1)
    fprintf(fid, '%d\n ', max(sz));
else
    for i=1:dims
        fprintf(fid, '%d', sz(i));
        if (i ~= dims)
            fprintf(fid, ' x ');
        end
    end
    fprintf(fid, '\n');
end
    
% If val is a vector or an N-D array, convert it to a row vector.
if (min(sz) == 1 | ndims(val) > 2)
    val = val(:)';
    sz = size(val);
end

fprintf(fid, '[ ');
for i=1:sz(1)
    for j=1:sz(2)
        if (isreal(val))
            fprintf(fid, '%9s ', sprintf(fmt, val(i,j)) );
        else
            fprintf(fid, '%9s ', sprintf(fmt, real(val(i,j)), imag(val(i,j))) );
        end
        if (mod (j,7) == 0) 
            fprintf(fid, '\n  ');
        end
    end
    if (i ~= sz(1))
        fprintf(fid, '\n  ');
    end
end
if (sz(1) == 1)
    fprintf(fid, ' ]');
else
    fprintf(fid, ']\n');
end
