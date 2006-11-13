% Convert all the U-files from .mat to .dat format

function u_mat2dat()

for i = 2:60
    for j = i:60
        mat_name = sprintf ('u%d_%d.mat',i,j);
        dat_name = sprintf ('u%02d_%02d.dat',i,j);
        eval (sprintf ('load %s', mat_name));
        fprintf('%s --> %s\n', mat_name, dat_name);
        eval (sprintf ('save -ascii %s %s', dat_name, 'u'));
        clear u;
    end
end
