c calculates the psf for each tile

        subroutine get_psf_tile()
        implicit none

        do itile=1,ngensmod
         either calc size of calibrator
         or of all primary calibs or
         of all sec + primary calibs
        end do

        get errors on psf shape 

        fit fn to gaussian shape

        determine if psf varies within errors

        fit shapelets to all these
        get errors on shapelet coeff

        fit fn to coeffs
        
        determine if statistically significant variation

        check behaviour of extrapolated coeff at map edges
        


        return
        end


