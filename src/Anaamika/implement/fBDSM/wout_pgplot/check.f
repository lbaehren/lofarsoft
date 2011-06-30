c! check if there is enuff disk space 

        subroutine check(n,m,byte)
        implicit none
        integer n,m,byte,nf,tot,filesize
        character str1*100

c        call system('rm -f a b')
c        call system('quota > a')
c        str1="tail -1 a | awk '{print $1,$3}' > b"
c        call system(str1)
c        open(unit=21,file='b',status='old')
c        read (21,*) nf,tot
c        close(21)
c        filesize=(4+4+8)+n*m*8+m*8
c        nf=int(float(tot-nf)*1.d3/float(filesize))
c        if (nf.le.5) write (*,*) '  There is disk space for only ',
c     /      nf,' more such files.'
c        call system('rm -f a b')


        return
        end




