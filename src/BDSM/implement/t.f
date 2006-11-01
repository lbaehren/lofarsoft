
        implicit none 
        integer nsrc,i,hh,dd,mm,ma,nchar
        real*8 ss,sa,ra,dec,ngau,dumr
        character command*500,gname*500,dumc*500,str*10

        gname='/data/niruj_pers/image/BOOTES.trial.gaul'
        open(unit=21,file=gname(1:nchar(gname)),form='formatted')
        call readhead_srclist(21,19,'Number_of_gaussians',dumc,ngau,
     /       dumr,'i')
        close(21)

        call int2str(ngau,str)
        command="tail -"//str(1:nchar(str))//" "//gname(1:nchar(gname))
     /           //" | awk '{print $8, $10}' > a"
        call system(command)

        command="tail -"//str(1:nchar(str))//" "//gname(1:nchar(gname))
     /   //"|awk '{print $8,$9,$10,$11,$4,$5,$6,$7,$16,$17,$18,$19,$20,"
     /   //"$21,$22,$23,$24,$25,$26,$27,$30}' | sort -r -g > BDSMFILE"
        call system(command)
        

        end
