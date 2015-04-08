if HAVE_DOXYGEN

dist_man_MANS = \
	man/man1/shcat.1 \
	man/man1/shdiff.1 \
	man/man1/shdelta.1 \
	man/man1/shpatch.1 \
	man/man1/shcp.1 \
	man/man1/shln.1 \
	man/man1/shar.1 \
	man/man1/unshar.1 \
	man/man1/shstat.1 \
	man/man1/shls.1 \
	man/man1/shattr.1 \
	man/man1/shpref.1 \
	man/man1/shrm.1 \
	man/man1/shrev.1 \
	man/man1/shpasswd.1 \
	man/man1/shinfo.1

man/man1/shcat.1: helpfile.stamp
man/man1/shdiff.1: helpfile.stamp
man/man1/shdelta.1: helpfile.stamp
man/man1/shpatch.1: helpfile.stamp
man/man1/shcp.1: helpfile.stamp
man/man1/shln.1: helpfile.stamp
man/man1/shar.1: helpfile.stamp
man/man1/unshar.1: helpfile.stamp
man/man1/shstat.1: helpfile.stamp
man/man1/shls.1: helpfile.stamp
man/man1/shattr.1: helpfile.stamp
man/man1/shpref.1: helpfile.stamp
man/man1/shrm.1: helpfile.stamp
man/man1/shrev.1: helpfile.stamp
man/man1/shpasswd.1: helpfile.stamp
man/man1/shinfo.1: helpfile.stamp

helpfile.stamp:
	mkdir -p man/man1
	$(HELP2MAN) --output=man/man1/shcat.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shcat
	$(HELP2MAN) --output=man/man1/shstat.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shstat
	$(HELP2MAN) --output=man/man1/shls.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shls
	$(HELP2MAN) --output=man/man1/shattr.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shattr
	$(HELP2MAN) --output=man/man1/shpref.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shpref
	$(HELP2MAN) --output=man/man1/shrm.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shrm
	$(HELP2MAN) --output=man/man1/shrev.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shrev
	$(HELP2MAN) --output=man/man1/shpasswd.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shpasswd
	$(HELP2MAN) --output=man/man1/shinfo.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shinfo
	$(HELP2MAN) --output=man/man1/shdiff.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shdiff
	$(HELP2MAN) --output=man/man1/shdelta.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shdelta
	$(HELP2MAN) --output=man/man1/shpatch.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shpatch
	$(HELP2MAN) --output=man/man1/shcp.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shcp
	$(HELP2MAN) --output=man/man1/shln.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shln
	$(HELP2MAN) --output=man/man1/shar.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shar
	$(HELP2MAN) --output=man/man1/unshar.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/unshar
	echo Timestamp > helpfile.stamp

html: doxyfile.stamp

doxyfile.stamp:
	mkdir -p html man
	$(DOXYGEN) doxygen.conf
	echo Timestamp > doxyfile.stamp

CLEANFILES = helpfile.stamp
all-local: doxyfile.stamp helpfile.stamp
clean-local:
	rm -rf $(builddir)/html $(builddir)/man doxyfile.stamp helpfile.stamp

endif
