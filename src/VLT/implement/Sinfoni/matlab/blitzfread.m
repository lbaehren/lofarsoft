function val = blitzfread(fid)
%BLITZFREAD Read Blitz array.
%   VAL=BLITZREAD(FID) reads data from file that was written in 
%   Blitz++ format, into VAL.
%
%   FID must be a valid file descriptor, obtained using FOPEN().
%
%   See also BLITZFWRITE.

if (nargin < 1)
    error('Not enough input arguments.');
    return
end

% Read the first line from file.
s = fgetl(fid);

% If the string contains a decimal point, we will assume that the
% array dimension is zero, i.e. the file contains only one scalar.
if (findstr(s,'.'))
    val = str2double(s);
    return
end

% Read the dimensions from the file, which are in the format %d x %d ...
dim = strread(s, '%d x');

% Read the next line. It must start with '[', otherwise it is an error.
s = fgetl(fid);
s = s(:,findstr(s, '[')+1:end);
if (isempty(s))
    error('Format error: expected ''[''');
    return
end

% Now we should read the values line by line, and add them to val,
% until a line end with ']', which signals the end of the array.
val = [];
while (isempty(findstr(s, ']')) & size(val) < prod(dim))
    val = vertcat(val, strread(s, '%f'));
    s = fgetl(fid);
    if (s == -1)
        error('Premature end of file');
    end
end
s = s(:,1:findstr(s,']')-1);
val = vertcat(val, strread(s, '%f'));
if (size(val) ~= prod(dim))
    error('Wrong number of elements');
end

% Now we must reshape val to match the given dimensions.
% Note that reshape uses Fortran-style column-major order, so
% we must permute the indices to obtain C-style row-major order.
val = reshape(val, dim');
val = permute(val, size(dim,1):-1:1);
