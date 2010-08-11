
        real*8 a

        a=1.d0

        open(unit=21,file='a',form='unformatted')
        write (21) a
        close(21)

        end
