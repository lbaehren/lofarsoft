c! to test sigma clipping

        implicit none
        character filen*500,extn*10
        integer n,m,seed,nchar,i,nm,x,y
        parameter (n=100)
        real*8 sigma,rand,arr(100,100),std,av   
        real*4 arr4(100,100),vec(10000),low,up

        m=100
        nm=10000
        filen='image'
        sigma=100.d0
        call cr8noisemap(filen,n,m,sigma)
        extn='.img'
        call readarray_bin(n,m,arr,n,m,21,filen,extn)


        call getseed(seed)
        do 100 i=1,500
         call ran1(seed,rand)
         x=int(rand*(n-1)+1)
         call ran1(seed,rand)
         y=int(rand*(m-1)+1)
         call ran1(seed,rand)
         if (i.lt.250) arr(x,y)=sigma*(rand*(1000.0-5.0)+5.0)
         if (i.ge.250) arr(x,y)=-sigma*(rand*(1000.0-5.0)+5.0)
100     continue
        filen='image'
        call arrstat(arr,n,m,1,1,n,m,std,av)
        write (*,*) 'Before clipping ',av,std

        call sigclip(arr,n,m,1,1,n,m,std,av,5)
        write (*,*) 'After  clipping ',av,std
        
        return
        end


