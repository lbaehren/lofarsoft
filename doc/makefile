doc:
	doxygen

cmake:
	cmake --help-html CMake.html

update:
	cd .. && svn update && cd doc && doxygen

clean:
	rm -f *~
	rm -rf html
