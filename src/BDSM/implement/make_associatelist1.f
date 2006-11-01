c! associate list type 1. master/panthi and slave/kothi lists.  
c! take each panthi, find all attached kothis, then append kothis without panthis.
c! then connect kothis with multiple panthis. 
c! format is first double deckers and then single kothis. see notebook for format.
        
        subroutine make_associatelist1(nsrcm,ram,decm,nsrcs,
     /     ras,decs,tol,scrat,master,sec,dsum,scratch)
        implicit none
        include "constants.inc"
        integer nsrcm,nsrcs,nchar,i,j
        character scrat*500,str1*1,fn*500,fmti*2,ffmt1*100,ffmt2*100
        character master*(*),sec*(*),scratch*500
        real*8 ram(nsrcm),decm(nsrcm),ras(nsrcs),decs(nsrcs),tol,dist
        integer n_assocsrc(nsrcm),assocsrc_m2s(nsrcm,nsrcs/10)  
        integer assocsrc_s2m(nsrcs,nsrcm/10),i1
        integer flag_s2m(nsrcs),dsum(5)

        call initialise_int_vec(flag_s2m,nsrcs,0) 
        do 100 i=1,nsrcm
         n_assocsrc(i)=0
         do 110 j=1,nsrcs
          if (abs(decm(i)-decs(j))*3600.d0.le.tol*5.d0) then  
           call justdist(ram(i),ras(j),decm(i),decs(j),dist)
           if (dist.le.tol) then
            n_assocsrc(i)=n_assocsrc(i)+1     ! number of kothis for panthi i
            assocsrc_m2s(i,n_assocsrc(i))=j   ! the associated kothis for panthi i
            flag_s2m(j)=flag_s2m(j)+1         ! number of panthis for kothi j
            assocsrc_s2m(j,flag_s2m(j))=i     ! panthis with kothi j
           end if
          end if
110      continue
100     continue

        call get_fmt_int(max(nsrcm,nsrcs),fmti)
        ffmt1="(1x,"//fmti//",1x,a1,1x,"//fmti//",$)"
        ffmt2="(1x,"//fmti//",$)"
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//'.asrl'
        write (*,*) '  Writing out '//fn(1:nchar(fn))
        write (*,*) 
        open(unit=24,file=fn(1:nchar(fn)),status='unknown')
        write (24,*) 99999999
        do i=1,nsrcm
         write (24,*) i,n_assocsrc(i)   ! panthi id, # of associated kothis
         do j=1,n_assocsrc(i)           ! for each associated kothi,
          if (flag_s2m(assocsrc_m2s(i,j)).eq.1) then 
           str1='s'   ! monogamous kothi
           write (24,*) assocsrc_m2s(i,j),' ',str1
          else
           str1='m'   ! kothi has multiple panthis
           write (24,ffmt1) assocsrc_m2s(i,j),str1,
     /       flag_s2m(assocsrc_m2s(i,j))
           do i1=1,flag_s2m(assocsrc_m2s(i,j))      ! write each panthi id
            write (24,ffmt2) assocsrc_s2m(assocsrc_m2s(i,j),i1)
           end do
           write (24,*)
          end if
         end do
        end do
        do j=1,nsrcs           ! for single kothis
         if (flag_s2m(j).eq.0) write (24,*) ' 0 ',j
        end do
        write (24,*) 99999999
        close(24)

        call vec_int_sum(n_assocsrc,nsrcm,nsrcm,dsum(1))
        call vec_int_nzero(n_assocsrc,nsrcm,nsrcm,dsum(2))
        call vec_int_nzero(flag_s2m,nsrcs,nsrcs,dsum(3))
        call vec_int_num_range(n_assocsrc,nsrcm,nsrcm,2,99999,dsum(4))
        call vec_int_num_range(flag_s2m,nsrcs,nsrcs,2,99999,dsum(5))

        write (*,*) '  Found ',dsum(1),' associations with ',
     /       master(1:nchar(master))
        write (*,*)  '  Sources in ',master(1:nchar(master)),
     /       ' with no associations is ',dsum(2)
        write (*,*)  '  Sources in ',sec(1:nchar(sec)),
     /       ' with no associations is ',dsum(3)
        write (*,*)  '  Sources in ',master(1:nchar(master)),
     /       ' with multiple associations is ',dsum(4)
        write (*,*)  '  Sources in ',sec(1:nchar(sec)),
     /       ' with multiple associations is ',dsum(5)

        return
        end
