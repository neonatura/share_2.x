AM_CFLAGS = -DDATADIR=\"$(datadir)\"

htmldir = html 
dist_man_MANS = man/man1/shnet.1

dist_docdir = $(datadir)/@PACKAGE@
dist_doc_DATA = $(top_srcdir)/doc/libshare_html.tar.xz

man/man1/shnet.1: helpfile.stamp

helpfile.stamp:
	mkdir -p man/man1
	cp -f $(top_srcdir)/doc/man/man1/shnet.1 man/man1
	$(HELP2MAN) --output=man/man1/shnet.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shnet
	echo Timestamp > helpfile.stamp

dist_man_MANS += \
	man/man3/libshare.3 \
	man/man3/libshare_fs.3 \
	man/man3/libshare_meta.3 \
	man/man3/libshare_socket.3

man/man3/libshare.3: doxyfile.stamp
man/man3/libshare_fs.3: doxyfile.stamp
man/man3/libshare_meta.3: doxyfile.stamp
man/man3/libshare_socket.3: doxyfile.stamp

if HAVE_DOXYGEN

$(htmldir): doxyfile.stamp

doxyfile.stamp:
#	mkdir -p $(top_srcdir)/doc/$(htmldir)
#	ln -sf $(top_srcdir)/doc/$(htmldir)
	mkdir -p html man
	$(DOXYGEN) doxygen.conf
	mkdir -p $(top_srcdir)/doc/man
	cp -fdR man/* $(top_srcdir)/doc/man
	echo Timestamp > doxyfile.stamp

$(top_srcdir)/doc/libshare_html.tar.xz: doxyfile.stamp
	rm -f $(top_srcdir)/doc/libshare_html.tar.xz
	tar -cpf $(top_srcdir)/doc/libshare_html.tar $(htmldir)
	xz $(top_srcdir)/doc/libshare_html.tar
	rm -f $(top_srcdir)/doc/libshare_html.tar

else

doxyfile.stamp:
	mkdir -p man
	cp -fdR $(top_srcdir)/doc/man/* ./man
	echo Timestamp > doxyfile.stamp

endif

CLEANFILES = helpfile.stamp

EXTRA_DIST = $(htmldir)

all-local: doxyfile.stamp helpfile.stamp
clean-local:
	rm -f doxyfile.stamp helpfile.stamp
	rm -rf $(top_builddir)/doc/man
	rm -rf $(top_builddir)/doc/$(htmldir)

