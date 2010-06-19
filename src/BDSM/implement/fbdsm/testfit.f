c! to compare with scipy

        implicit none
        integer i,ia(2),n,seed
        parameter (n=30)
        real*8 x(30),y(30),err(30)
        real*8 alamda,chisq,covar(2,2),alpha(2,2),a(2),rand
        
        data ia/1,1/
        data a/0.1d0,1.0d0/
        call getseed(seed)

        do 100 i=1,n
         x(i)=i*1.d0
         y(i)=sqrt(0.024d0*0.024d0+2.3d0*2.3d0/x(i)/x(i))
         call ran1(seed,rand)
         y(i)=y(i)+rand*0.0001d0
         err(i)=1.d0
         write (*,'(f10.4,a,$)') y(i),' , '
100     continue
        write (*,*) 

        alamda=-1.d0
        do i=1,20
         call mrqmin(x,y,err,n,n,a,ia,2,covar,alpha,2,
     /       chisq,alamda,5)
         end do
        alamda=0.d0
        call mrqmin(x,y,err,n,n,a,ia,2,covar,alpha,2,
     /       chisq,alamda,5)

         write (*,*) a

        return
        end

