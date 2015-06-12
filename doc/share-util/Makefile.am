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
	man/man1/shinfo.1 \
	man/man1/shdb.1 \
	man/man1/shfsck.1

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
man/man1/shdb.1: helpfile.stamp
man/man1/shfsck.1: helpfile.stamp

helpfile.stamp:
	mkdir -p man/man1
	$(HELP2MAN) --output=man/man1/shcat.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shcat
	$(HELP2MAN) --output=man/man1/shstat.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shstat
	$(HELP2MAN) --output=man/man1/shls.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shls
	$(HELP2MAN) --output=man/man1/shattr.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shattr
	$(HELP2MAN) --output=man/man1/shpref.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shpref
	$(HELP2MAN) --output=man/man1/shrm.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shrm
	$(HELP2MAN) --output=man/man1/shrev.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shrev
	$(HELP2MAN) --output=man/man1/shpasswd.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shpasswd
	$(HELP2MAN) --output=man/man1/shinfo.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shinfo
	$(HELP2MAN) --output=man/man1/shdb.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shdb
	$(HELP2MAN) --output=man/man1/shfsck.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shfsck
	$(HELP2MAN) --output=man/man1/shdiff.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shdiff
	$(HELP2MAN) --output=man/man1/shdelta.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shdelta
	$(HELP2MAN) --output=man/man1/shpatch.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shpatch
	$(HELP2MAN) --output=man/man1/shcp.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shcp
	$(HELP2MAN) --output=man/man1/shln.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shln
	$(HELP2MAN) --output=man/man1/shar.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shar
	$(HELP2MAN) --output=man/man1/unshar.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/unshar
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
