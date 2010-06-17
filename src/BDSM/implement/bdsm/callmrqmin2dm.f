c! call mrqmin2dm when u have N gaussians in principle but want to solve for < N.

        subroutine callmrqmin2dm(n6,y,mask,sig,ndata,mdata,
     /        a,ia,ma,covar,alpha,nca,chisq,alamda,nn)
        implicit none
        integer n6,ma,nca,ndata,ia(ma),i,j,nn,mdata
        real*8 alamda,chisq,a(ma),alpha(nca,nca),covar(nca,nca)
        integer ria(n6)
        real*8 ra(n6),ralpha(n6,n6),rcovar(n6,n6),sig(ndata,mdata)
        real*8 y(ndata,mdata)
        integer mask(ndata,mdata)

        if (n6.gt.nca.or.n6.gt.ma) write (*,*) ' EERROORRRROORR'
        do i=1,n6
         ra(i)=a(i)
         ria(i)=ia(i)
         do j=1,n6
          rcovar(i,j)=covar(i,j)
          ralpha(i,j)=alpha(i,j)
         end do
        end do

334     call mrqmin2dm(y,mask,sig,ndata,mdata,ra,ria,n6,
     /        rcovar,ralpha,n6,chisq,alamda,nn)

        do i=1,n6
         a(i)=ra(i)
         ia(i)=ria(i)
         do j=1,n6
          covar(i,j)=rcovar(i,j)
          alpha(i,j)=ralpha(i,j)
         end do
        end do

        return
        end



