c! reads the source list (*.gaul) headers
c! CHANGE GAUL FORMAT

        subroutine sourcelistheaders(f2,f1,n,m,nisl,ngau,gpi,nffmt,
     /             ffmt,srldir)
        implicit none
        integer n,m,nchar,ngau,nffmt,headint,dumi,nisl,isrc,gpi
        character f1*500,fn*500,extn*20,f2*500,ffmt*500,head*500
        character dumc*500,srldir*500
        real*8 dumr

Cf2py   intent(in) f2,srldir
Cf2py   intent(out) nisl,ngau,f1,n,m,gpi,ffmt,nffmt
        
        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'   
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
        call readhead_srclist(22,10,'Image_name',f1,dumi,dumr,'s')
        call readhead_srclist(22,12,'Image_size_x',dumc,n,dumr,'i')
        call readhead_srclist(22,12,'Image_size_y',dumc,m,dumr,'i')
        call readhead_srclist(22,17,'Number_of_islands',dumc,nisl,
     /       dumr,'i')
        call readhead_srclist(22,19,'Number_of_gaussians',dumc,ngau,
     /       dumr,'i')
        call readhead_srclist(22,24,'Max_gaussians_per_island',dumc,
     /       gpi,dumr,'i')
        call readhead_srclist(22,3,'fmt',dumc,nffmt,dumr,'i')
        backspace(22)
        read (22,*) head,headint,ffmt
        close(22)

        return
        end


