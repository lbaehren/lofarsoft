
# scripts distributed as is (installed in share/)
data_DATA = apsr.cfg

dist_data_DATA = apsr.info \
                 apsr.viewer \
                 dspsr.cal \
                 dspsr.default \
                 dspsr.fourth \
                 dspsr.multi \
                 dspsr.select \
                 dspsr.single \
                 dspsr.singleF \
                 singleF.psh \
                 dspsr.giant \
                 giant.psh \
                 apsr.scratch \
                 local.scratch \
                 multi.txt \
                 pdfb3_1024_1024_2048 \
                 pdfb3_1024_256_2048 \
                 pdfb3_2048_512_2048 \
                 pdfb3_256_256_2048 \
                 pdfb3_512_256_2048 \
                 pdfb3_512_512_2048 \
                 names_J0737-3039.txt \
                 names_J0024-7204.txt \
                 names_J1910-5959.txt \
                 names_J1701-3006.txt \
                 names_J1748-2446.txt


nobase_dist_data_DATA = testing/CAL.spec \
                        testing/J0437_1405MHz.spec \
                        testing/J0437_2500MHz_dbdisk.spec \
                        testing/J0437_2500MHz.spec \
                        testing/J0737-3038.spec

EXTRA_DIST = apsr.cfg.in dspsr.giant.in dspsr.singleF.in
MOSTLYCLEANFILES=apsr.cfg dspsr.giant dspsr.singleF

edit = sed \
	-e 's|@datadir[@]|$(datadir)|g' \
	-e 's|@prefix[@]|$(prefix)|g'

dspsr.singleF dspsr.giant: Makefile
	rm -f $@ $@.tmp
	$(edit) '$(srcdir)/$@.in' >$@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

dspsr.singleF: $(srcdir)/dspsr.singleF.in
dspsr.giant: $(srcdir)/dspsr.giant.in

include $(top_srcdir)/config/Makefile.targets

