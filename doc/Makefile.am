AM_CFLAGS = -DDATADIR=\"$(datadir)\"

htmldir = html 
dist_man_MANS = \
	man/man1/share.1 \
	man/man1/shared.1 \
	man/man1/shcat.1 \
	man/man1/shmkdir.1 \
	man/man1/shcp.1 \
	man/man1/shls.1 \
	man/man1/shping.1 \
	man/man1/shrm.1

dist_docdir = $(datadir)/@PACKAGE@
dist_doc_DATA = $(top_srcdir)/doc/libshare_html.tar.xz

man/man1/share.1: helpfile.stamp
man/man1/shared.1: helpfile.stamp
man/man1/shcat.1: helpfile.stamp
man/man1/shmkdir.1: helpfile.stamp
man/man1/shcp.1: helpfile.stamp
man/man1/shls.1: helpfile.stamp
man/man1/shping.1: helpfile.stamp
man/man1/shrm.1: helpfile.stamp

helpfile.stamp:
	mkdir -p man/man1
	cp -f $(top_srcdir)/doc/man/man1/share.1 man/man1
	$(HELP2MAN) --output=man/man1/share.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/share
	$(HELP2MAN) --output=man/man1/shared.1 --name='libshare command-line tool' $(top_builddir)/scripts/shared
	$(HELP2MAN) --output=man/man1/shcat.1 --name='libshare command-line tool' $(top_builddir)/scripts/shcat
	$(HELP2MAN) --output=man/man1/shmkdir.1 --name='libshare command-line tool' $(top_builddir)/scripts/shmkdir
	$(HELP2MAN) --output=man/man1/shls.1 --name='libshare command-line tool' $(top_builddir)/scripts/shls
	$(HELP2MAN) --output=man/man1/shping.1 --name='libshare command-line tool' $(top_builddir)/scripts/shping
	$(HELP2MAN) --output=man/man1/shrm.1 --name='libshare command-line tool' $(top_builddir)/scripts/shrm
	$(HELP2MAN) --output=man/man1/shcp.1 --name='libshare command-line tool' $(top_builddir)/scripts/shcp
	echo Timestamp > helpfile.stamp

dist_man_MANS += \
	man/man3/libshare.3 \
	man/man3/libshare_mem.3 \
	man/man3/libshare_fs.3 \
	man/man3/libshare_net.3 \
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
	man/man3/shcrypt_ashencode.3 \
	man/man3/shcrypt_shencode.3 \
	man/man3/shcrypt_shencode_str.3 \
	man/man3/shcrypt_ashdecode.3 \
	man/man3/shcrypt_shdecode.3 \
	man/man3/shcrypt_shdecode_str.3 \
	man/man3/shkey_bin.3 \
	man/man3/shkey_str.3 \
	man/man3/shkey_num.3 \
	man/man3/shkey_uniq.3 \
	man/man3/shkey_free.3 \
	man/man3/shkey_print.3 \
	man/man3/ashkey_str.3 \
	man/man3/ashkey_num.3 \
	man/man3/shlock_open.3 \
	man/man3/shlock_tryopen.3 \
	man/man3/shlock_close.3 \
	man/man3/shlock_destroy.3 \
	man/man3/shpool_init.3 \
	man/man3/shpool_size.3 \
	man/man3/shpool_grow.3 \
	man/man3/shpool_get.3 \
	man/man3/shpool_get_index.3 \
	man/man3/shpool_size.3 \
	man/man3/shpool_grow.3 \
	man/man3/shpool_put.3 \
	man/man3/shpool_free.3 
	man/man3/shnet_accept.3 \
	man/man3/shnet_bindsk.3 \
	man/man3/shnet_bind.3 \
	man/man3/shnet_close.3 \
	man/man3/shnet_conn.3 \
	man/man3/shnet_connect.3 \
	man/man3/shnet_fcntl.3 \
	man/man3/shnet_gethostbyname.3 \
	man/man3/shnet_peer.3 \
	man/man3/shnet_read.3 \
	man/man3/shnet_verify.3 \
	man/man3/shnet_select.3 \
	man/man3/shnet_sk.3 \
	man/man3/shnet_socket.3 \
	man/man3/shnet_write.3

man/man3/libshare.3: doxyfile.stamp
man/man3/libshare_mem.3: doxyfile.stamp
man/man3/libshare_fs.3: doxyfile.stamp
man/man3/libshare_net.3: doxyfile.stamp
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
man/man3/shcrypt_ashencode.3: doxyfile.stamp
man/man3/shcrypt_shencode.3: doxyfile.stamp
man/man3/shcrypt_shencode_str.3: doxyfile.stamp
man/man3/shcrypt_ashdecode.3: doxyfile.stamp
man/man3/shcrypt_shdecode.3: doxyfile.stamp
man/man3/shcrypt_shdecode_str.3: doxyfile.stamp
man/man3/shkey_bin.3: doxyfile.stamp
man/man3/shkey_str.3: doxyfile.stamp
man/man3/shkey_num.3: doxyfile.stamp
man/man3/shkey_uniq.3: doxyfile.stamp
man/man3/shkey_free.3: doxyfile.stamp
man/man3/shkey_print.3: doxyfile.stamp
man/man3/ashkey_str.3: doxyfile.stamp
man/man3/ashkey_num.3: doxyfile.stamp
man/man3/shlock_open.3: doxyfile.stamp
man/man3/shlock_tryopen.3: doxyfile.stamp
man/man3/shlock_close.3: doxyfile.stamp
man/man3/shlock_destroy.3: doxyfile.stamp
man/man3/shpool_init.3: doxyfile.stamp
man/man3/shpool_size.3: doxyfile.stamp
man/man3/shpool_grow.3: doxyfile.stamp
man/man3/shpool_get.3: doxyfile.stamp
man/man3/shpool_get_index.3: doxyfile.stamp
man/man3/shpool_size.3: doxyfile.stamp
man/man3/shpool_grow.3: doxyfile.stamp
man/man3/shpool_put.3: doxyfile.stamp
man/man3/shpool_free.3: doxyfile.stamp
man/man3/shnet_accept.3: doxyfile.stamp
man/man3/shnet_bindsk.3: doxyfile.stamp
man/man3/shnet_bind.3: doxyfile.stamp
man/man3/shnet_close.3: doxyfile.stamp
man/man3/shnet_conn.3: doxyfile.stamp
man/man3/shnet_connect.3: doxyfile.stamp
man/man3/shnet_fcntl.3: doxyfile.stamp
man/man3/shnet_gethostbyname.3: doxyfile.stamp
man/man3/shnet_peer.3: doxyfile.stamp
man/man3/shnet_read.3: doxyfile.stamp
man/man3/shnet_verify.3: doxyfile.stamp
man/man3/shnet_select.3: doxyfile.stamp
man/man3/shnet_sk.3: doxyfile.stamp
man/man3/shnet_socket.3: doxyfile.stamp
man/man3/shnet_write.3: doxyfile.stamp

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
	cp -f man/man3/libshare_membuf.3 man/man3/shbuf_grow.3
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
	cp -f man/man3/libshare_memcrypt.3 man/man3/shcrypt_ashencode.3
	cp -f man/man3/libshare_memcrypt.3 man/man3/shcrypt_shencode.3
	cp -f man/man3/libshare_memcrypt.3 man/man3/shcrypt_shencode_str.3
	cp -f man/man3/libshare_memcrypt.3 man/man3/shcrypt_ashdecode.3
	cp -f man/man3/libshare_memcrypt.3 man/man3/shcrypt_shdecode.3
	cp -f man/man3/libshare_memcrypt.3 man/man3/shcrypt_shdecode_str.3
	cp -f man/man3/libshare_memkey.3 man/man3/shkey_bin.3
	cp -f man/man3/libshare_memkey.3 man/man3/shkey_str.3
	cp -f man/man3/libshare_memkey.3 man/man3/shkey_num.3
	cp -f man/man3/libshare_memkey.3 man/man3/shkey_uniq.3
	cp -f man/man3/libshare_memkey.3 man/man3/shkey_free.3
	cp -f man/man3/libshare_memkey.3 man/man3/shkey_print.3
	cp -f man/man3/libshare_memkey.3 man/man3/ashkey_str.3
	cp -f man/man3/libshare_memkey.3 man/man3/ashkey_num.3
	cp -f man/man3/libshare_memlock.3 man/man3/shlock_open.3
	cp -f man/man3/libshare_memlock.3 man/man3/shlock_tryopen.3
	cp -f man/man3/libshare_memlock.3 man/man3/shlock_close.3
	cp -f man/man3/libshare_memlock.3 man/man3/shlock_destroy.3
	cp -f man/man3/libshare_mempool.3 man/man3/shpool_init.3
	cp -f man/man3/libshare_mempool.3 man/man3/shpool_size.3
	cp -f man/man3/libshare_mempool.3 man/man3/shpool_grow.3
	cp -f man/man3/libshare_mempool.3 man/man3/shpool_get.3
	cp -f man/man3/libshare_mempool.3 man/man3/shpool_get_index.3
	cp -f man/man3/libshare_mempool.3 man/man3/shpool_size.3
	cp -f man/man3/libshare_mempool.3 man/man3/shpool_grow.3
	cp -f man/man3/libshare_mempool.3 man/man3/shpool_put.3
	cp -f man/man3/libshare_mempool.3 man/man3/shpool_free.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_accept.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_bindsk.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_bind.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_close.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_conn.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_connect.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_fcntl.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_gethostbyname.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_peer.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_read.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_verify.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_select.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_sk.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_socket.3
	cp -f man/man3/libshare_net.3 man/man3/shnet_write.3
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

