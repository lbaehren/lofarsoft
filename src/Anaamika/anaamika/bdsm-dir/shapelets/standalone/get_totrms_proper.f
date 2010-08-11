c! total and rms of full image with mask attached.

        subroutine get_totrms_proper(image,mask,x,y,n,m,fw,tot,arms)
        implicit none
        include "constants.inc"
        integer x,y,n,m,i,j,mask(x,y),marea
        real*8 image(x,y),tot,arms,dumr,av,fw(3)

        call arrstatmask(image,mask,n,m,1,1,n,m,dumr,av)
         write (*,*) 'done arrstatmask'
        call intmatsum(mask,n,m,n,m,marea)
         write (*,*) 'intmnatsum'
        tot=av*marea/(2.d0*pi*fw(1)*fw(2)/fwsig/fwsig)              ! this is total flux in image with mask
         write (*,*) 'av area tot ',av,marea,fw(1),fw(2),tot
        call sigclip(image,n,m,1,1,n,m,arms,dumr,3)
        
        return  
        end


