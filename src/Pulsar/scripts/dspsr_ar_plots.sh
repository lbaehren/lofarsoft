#!/bin/sh
#
stem=$1
nch=$2

pav -DFTp -g ${stem}_DFTp.ps/cps $stem.ar
pav -GTpf$nch -g ${stem}_GTpf$nch.ps/cps $stem.ar
pav -YFp -g ${stem}_YFp.ps/cps $stem.ar
pav -B -g ${stem}_B.ps/cps $stem.ar
convert \( ${stem}_GTpf$nch.ps ${stem}_B.ps +append \) \( ${stem}_DFTp.ps ${stem}_YFp.ps +append \) -append -rotate 90 -background white -flatten ${stem}_diag.png
