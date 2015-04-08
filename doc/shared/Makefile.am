if HAVE_DOXYGEN
CLEANFILES = helpfile.stamp

dist_man_MANS = \
	shared

man/man1/shared.1: helpfile.stamp

helpfile.stamp:
	mkdir -p man/man1
	$(HELP2MAN) --output=man/man1/shared.1 --name='libshare transaction daemon' $(top_builddir)/src/share-daemon/shared
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
