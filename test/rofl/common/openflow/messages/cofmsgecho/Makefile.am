MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgechotest_SOURCES= unittest.cpp cofmsgechotest.hpp cofmsgechotest.cpp
cofmsgechotest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgechotest_LDFLAGS= -static
cofmsgechotest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgechotest
TESTS = cofmsgechotest
