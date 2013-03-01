
default: Util MEAL psrchive

Util MEAL psrchive:
	@mkdir -p html/$@
	@cp commands.sty html/$@/commands.sty
	doxygen $@.dox > doxygen.out 2> doxygen.err
	@grep '^Error' doxygen.err || echo 2> /dev/null

install:
	cp html/*.tag `psrchive --html`
	./upload.csh

