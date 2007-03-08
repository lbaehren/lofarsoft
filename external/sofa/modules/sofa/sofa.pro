SOFA_DIR=../..
TEMPLATE = subdirs

include($$SOFA_DIR/sofa.cfg)

SUBDIRS += component
SUBDIRS += simulation
TEMPLATE = subdirs
