AUTOMAKE_OPTIONS = foreign
MAINTAINERCLEANFILES = all-local $(php_wrap_sources) Makefile.in

if HAVE_SWIG
SWIG_PROG=swig
else
SWIG_PROG=true
endif

common_includes = \
	$(top_srcdir)/src/share-lib/share.h \
	$(top_srcdir)/src/share-lib/sherr.h \
	$(top_srcdir)/src/share-lib/share_macro.h \
	$(top_srcdir)/src/share-lib/shpeer.h \
	$(top_srcdir)/src/share-lib/shtime.h \
	$(top_srcdir)/src/share-lib/mem/shmem.h \
	$(top_srcdir)/src/share-lib/mem/shmem_buf.h \
	$(top_srcdir)/src/share-lib/mem/shmem_pool.h \
	$(top_srcdir)/src/share-lib/mem/shmem_meta.h \
	$(top_srcdir)/src/share-lib/mem/shmem_crypt.h \
	$(top_srcdir)/src/share-lib/mem/shmem_key.h \
	$(top_srcdir)/src/share-lib/mem/shmem_lock.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-internal.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-list.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-main.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-test.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/fs/shfs.h \
	$(top_srcdir)/src/share-lib/fs/shfs_meta.h \
	$(top_srcdir)/src/share-lib/fs/shfs_journal.h \
	$(top_srcdir)/src/share-lib/fs/shfs_read.h \
	$(top_srcdir)/src/share-lib/fs/shfs_write.h \
	$(top_srcdir)/src/share-lib/fs/shfs_partition.h \
	$(top_srcdir)/src/share-lib/fs/shfs_inode.h \
	$(top_srcdir)/src/share-lib/fs/shfs_dir.h \
	$(top_srcdir)/src/share-lib/shpref.h \
  $(top_srcdir)/src/share-lib/net/shnet.h \
  $(top_srcdir)/src/share-lib/net/shnet_write.h \
  $(top_srcdir)/src/share-lib/net/shnet_read.h \
  $(top_srcdir)/src/share-lib/net/shnet_bind.h \
  $(top_srcdir)/src/share-lib/net/shnet_connect.h \
  $(top_srcdir)/src/share-lib/net/shnet_fcntl.h \
  $(top_srcdir)/src/share-lib/net/shnet_gethost.h \
  $(top_srcdir)/src/share-lib/net/shnet_close.h \
  $(top_srcdir)/src/share-lib/net/shnet_socket.h \
  $(top_srcdir)/src/share-lib/net/shnet_accept.h \
  $(top_srcdir)/src/share-lib/net/shnet_write.h 



BUILT_SOURCES = phpshare_wrap.c
SWIG_CONF = $(top_srcdir)/wrap/php/phpshare.i

SWIG_PHP_OPT = -php -outdir "$(top_builddir)/wrap/php/"

phpshare_wrap.c: .swig-stamp $(common_includes)
phpshare.php: .swig-stamp

.swig-stamp:
	$(SWIG_PROG) $(SWIG_PHP_OPT) $(SWIG_CONF)
	echo Timestamp > .swig-stamp

all-local: .swig-stamp 

clean-local:
	rm -f .swig-stamp 


