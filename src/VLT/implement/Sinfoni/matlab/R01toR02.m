% Function ot compute r0 at a different wavelength
% r01 is the given r0 lambda1 is the wavelength correspondin to r01
% lambda2 is the wavelength at which r0 is required.
% function r02=R01toR02(r01,lambda1,lambda2);%

function r02=R01toR02(r01,lambda1,lambda2);

r02=r01.*(lambda2./lambda1).^(6/5);
