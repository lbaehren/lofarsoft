DOXY_EXEC_PATH = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin
DOXYFILE = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/-
DOXYDOCS_PM = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/DoxyDocs.pm
DOXYSTRUCTURE_PM = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/DoxyStructure.pm
DOXYRULES = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/doxyrules.make
DOXYLATEX_PL = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/doxylatex.pl
DOXYLATEXSTRUCTURE_PL = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/doxylatex-structure.pl
DOXYSTRUCTURE_TEX = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/doxystructure.tex
DOXYDOCS_TEX = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/doxydocs.tex
DOXYFORMAT_TEX = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/doxyformat.tex
DOXYLATEX_TEX = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/doxylatex.tex
DOXYLATEX_DVI = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/doxylatex.dvi
DOXYLATEX_PDF = /home/vilchez/Bureau/dynspec/doc/ICD3-ICD6-Rebin/perlmod/doxylatex.pdf

.PHONY: clean-perlmod
clean-perlmod::
	rm -f $(DOXYSTRUCTURE_PM) \
	$(DOXYDOCS_PM) \
	$(DOXYLATEX_PL) \
	$(DOXYLATEXSTRUCTURE_PL) \
	$(DOXYDOCS_TEX) \
	$(DOXYSTRUCTURE_TEX) \
	$(DOXYFORMAT_TEX) \
	$(DOXYLATEX_TEX) \
	$(DOXYLATEX_PDF) \
	$(DOXYLATEX_DVI) \
	$(addprefix $(DOXYLATEX_TEX:tex=),out aux log)

$(DOXYRULES) \
$(DOXYMAKEFILE) \
$(DOXYSTRUCTURE_PM) \
$(DOXYDOCS_PM) \
$(DOXYLATEX_PL) \
$(DOXYLATEXSTRUCTURE_PL) \
$(DOXYFORMAT_TEX) \
$(DOXYLATEX_TEX): \
	$(DOXYFILE)
	cd $(DOXY_EXEC_PATH) ; doxygen "$<"

$(DOXYDOCS_TEX): \
$(DOXYLATEX_PL) \
$(DOXYDOCS_PM)
	perl -I"$(<D)" "$<" >"$@"

$(DOXYSTRUCTURE_TEX): \
$(DOXYLATEXSTRUCTURE_PL) \
$(DOXYSTRUCTURE_PM)
	perl -I"$(<D)" "$<" >"$@"

$(DOXYLATEX_PDF) \
$(DOXYLATEX_DVI): \
$(DOXYLATEX_TEX) \
$(DOXYFORMAT_TEX) \
$(DOXYSTRUCTURE_TEX) \
$(DOXYDOCS_TEX)

$(DOXYLATEX_PDF): \
$(DOXYLATEX_TEX)
	pdflatex -interaction=nonstopmode "$<"

$(DOXYLATEX_DVI): \
$(DOXYLATEX_TEX)
	latex -interaction=nonstopmode "$<"
