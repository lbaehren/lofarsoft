c! reads the source list (*.gaul) headers
c! CHANGE SRL FORMAT

        subroutine srllistheaders(f2,n,m,nisl,ngau,nsrc,gpi,srldir)
        implicit none
        integer n,m,nchar,nsrc,nffmt,headint,dumi,nisl,isrc,gpi,ngau
        character f1*500,fn*500,extn*20,f2*500,ffmt*500,head*500
        character dumc*500,srldir*500
        real*8 dumr
        
        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl'   
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
        call readhead_srclist(22,12,'Image_size_x',dumc,n,dumr,'i')
        call readhead_srclist(22,12,'Image_size_y',dumc,m,dumr,'i')
        call readhead_srclist(22,17,'Number_of_islands',dumc,nisl,
     /       dumr,'i')
        call readhead_srclist(22,17,'Number_of_sources',dumc,nsrc,
     /       dumr,'i')
        call readhead_srclist(22,19,'Number_of_gaussians',dumc,ngau,
     /       dumr,'i')
        call readhead_srclist(22,24,'Max_gaussians_per_island',dumc,
     /       gpi,dumr,'i')
        backspace(22)
        read (22,*) head,headint,ffmt
        close(22)

        return
        end

