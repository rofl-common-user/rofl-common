MAINTAINERCLEANFILES = \
	$(top_srcdir)/aclocal.m4 \
	$(top_srcdir)/configure \
	$(top_srcdir)/Makefile.in

export INCLUDES += -I$(abs_srcdir)/src/ -I$(abs_builddir)/src/
SUBDIRS = src test examples tools doc 
export LIBROFL=$(PWD)/src/rofl/librofl_common.la

EXTRA_DIST = \
	autogen.sh \
	AUTHORS \
	CHANGES \
	COPYRIGHT \
	README \
	VERSION

#Could be improved.. 
.PHONY: doc
doc:
	@cd doc/ && make doc		

install-exec-hook:

uninstall-hook:

maintainer-clean-local:
	rm -rf $(top_srcdir)/m4
	rm -rf $(top_srcdir)/build-aux
