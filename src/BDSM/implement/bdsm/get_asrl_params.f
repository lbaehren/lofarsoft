c! goodness of association params

        subroutine get_asrl_params(nn,id1,id2,file1,file2,
     /     scratch,srldir,runcode,tol,spinflux,nf,bmmaj,bmmin,bmpa)
        implicit none
        character file1*500,file2*500,scratch*500,srldir*500,runcode*2
        character f1*500,spinflux*500
        integer nchar,nlines,nn,id1,id2,nf
        real*8 tol,bmmaj(nf),bmmin(nf),bmpa(nf)

        f1=file1(1:nchar(file1))//'__'//file2(1:nchar(file2))
        call getnlines_pasl(f1,scratch,nlines)
        if (mod(nlines,2).ne.0) then
         write (*,*) ' !!!!!  ERROR !!!'
        else
         call sub_get_asrl_params(nn,f1,nlines/2,scratch,id1,id2,nf,
     /        bmmaj,bmmin,bmpa)
        end if

        return
        end
c!
c!
c! format of op file is 
c! id1 id2 qtyid av std med avc stdc medc
        subroutine sub_get_asrl_params(nn,f1,npairs,scratch,id1,id2,nf,
     /             bmmaj,bmmin,bmpa)
        implicit none
        character f1*500,scratch*500,extn*20,fn*500
        integer npairs,nchar,nn,id1,id2,nf
        real*8 var(100,npairs)
        real*8 av,std,med,stdc,avc,medc
        real*8 bmmaj(nf),bmmin(nf),bmpa(nf)

        extn='.pasf'
        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        call readin_pasl(fn,f1,scratch,npairs,var,100)
        
        call call_vec_std(var,npairs,100,1,npairs,av,std,        ! RA
     /       med,avc,stdc,medc,29,1.d0,0.d0)
        write (nn,777) id1,id2,29,av,std,med,avc,stdc,medc
        call call_vec_std(var,npairs,100,1,npairs,av,std,        ! Dec
     /       med,avc,stdc,medc,30,1.d0,0.d0)
        write (nn,777) id1,id2,30,av,std,med,avc,stdc,medc
        call call_vec_std(var,npairs,100,1,npairs,av,std,        ! peak ratio
     /       med,avc,stdc,medc,14,1.d0,0.d0)
        write (nn,777) id1,id2,14,av,std,med,avc,stdc,medc
        call call_vec_std(var,npairs,100,1,npairs,av,std,        ! peak spin
     /       med,avc,stdc,medc,35,1.d0,0.d0)
        write (nn,777) id1,id2,35,av,std,med,avc,stdc,medc
        call call_vec_std(var,npairs,100,1,npairs,av,std,        ! tot spin
     /       med,avc,stdc,medc,36,1.d0,0.d0)
        write (nn,777) id1,id2,36,av,std,med,avc,stdc,medc

        if (id2.eq.nf) then
         call call_vec_std(var,npairs,100,1,npairs,av,std,        !  bmaj/beam
     /        med,avc,stdc,medc,4,1.d0/bmmaj(id1)/3600.d0,0.d0)
         write (nn,777) id1,id1,4,av,std,med,avc,stdc,medc
         call call_vec_std(var,npairs,100,1,npairs,av,std,        !  bmin/beam
     /        med,avc,stdc,medc,5,1.d0/bmmin(id1)/3600.d0,0.d0)
         write (nn,777) id1,id1,5,av,std,med,avc,stdc,medc
         call call_vec_std(var,npairs,100,1,npairs,av,std,        !  bpa-beam
     /        med,avc,stdc,medc,6,1.d0,-bmpa(id1))
         write (nn,777) id1,id1,6,av,std,med,avc,stdc,medc
         if (id1.eq.nf-1) then
          call call_vec_std(var,npairs,100,1,npairs,av,std,        !  bmaj/beam
     /        med,avc,stdc,medc,10,1.d0/bmmaj(id2)/3600.d0,0.d0)
          write (nn,777) id2,id2,4,av,std,med,avc,stdc,medc
          call call_vec_std(var,npairs,100,1,npairs,av,std,        !  bmin/beam
     /         med,avc,stdc,medc,11,1.d0/bmmin(id2)/3600.d0,0.d0)
          write (nn,777) id2,id2,5,av,std,med,avc,stdc,medc
          call call_vec_std(var,npairs,100,1,npairs,av,std,        !  bpa-beam
     /         med,avc,stdc,medc,12,1.d0,-bmpa(id2))
          write (nn,777) id2,id2,6,av,std,med,avc,stdc,medc
         end if
        end if

! put here correlation stuff for later.

777     format(i3,1x,i3,1x,i3,1x,6(1Pe11.4,1x))

        return
        end
c!
c!
c!  calc statistics for nn-th variable. clipped as well. 
        subroutine call_vec_std(var,npairs,nm,n1,n,av,std,med,avc,
     /             stdc,medc,nn,fac,add)
        implicit none
        integer nn,n1,n,nm,npairs,i
        real*8 var(nm,npairs),av,std,vec(npairs),med,nsig,avc,stdc
        real*8 medc,fac,add

        do i=1,npairs
         vec(i)=var(nn,i)*fac+add
        end do
        call vec_std(vec,npairs,n1,n,av,std)
        call calcmedian(vec,npairs,1,npairs,med)
        nsig=3.d0
        call sigclip1d(vec,npairs,1,n,stdc,avc,nsig)
        if (stdc.ne.stdc) then 
         stdc=0.d0
         medc=med
        else
         call calcmedianclip(vec,npairs,1,npairs,medc,nsig)
        end if

        return
        end






