c! write gsm_table_solnname and gsm_calcf_solnname and gsm_posn_solnname

        subroutine write_orgsm(spin_arr,espin_arr,sumngaul,np,nf,
     /             maxngaul,nassoc,tot,dtot,freq,n1,n2,n3,assoc_arr,
     /             avsp00,stdsp00,medsp00,tofit,calcflux0,calcflux1,
     /             ra,dec,era,edec,avra,eavra,avdec,eavdec)
        implicit none
        integer sumngaul,np,n1,n2,n3,nf,maxngaul,i,nchar,nassoc,inf,k
        real*8 freq(nf),tot(nf,maxngaul),spin_arr(sumngaul,np)
        real*8 dtot(nf,maxngaul),espin_arr(sumngaul,np),totfl
        real*8 ra(nf,maxngaul),dec(nf,maxngaul),ra1,dec1
        real*8 era(nf,maxngaul),edec(nf,maxngaul),era1,edec1
        integer assoc_arr(sumngaul,nf),bnchar,round,tofit(nf)
        character str1*500,str2*500,str3*500,str4*500,str5*500
        real*8 avsp00,stdsp00,medsp00,calcfl
        real*8 calcflux0(nf,maxngaul),calcflux1(nf,maxngaul)
        integer g_order,b_order,f_ord_g(nf),f_ord_b(nf)
        integer code2,sumcode
        real*8 avra(sumngaul),avdec(sumngaul)
        real*8 eavra(sumngaul),eavdec(sumngaul)

c! write headers in gsm_table_solnname and gsm_calcf_solnname and gsm_posns_solnname 
        do i=n1,n3,n3-n1
         write (i,*) 'Mean_spin00 = ',avsp00
         write (i,*) 'Std_spin00 = ',stdsp00
         write (i,*) 'Median = ',medsp00
        end do
        str1='#assoc'
        str5='#assoc'
        str2='   .'
        str4='   .'
        do i=1,nf
         write (str3,*) round(freq(i)/1.d6)
         if (str3(nchar(str3):nchar(str3)).eq.'.') 
     /       str3(nchar(str3):nchar(str3))=' '
         str1=str1(1:nchar(str1))//'     S_'//
     /        str3(bnchar(str3):nchar(str3))
         str5=str5(1:nchar(str5))//'                        S_'//
     /        str3(bnchar(str3):nchar(str3))
         str2=str2(1:nchar(str2))//'        Jy'
         str4=str4(1:nchar(str4))//'       RA         eRA        '//
     /        'Dec       eDec '
        end do
        write (n1,'(a,3x,$)') str1(1:nchar(str1))
        write (n3,'(a,3x)') str5(1:nchar(str5))
        write (n1,'(4x,16(a11))') 
     /   '    I00    ','    eI00   ',
     /   '   spin00  ','  espin00  ','     I11   ',
     /   '    eI11   ','   spin10  ','   espin10 ',
     /   '   spin11  ','   espin11 ','     R.A.  ',
     /   '    eR.A.  ','     Dec   ','    eDec   ',
     /   '    code   '
        write (n1,*) str2(1:nchar(str2))
        write (n3,*) str4(1:nchar(str4))

c! write numbers in gsm_table_solnname
        do i=1,nassoc
         write (n1,'(i7,a2,$)') i,'  '
         write (n3,'(i7,a2,$)') i,'  '
         do inf=1,nf
          if (assoc_arr(i,inf).eq.0) then
           totfl=-999.d0
           ra1=999.99d0
           dec1=99.99d0
           era1=999.99d0
           edec1=99.99d0
          else
           totfl=tot(inf,assoc_arr(i,inf))
           ra1=ra(inf,assoc_arr(i,inf))
           dec1=dec(inf,assoc_arr(i,inf))
           era1=era(inf,assoc_arr(i,inf))
           edec1=edec(inf,assoc_arr(i,inf))
          end if
          write (n1,'(1Pe10.3,a1,$)') totfl,' '
          write (n3,'(4(0Pf10.6,a1),$)') 
     /           ra1,' ',era1,' ',dec1,' ',edec1,' '
         end do
         do k=1,5
          write (n1,'(1x,1Pe10.3,1x,1Pe10.3,$)') 
     /     spin_arr(i,k),espin_arr(i,k)
         end do
         write (n1,'(4(1x,0Pf10.6),$)') 
     /          avra(i),eavra(i),avdec(i),eavdec(i)
         code2=0
         sumcode=0
         do inf=1,nf
          if (assoc_arr(i,inf).ne.0) then
           code2=code2+(10**(nf-inf))
           sumcode=sumcode+(10**nf)
          end if
         end do
         code2=sumcode+code2
         write (n1,'(2x,i6,$)') code2
         write (n1,*)
         write (n3,*)
        end do

c! define fitted and not fitted files
        g_order=0  ! index for files fitted
        b_order=0  ! index for files not fitted
        do i=1,nf
         if (tofit(i).eq.1) then
          g_order=g_order+1
          f_ord_g(g_order)=i
         end if
         if (tofit(i).eq.0) then
          b_order=b_order+1
          f_ord_b(b_order)=i
         end if
        end do

c! write headers in gsm_calcf_solnname
        str1='#assoc'
        str2='   .'
        do i=1,g_order
         write (str3,*) round(freq(f_ord_g(i))/1.d6)
         if (str3(nchar(str3):nchar(str3)).eq.'.') 
     /       str3(nchar(str3):nchar(str3))=' '
         str1=str1(1:nchar(str1))//'     S_'//
     /        str3(bnchar(str3):nchar(str3))
         str2=str2(1:nchar(str2))//'        Jy'
        end do
        do i=1,b_order
         write (str3,*) round(freq(f_ord_b(i))/1.d6)
         if (str3(nchar(str3):nchar(str3)).eq.'.') 
     /       str3(nchar(str3):nchar(str3))=' '
         str1=str1(1:nchar(str1))//'      S_'//
     /        str3(bnchar(str3):nchar(str3))//
     /        '      S_'//
     /        str3(bnchar(str3):nchar(str3))
         str2=str2(1:nchar(str2))//'        obs' 
         str2=str2(1:nchar(str2))//'       calc' 
        end do
        write (n2,'(a,3x)') str1(1:nchar(str1))
        write (n2,*) str2(1:nchar(str2))

c! write numbers in gsm_calcf_solnname
        do i=1,nassoc
         write (n2,'(i7,a2,$)') i,'  '
         do inf=1,g_order
          if (assoc_arr(i,f_ord_g(inf)).eq.0) then
           totfl=-999.d0
          else
           totfl=tot(f_ord_g(inf),assoc_arr(i,f_ord_g(inf)))
          end if
          write (n2,'(1Pe10.3,a1,$)') totfl,' '
         end do
         do inf=1,b_order
          if (assoc_arr(i,f_ord_b(inf)).eq.0) then
           totfl=-999.d0
           calcfl=-999.d0
          else
           totfl=tot(f_ord_b(inf),assoc_arr(i,f_ord_b(inf)))
           calcfl=calcflux0(f_ord_b(inf),assoc_arr(i,f_ord_b(inf)))
          end if
          write (n2,'(1Pe10.3,a1,1Pe10.3,a1,$)') totfl,' ',calcfl,' '
         end do
         write (n2,*)
        end do

        return
        end

