MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

export INCLUDES += -I$(abs_srcdir)/../src/

unittest_SOURCES= \
	unittest.cc \
	cthread_test.cc \
	cthread_test.h

unittest_LDADD=$(top_builddir)/src/rofl/librofl_common.la -lcppunit -lpthread

check_PROGRAMS=unittest 

#TESTS=unittest

