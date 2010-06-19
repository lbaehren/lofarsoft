        
        implicit none
        integer over,npairs,ptpbin,nbin,ptplastbin,i,round
        
        write (*,*) ' enter over'
        read (*,*) over
        npairs=300
        if (npairs.lt.2) write (*,*) '  Too few points !! '
        if (npairs.lt.100) then
         ptpbin=npairs/5
        else
         ptpbin=npairs/10
        end if
        if (npairs.gt.1000) ptpbin=npairs/20
        if (mod(ptpbin,2).eq.1) ptpbin=ptpbin+1  ! is even unless = total N
        if (npairs.lt.10) then
         ptpbin=npairs
        end if
        nbin=(npairs-ptpbin)/(ptpbin/over)+1         ! last bin is adjusted later
        ptplastbin=npairs-(1+(nbin-1)*ptpbin/over)+1 ! num pts in last bin
        nbin=nbin+1                     

        write (*,*) 'nbin ptpbin ptplastbin ',nbin,ptpbin,ptplastbin
        do i=1,nbin-1
         write (*,*) round(1+(i-1)*ptpbin*1.d0/over+(1-1)),
     /               round(1+(i-1)*ptpbin*1.d0/over+(ptpbin-1))
        end do

        end
