doc:
	doxygen

cmake:
	cmake --help-html CMake.html

update:
	cd .. && svn update && cd doc && doxygen

clean:
	rm -f *~
	rm -f html/*.png rm -f html/*.map rm -f html/*.html rm -f html/*.md5
