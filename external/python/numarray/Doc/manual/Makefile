# Makefile for numarray manual
#
# Copyright (C) 2002 Jochen Küpper


# some programs, might need adjustment
GZIP		= gzip -9f
MKHOWTO		= python2 ./mkhowto


# some information for generated html
MAILINGLIST	= numpy-discussion@lists.sourceforge.net
ADDRESS		= "Send comments to the <a href=\"mailto:$(MAILINGLIST)\">NumArray community</a>."
OPTIONS 	= --address=$(ADDRESS) --split=4

WEBDIR		= shell.sourceforge.net:/home/groups/n/nu/numpy/htdocs/numarray/manual/


# LaTeX source files
SOURCES		= arrayattributes.tex\
		arraybasics.tex \
                arrayindexing.tex \
                intermediate.tex \
		arrayfunctions.tex \
		arraymethods.tex \
		convolve.tex \
		copyright.tex \
		extending.tex \
		chararray.tex \
		recordarray.tex \
		objectarray.tex \
		fft.tex \
		glossary.tex \
		installation.tex \
		introduction.tex \
		linearalgebra.tex \
		ma.tex \
		mlab.tex \
		numarray.tex \
		overview.tex \
                randomarray.tex \
		memmap.tex \
		ufuncs.tex


# entry points

default:	pdf html # need soemthing to generate a current index before html

# some targets (noteably ps) remove the dvi, so make that last
all:		pdf html ps text dvi

dvi:		numarray.dvi

html:		numarray

pdf:		numarray.pdf

ps:		numarray.ps.gz

text:		numarray.txt.gz


web:		all
		@scp -r icons numarray numarray.dvi numarray.pdf numarray.ps.gz numarray.txt.gz $(WEBDIR)


# some real targets

mkhowto:
		echo "You need to create a link to mkhowto,"
		echo "see the README for details."

numarray:	Makefile mkhowto $(SOURCES)
		$(MKHOWTO) --html $(OPTIONS) numarray.tex

numarray.dvi:	Makefile mkhowto $(SOURCES)
		$(MKHOWTO) --dvi $(OPTIONS) numarray.tex

numarray.pdf:	Makefile mkhowto $(SOURCES)
		$(MKHOWTO) --pdf $(OPTIONS) numarray.tex

numarray.ps.gz:	Makefile mkhowto $(SOURCES)
		$(MKHOWTO) --ps $(OPTIONS) numarray.tex
		$(GZIP) numarray.ps

numarray.txt.gz: Makefile mkhowto $(SOURCES)
		$(MKHOWTO) --text $(OPTIONS) numarray.tex
		$(GZIP) numarray.txt

