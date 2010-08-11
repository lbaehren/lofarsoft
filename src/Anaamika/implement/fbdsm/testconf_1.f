c! this is correct.

        implicit none
        real*8 k1,freq,dr1,dr2,smin,smax,gam,g1,rand,flux
        real*8 low(8),hi(8)
        integer seed,i,nsrc,j,num(8),k

        data low/1.d-8,1.d-7,1.d-6,1.d-5,1.d-4,1.d-3,1.d-2,1.d-1/
        data hi/1.d-7,1.d-6,1.d-5,1.d-4,1.d-3,1.d-2,1.d-1,1.d0/
        k1=277.d0
        gam=2.17d0
        smin=1.d-8
        smax=1.d0
        freq=800.d0

        k1=k1*((freq/1400.d0)**(-0.7d0))
        g1=1.d0-gam
        dr1=k1/g1*(smin**g1)
        dr2=k1/g1*(smax**g1)
        do i=1,8
         num(i)=0
        end do
         
        call getseed(seed)
        nsrc=77830991
        do i=1,nsrc
         do j=1,1
          call ran1(seed,rand)
          flux=((dr1-rand*(dr1-dr2))*g1/k1)**(1.d0/g1)
          do k=1,8
           if (flux.ge.low(k).and.flux.le.hi(k)) num(k)=num(k)+1
          end do
         end do
        end do

        write (*,*) num

        end
