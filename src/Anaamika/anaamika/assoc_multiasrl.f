c! simple version now with C and S only.
c! assoc_arr(i,j) has the gaul id of the jth gaul file of the ith association. 0 => no gaus
c! for list j.
c! assoc_ind(i,j) has the row number in assoc_arr for the jth gaul list for the ith gaus
c! for list j, ie, is the association number.
c!
c! modified assoc_multi_asrl since that only checks for the first association
c! and not the closest one. Also doesnt check if all prev ones were associated
c! or not. e.g. 1-2, 1-3 is done and for 2-3, shud check if 3 also has been
c! associated or not. hence now take a pair of lists, construct the
c! associations and then update the assoc arrays.

c! now modify so that when we check NN of all gaul in list 1, we then
c! store and go back and see if there are duplicates; that is, if
c! NN(a1) = 1 and NN(a2) = 1 then check whether a1 or a2 is closer and
c! update the NN list. Then go through it and assign associations.
 
c! You still will have problems about NN(a1) = a2; NN(a1) = a3 and NN(a2) = b3
c! How to solve that ?
        
        subroutine assoc_multiasrl(nf,maxngaul,sumngaul,ngaul,code,
     /     flag,xpix,ypix,pbmaj,pbmin,pbpa,ra,dec,assoc_arr,assoc_ind,
     /     nassoc,bmaj,bmin,bpa,n1,n2,n3)
        implicit none
        integer sumngaul,maxngaul,nf,inf,nassoc,i,j,inf1,inf2,ig1,ig2
        integer assoc_arr(sumngaul,nf),assoc_ind(maxngaul,nf),n1,n2,n3
        integer flag(nf,maxngaul),dumi,ngaul(nf),minind(maxngaul)
        integer a_ind(20),n_occ,chosenone
        real*8 xpix(nf,maxngaul),ypix(nf,maxngaul),pbmaj(nf),bsize
        real*8 bmaj(nf,maxngaul),bmin(nf,maxngaul),bpa(nf,maxngaul)
        real*8 pbmin(nf),pbpa(nf),ra(nf,maxngaul),fwhm1,fwhm2,tol
        real*8 dec(nf,maxngaul),dist,sizerat,mindist(maxngaul),tol1
        real*8 dist_occ
        character code(nf,maxngaul)*4

        dumi=0
        call initialisemask(assoc_arr,sumngaul,nf,sumngaul,nf,dumi)
        call initialisemask(assoc_ind,maxngaul,nf,maxngaul,nf,dumi)

        nassoc=0
        do inf1=1,nf                 ! each file
         do inf2=inf1+1,nf             ! with other files
          bsize=max(pbmaj(inf1),pbmaj(inf2))*3600.d0 ! for MSSS
          tol=bsize*1.0d0 ! for MSSS
          tol1=bsize*10.d0
          do ig1=1,ngaul(inf1)
           if (mod(ig1,1000).eq.0) write (*,*) inf1, inf2, ig1
           mindist(ig1)=9.d9
           minind(ig1)=0
           do ig2=1,ngaul(inf2)    ! take each pair
            if (abs(dec(inf1,ig1)-dec(inf2,ig2))*3600.d0.le.tol1) then! for MSSS
c             if ((code(inf1,ig1).eq.'S'.or.code(inf1,ig1).eq.'C').and.  !  comment for MSSS 
c     /          (code(inf2,ig2).eq.'S'.or.code(inf2,ig2).eq.'C').and.
c     /          (flag(inf1,ig1).eq.0.and.flag(inf2,ig2).eq.0)) then

              call justdist(ra(inf1,ig1),ra(inf2,ig2),dec(inf1,ig1),
     /             dec(inf2,ig2),dist)
              if (dist.le.tol) then
               if (dist.lt.mindist(ig1)) then
                mindist(ig1)=dist
                minind(ig1)=ig2
               end if
              end if
c             end if  ! if code=S/C   ! for MSSS
            end if  ! d1-d2 < tol1 
           end do   ! loop over ig2
          end do    ! loop over ig1

          do ig2=1,ngaul(inf2)   ! faster to check for inf2 (also assuming #inf2 < #inf1)
           if (mod(ig2,1000).eq.0) write (*,*) '         ',ig2
           n_occ=0
           call initialise_int_vec(a_ind,20,0)
           do ig1=1,ngaul(inf1) 
            if (minind(ig1).eq.ig2) then
             n_occ=n_occ+1
             a_ind(n_occ)=ig1
             if (mindist(ig1).lt.dist_occ) dist_occ=mindist(ig1)
            end if
           end do
           if (n_occ.gt.1) then
            dist_occ=9.d9
            do i=1,n_occ
             if (dist_occ.gt.mindist(a_ind(i))) then
              chosenone=i
              dist_occ=mindist(a_ind(i))
             end if
            end do
            do i=1,n_occ
             if (i.ne.chosenone) then
              minind(a_ind(i))=0
              mindist(a_ind(i))=9.d9
             end if
            end do
           end if
          end do

          do ig1=1,ngaul(inf1) 
           if (minind(ig1).ne.0) then
            if (assoc_ind(ig1,inf1).ne.0) then       ! check if ig1 already in list 
             assoc_arr(assoc_ind(ig1,inf1),inf2)=minind(ig1)
             assoc_ind(minind(ig1),inf2)=assoc_ind(ig1,inf1)
            else 
             nassoc=nassoc+1
             assoc_arr(nassoc,inf1)=ig1 
             assoc_arr(nassoc,inf2)=minind(ig1)
             assoc_ind(ig1,inf1)=nassoc
             assoc_ind(minind(ig1),inf2)=nassoc
            end if
           end if  ! minind ne 0
          end do  ! ig1

         end do  ! inf2
        end do  ! inf1


c! now put all singletons in
        do inf1=1,nf                 ! each file
         do ig1=1,ngaul(inf1)
          if ((code(inf1,ig1).eq.'S'.or.code(inf1,ig1).eq.'C').and.
     /        flag(inf1,ig1).eq.0) then
           if (assoc_ind(ig1,inf1).eq.0) then
            nassoc=nassoc+1
            assoc_arr(nassoc,inf1)=ig1
            assoc_ind(ig1,inf1)=nassoc
           end if
          end if
         end do
        end do
        
        do i=n1,n3
         write (i,*) '  Number_of_associations = ',nassoc
        end do
        write (*,*) '  Number_of_associations = ',nassoc
        if (nassoc.eq.0) stop

        return
        end



