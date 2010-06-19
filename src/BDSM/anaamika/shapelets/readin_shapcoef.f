
        subroutine readin_shapcoef(nn,maxnmax,nmax,nisl,iisl,coef)
        implicit none
        integer nn,maxnmax,nmax,n1,nisl,iisl
        real*8 coef(maxnmax,maxnmax,nisl)

        n1=nmax*(nmax-1)/2
        call sub_readin_shapcoef(nn,maxnmax,nmax,nisl,iisl,coef,n1)
        
        return
        end
c!
c!
c!
        subroutine sub_readin_shapcoef(nn,maxnmax,nmax,nisl,iisl,
     /             coef,n1)
        implicit none
        integer nn,maxnmax,nmax,n1,dumi,i,j,nisl,iisl
        real*8 coef(maxnmax,maxnmax,nisl),cfvec(n1)

        read (nn,*) dumi,cfvec 
        dumi=0
        do i=1,nmax
         do j=1,nmax-i
          dumi=dumi+1
          coef(i,j,iisl)=cfvec(dumi)
         end do
        end do

        return
        end


