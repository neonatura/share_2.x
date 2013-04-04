if HAVE_DOXYGEN
dist_man_MANS = man/man1/shnet.1 man/man3/sh_buffer.3 man/man3/sh_fs.3 man/man3/sh_meta.3  
man/man1/shnet.1: doxyfile.stamp
man/man3/sh_buffer.3: doxyfile.stamp
man/man3/sh_fs.3: doxyfile.stamp
man/man3/sh_meta.3: doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) doxygen.conf
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	rm -rf $(top_srcdir)/doc/man
endif

#docdir = $(datadir)/doc/@PACKAGE@
#doc_DATA = README DONTS
