AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD) $(LIB_PTHREAD) -version-info @version_info@

noinst_PROGRAMS = test_libshare test_libshare_static

#test_libshare
test_libshare_SOURCES = $(common_INCLUDES) test_libshare.c
test_libshare_LDADD = libshare.la

# test_libshare_static
test_libshare_static_SOURCES = $(common_INCLUDES) test_libshare.c
test_libshare_static_LDFLAGS = -static
test_libshare_static_LDADD = libshare.a

TESTS = test_libshare_static test_libshare

if LINUX
install-exec-hook:
	mkdir -p /var/lib/share
	chmod 0777 /var/lib/share
endif

tar_sources = \
	arch/buffer.c \
	arch/checkpoint.c \
	arch/common.h \
	arch/compare.c \
	arch/create.c \
	arch/delete.c \
	arch/exit.c \
	arch/extract.c \
	arch/incremen.c \
	arch/list.c \
	arch/misc.c \
	arch/names.c \
	arch/sparse.c \
	arch/suffix.c \
	arch/system.c \
	arch/tar.c \
	arch/transform.c \
	arch/unlink.c \
	arch/update.c \
	arch/utf8.c \
	arch/warning.c \
	arch/xheader.c

include_HEADERS = \
	share.h share_base.h sherr.h shtime.h shcrc.h shpeer.h shpref.h \
	net/shnet.h fs/shfs.h mem/shmem.h 

common_INCLUDES = \
	$(top_builddir)/config.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-internal.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-list.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-main.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-test.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/shpref.h \
	$(top_srcdir)/src/share-lib/test/CuTest.h \
	$(top_srcdir)/src/share-lib/test/shtest.h

common_sources = \
	$(top_srcdir)/src/share-lib/share.c \
	$(top_srcdir)/src/share-lib/mem/shmem.c \
	$(top_srcdir)/src/share-lib/mem/shmem_buf.c \
	$(top_srcdir)/src/share-lib/mem/shmem_pool.c \
	$(top_srcdir)/src/share-lib/mem/shmem_meta.c \
	$(top_srcdir)/src/share-lib/mem/shmem_crypt.c \
	$(top_srcdir)/src/share-lib/mem/shmem_key.c \
	$(top_srcdir)/src/share-lib/mem/shmem_lock.c \
	$(top_srcdir)/src/share-lib/mem/shmem_scrypt.c \
	$(top_srcdir)/src/share-lib/mem/shmem_scrypt_gen.c \
	$(top_srcdir)/src/share-lib/mem/shmem_json.c \
	$(top_srcdir)/src/share-lib/mem/shmem_digest.c \
	$(top_srcdir)/src/share-lib/fs/shfs_read.c \
	$(top_srcdir)/src/share-lib/fs/shfs.c \
	$(top_srcdir)/src/share-lib/fs/shfs_meta.c \
	$(top_srcdir)/src/share-lib/fs/shfs_journal.c \
	$(top_srcdir)/src/share-lib/fs/shfs_write.c \
	$(top_srcdir)/src/share-lib/fs/shfs_proc.c \
	$(top_srcdir)/src/share-lib/fs/shfs_rev.c \
	$(top_srcdir)/src/share-lib/fs/shfs_partition.c \
	$(top_srcdir)/src/share-lib/fs/shfs_inode.c \
	$(top_srcdir)/src/share-lib/fs/shfs_link.c \
	$(top_srcdir)/src/share-lib/fs/shfs_dir.c \
	$(top_srcdir)/src/share-lib/fs/shfs_file.c \
	$(top_srcdir)/src/share-lib/fs/shfs_cache.c \
	$(top_srcdir)/src/share-lib/fs/shfs_aux.c \
	$(top_srcdir)/src/share-lib/fs/shfs_log.c \
	$(top_srcdir)/src/share-lib/fs/shfs_msg.c \
	$(top_srcdir)/src/share-lib/delta/xdelta3.c \
	$(top_srcdir)/src/share-lib/net/shnet.c \
	$(top_srcdir)/src/share-lib/net/shnet_read.c \
	$(top_srcdir)/src/share-lib/net/shnet_bind.c \
	$(top_srcdir)/src/share-lib/net/shnet_connect.c \
	$(top_srcdir)/src/share-lib/net/shnet_fcntl.c \
	$(top_srcdir)/src/share-lib/net/shnet_gethost.c \
	$(top_srcdir)/src/share-lib/net/shnet_close.c \
	$(top_srcdir)/src/share-lib/net/shnet_socket.c \
	$(top_srcdir)/src/share-lib/net/shnet_accept.c \
	$(top_srcdir)/src/share-lib/net/shnet_select.c \
	$(top_srcdir)/src/share-lib/net/shnet_write.c \
	$(top_srcdir)/src/share-lib/test/CuTest.c

lib_LIBRARIES = libshare.a
libshare_a_LIBADD = ../gnu/libgnu.a 
libshare_a_SOURCES = $(common_INCLUDES) $(common_sources) \
	$(top_srcdir)/src/share-lib/test/shtest.c
libshare_a_CFLAGS = $(AM_CFLAGS) \
	-DSHARELIB=1 \
	-DXD3_POSIX=1 \
	-DUSE_POSIX_FILES=1 \
	-DXD3_USE_LARGEFILE64=1 \
	-DGENERIC_ENODE_TABLES=0 \
	-I$(srcdir)/net -I$(srcdir)/fs -I$(srcdir)/mem

lib_LTLIBRARIES = libshare.la
libshare_la_LIBADD = ../gnu/libgnu.a
libshare_la_SOURCES = $(common_INCLUDES) $(common_sources) \
	$(top_srcdir)/src/share-lib/test/shtest.c
libshare_la_CFLAGS = $(AM_CFLAGS) \
	-DSHARELIB=1 \
	-DXD3_POSIX=1 \
	-DUSE_POSIX_FILES=1 \
	-DXD3_USE_LARGEFILE64=1 \
	-DGENERIC_ENODE_TABLES=0 \
	-I$(srcdir)/net -I$(srcdir)/fs -I$(srcdir)/mem

#share_a_LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)
#libshare_a_LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)
#share_LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)

if DEBUG
AM_CFLAGS = -g3 -O0 -DDEBUG
AM_CXXFLAGS = -g3 -O0 -DDEBUG
else
AM_CFLAGS = \
  -fexpensive-optimizations -ffast-math \
  -fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops
AM_CXXFLAGS = \
  -fexpensive-optimizations -ffast-math \
  -fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops
endif

CLEANFILES = $(top_srcdir)/src/share-lib/test/shtest.c 

$(top_srcdir)/src/share-lib/test/shtest.c: $(common_sources) 
	@mkdir -p $(top_srcdir)/src/share-lib/test/
	@echo "Generating 'test/shtest.c' for test suite."
	$(top_srcdir)/scripts/make-tests.sh $(top_srcdir)/src/share-lib/*.c $(top_srcdir)/src/share-lib/*/*.c > $(top_srcdir)/src/share-lib/test/shtest.c

clean-local:
	rm -f $(top_srcdir)/src/share-lib/test/shtest.c


