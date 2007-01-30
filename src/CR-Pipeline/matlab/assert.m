function assert( tst, msg)
% displays an error message if the assertion fails
if ~ tst
    error(sprintf('Assertion failed: %s',msg));
end;    
