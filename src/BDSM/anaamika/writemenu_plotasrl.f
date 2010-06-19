c!      ----------
c!
        subroutine writemenu_plotasrl(nmenu)               ! defines order of var
        implicit none
        integer nmenu

        write (*,*) 
        write (*,*) ' 1. peakM       2. raM         3. decM  '
        write (*,*) ' 4. bmajM       5. bminM       6. bpaM  '
        write (*,*) ' 7. peakS       8. raS         9. decS  '
        write (*,*) '10. bmajS      11. bminS      12. bpaS  '
        write (*,*) '13. dist       14. peakratio  15. bpadiff'
        write (*,*) '16. bmajratio  17. bminratio  18. idM '
        write (*,*) '19. idS        20. src resrms 21. isl resrms' 
        write (*,*) '22. totalM     23. dtotalM    24. dpeakM '
        write (*,*) '25. totalS     26. dtotalS    27. dpeakS '
        write (*,*) '28. totalrat   29. ra M-S     30. dec M-S'
        write (*,*) '31. peak diff  32. expe       33. dist_src '
        write (*,*) '34. id         35. sp.in. pk  36. sp.in tot '
        write (*,*) '37. Tot/pkM    38. Tot/pkS    39. Bmaj*minM'
        write (*,*) '40. Bmaj*minS  41. Bmaj/beamM 42. Bmin/beamM'
        write (*,*) '43. Bmaj/beamS 44. Bmin.beamS 45. rmsM'
        write (*,*) '46. rmsS       47. SNR_M      48. SNR_S'
        write (*,*) '49. totM/pkS   50. PkM/totS   51. R_diff'
        write (*,*) '52. Theta_diff 53. RadialDist 54. Azimuth'
        write (*,*) '55. Posn diff  56. PosnDifAng 57. quit'
        write (*,*) 
        nmenu=57

        return
        end
