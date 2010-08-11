
        implicit none
        character nam*40,fn*500

        fn='wenss'
        call coord2name(10.234d0,80.37d0,fn,nam)
        write (*,*) nam
        call coord2name(0.234d0,-80.37d0,fn,nam)
        write (*,*) nam
        call coord2name(100.234d0,0.37d0,fn,nam)
        write (*,*) nam
        call coord2name(350.234d0,-0.37d0,fn,nam)
        write (*,*) nam

        end
