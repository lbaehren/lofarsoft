c! for ks stat on gaussian ... cdf for mean 0 and std 1.
        
        function ks_gaus(val)
        implicit none
        real*8 val,ks_gaus,erfc

        ks_gaus=1.d0-0.5d0*erfc(val)
        
        return
        end


