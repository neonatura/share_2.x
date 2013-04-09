AM_CFLAGS = -DDATADIR=\"$(datadir)\"

htmldir = html 
dist_man_MANS = man/man1/share.1

dist_docdir = $(datadir)/@PACKAGE@
dist_doc_DATA = $(top_srcdir)/doc/libshare_html.tar.xz

man/man1/share.1: helpfile.stamp

helpfile.stamp:
	mkdir -p man/man1
	cp -f $(top_srcdir)/doc/man/man1/share.1 man/man1
	$(HELP2MAN) --output=man/man1/share.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/share
	echo Timestamp > helpfile.stamp

dist_man_MANS += \
	man/man3/libshare.3 \
	man/man3/libshare_fs.3 \
	man/man3/libshare_socket.3 \
	man/man3/shbuf_init.3 \
	man/man3/shbuf_catstr.3 \
	man/man3/shbuf_cat.3 \
	man/man3/shbuf_size.3 \
	man/man3/shbuf_clear.3 \
	man/man3/shbuf_trim.3 \
	man/man3/shbuf_free.3 \
	man/man3/shmeta_init.3 \
	man/man3/shmeta_free.3 \
	man/man3/shmeta_set.3 \
	man/man3/shmeta_set_str.3 \
	man/man3/shmeta_unset_str.3 \
	man/man3/shmeta_set_void.3 \
	man/man3/shmeta_unset_void.3 \
	man/man3/shmeta_get_str.3 \
	man/man3/shmeta_get_void.3 \
	man/man3/shmeta_get.3 \
	man/man3/shmeta_print.3

man/man3/libshare.3: doxyfile.stamp
man/man3/libshare_fs.3: doxyfile.stamp
man/man3/libshare_socket.3: doxyfile.stamp
man/man3/shbuf_init.3: doxyfile.stamp
man/man3/shbuf_catstr.3: doxyfile.stamp
man/man3/shbuf_cat.3: doxyfile.stamp
man/man3/shbuf_size.3: doxyfile.stamp
man/man3/shbuf_clear.3: doxyfile.stamp
man/man3/shbuf_trim.3: doxyfile.stamp
man/man3/shbuf_free.3: doxyfile.stamp
man/man3/shmeta_init.3: doxyfile.stamp
man/man3/shmeta_free.3: doxyfile.stamp
man/man3/shmeta_set.3: doxyfile.stamp
man/man3/shmeta_set_str.3: doxyfile.stamp
man/man3/shmeta_unset_str.3: doxyfile.stamp
man/man3/shmeta_set_void.3: doxyfile.stamp
man/man3/shmeta_unset_void.3: doxyfile.stamp
man/man3/shmeta_get_str.3: doxyfile.stamp
man/man3/shmeta_get_void.3: doxyfile.stamp
man/man3/shmeta_get.3: doxyfile.stamp
man/man3/shmeta_print.3: doxyfile.stamp

if HAVE_DOXYGEN

$(htmldir): doxyfile.stamp

doxyfile.stamp:
	mkdir -p html man
	$(DOXYGEN) doxygen.conf
	mkdir -p $(top_srcdir)/doc/man
	cp -fdR man/* $(top_srcdir)/doc/man
	cp -f man/man3/libshare_membuf.3 man/man3/shbuf_init.3
	cp -f man/man3/libshare_membuf.3 man/man3/shbuf_catstr.3
	cp -f man/man3/libshare_membuf.3 man/man3/shbuf_cat.3
	cp -f man/man3/libshare_membuf.3 man/man3/shbuf_size.3
	cp -f man/man3/libshare_membuf.3 man/man3/shbuf_clear.3
	cp -f man/man3/libshare_membuf.3 man/man3/shbuf_trim.3
	cp -f man/man3/libshare_membuf.3 man/man3/shbuf_free.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_init.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_free.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_set.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_set_str.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_unset_str.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_set_void.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_unset_void.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_get_str.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_get_void.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_get.3
	cp -f man/man3/libshare_memmeta.3 man/man3/shmeta_print.3
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

