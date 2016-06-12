if HAVE_DOXYGEN
CLEANFILES = helpfile.stamp

if HAVE_HELP2MAN
MAN_PROG=$(HELP2MAN)
else
MAN_PROG=true
endif

dist_man_MANS = \
	shared

man/man1/shared.1: helpfile.stamp

helpfile.stamp:
	mkdir -p man/man1
	$(MAN_PROG) --output=man/man1/shared.1 --name='libshare transaction daemon' $(top_builddir)/src/share-daemon/shared
	echo Timestamp > helpfile.stamp

$(htmldir): doxyfile.stamp

doxyfile.stamp:
	mkdir -p html man
	$(DOXYGEN) doxygen.conf
	echo Timestamp > doxyfile.stamp

all-local: doxyfile.stamp helpfile.stamp
clean-local:
	rm -f doxyfile.stamp helpfile.stamp
	rm -rf $(builddir)/man
	rm -rf $(builddir)/$(htmldir)

endif
