c!  g77 testfdr.f conv2_get_wid_fac.f xcr8noisemap.f conv2.f writearray.f callfdr.f sub_prog.f readarray.f get_keyword.f check.f gasdev.f sort.f ran1.f conv2_get_gaus.f arrstat.f sigclip.f -o testfdr

c! plot alpha vs alpha-thalpa. 

        implicit none
        integer num,seed,i,j,nsrcpix,n,m
        include "constants.inc"
        real*8 gasdev,a(1000,1000),sig_c(3),fac,wid,s1,ca(1000,1000)
        real*8 cdelt(3),flux,x,y,rand,pcrit,sigcrit,alpha
        real*8 falsedet,totdet,thalpha
        real*8 stdclip,avclip
        character f1*500,bcon*10,bcode*4,f2*500,extn*20
        
        nsrcpix=4000

        num=1000

        seed=-54278
        bcon='periodic'
        bcode='bare'                       ! total is preserved
        sig_c(1)=2.d0
        sig_c(2)=2.d0
        sig_c(3)=0.d0
        cdelt(1)=1.0d0
        cdelt(2)=1.0d0
        cdelt(3)=0.d0

        f1='dum'
        f2='dum.norm'

        call conv2_get_wid_fac(sig_c,num,num,s1,fac,wid)
        call xcr8noisemap(num,num,num,num,1.d0/fac,a,seed)
        call conv2(a,num,num,num,num,sig_c,ca,bcon,bcode,fac,s1)
        do i=1,nsrcpix
         rand=gasdev(seed)
         flux=rand*10.d0+10.d0
         call ran1(seed,rand)
         x=rand*(num-1)+1
         call ran1(seed,rand)
         y=rand*(num-1)+1
         ca(int(x),int(y))=flux
        end do

        call writearray_bin2D(ca,num,num,num,num,f2,'mv')

        open(unit=28,file='filetestfdr',status='unknown')
        do j=1,1000,2
         write (*,*) j
         alpha=0.001d0*j
         call callfdr(f1,4.0d0,4.0d0,cdelt,num,num,
     /     '/data/niruj_pers/image/',0.d0,0.d0,num*num,alpha,
     /     pcrit,sigcrit)
         falsedet=num*num*0.5d0*erfc(sigcrit/sq2)
         totdet=falsedet+nsrcpix
         thalpha=falsedet/totdet
         write (28,*) alpha,pcrit,sigcrit,falsedet,totdet,thalpha
        end do
        close(28)

        end


