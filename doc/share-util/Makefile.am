if HAVE_DOXYGEN

if HAVE_HELP2MAN
MAN_PROG=$(HELP2MAN)

dist_man_MANS = \
	man/man1/shcat.1 \
	man/man1/shdiff.1 \
	man/man1/shdelta.1 \
	man/man1/shpatch.1 \
	man/man1/shcp.1 \
	man/man1/shln.1 \
	man/man1/shstat.1 \
	man/man1/shls.1 \
	man/man1/shattr.1 \
	man/man1/shpref.1 \
	man/man1/shrm.1 \
	man/man1/shrev.1 \
	man/man1/shpasswd.1 \
	man/man1/shinfo.1 \
	man/man1/shpkg.1 \
	man/man1/shdb.1 \
	man/man1/shcert.1 \
	man/man1/shfsck.1
else
MAN_PROG=true
endif


man/man1/shcat.1: helpfile.stamp
man/man1/shdiff.1: helpfile.stamp
man/man1/shdelta.1: helpfile.stamp
man/man1/shpatch.1: helpfile.stamp
man/man1/shcp.1: helpfile.stamp
man/man1/shln.1: helpfile.stamp
man/man1/shstat.1: helpfile.stamp
man/man1/shls.1: helpfile.stamp
man/man1/shattr.1: helpfile.stamp
man/man1/shpref.1: helpfile.stamp
man/man1/shrm.1: helpfile.stamp
man/man1/shrev.1: helpfile.stamp
man/man1/shpasswd.1: helpfile.stamp
man/man1/shinfo.1: helpfile.stamp
man/man1/shpkg.1: helpfile.stamp
man/man1/shdb.1: helpfile.stamp
man/man1/shcert.1: helpfile.stamp
man/man1/shfsck.1: helpfile.stamp

helpfile.stamp:
	mkdir -p man/man1
	$(MAN_PROG) --output=man/man1/shcat.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shcat
	$(MAN_PROG) --output=man/man1/shstat.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shstat
	$(MAN_PROG) --output=man/man1/shls.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shls
	$(MAN_PROG) --output=man/man1/shattr.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shattr
	$(MAN_PROG) --output=man/man1/shpref.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shpref
	$(MAN_PROG) --output=man/man1/shrm.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shrm
	$(MAN_PROG) --output=man/man1/shrev.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shrev
	$(MAN_PROG) --output=man/man1/shpasswd.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shpasswd
	$(MAN_PROG) --output=man/man1/shinfo.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shinfo
	$(MAN_PROG) --output=man/man1/shpkg.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shpkg
	$(MAN_PROG) --output=man/man1/shdb.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shdb
	$(MAN_PROG) --output=man/man1/shcert.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shcert
	$(MAN_PROG) --output=man/man1/shfsck.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shfsck
	$(MAN_PROG) --output=man/man1/shdiff.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shdiff
	$(MAN_PROG) --output=man/man1/shdelta.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shdelta
	$(MAN_PROG) --output=man/man1/shpatch.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shpatch
	$(MAN_PROG) --output=man/man1/shcp.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shcp
	$(MAN_PROG) --output=man/man1/shln.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/static/shln
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
