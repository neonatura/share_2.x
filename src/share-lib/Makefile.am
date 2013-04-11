

AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)


#noinst_HEADERS = arith.h common.h tar.h


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



common_INCLUDES = \
	$(top_builddir)/config.h \
	share.h \
	sherr.h \
	share_macro.h \
	shpeer.h \
	shtime.h \
	shmem.h \
	mem/shmem.h \
	mem/shmem_buf.h \
	mem/shmem_pool.h \
	mem/shmem_meta.h \
	mem/shmem_crypt.h \
	mem/shmem_key.h \
	mem/shmem_lock.h \
	delta/xdelta3.h \
	delta/xdelta3-internal.h \
	delta/xdelta3-list.h \
	delta/xdelta3-main.h \
	delta/xdelta3-test.h \
	shcrc.h \
	fs/shfs.h \
	fs/shfs_meta.h \
	fs/shfs_journal.h \
	fs/shfs_read.h \
	fs/shfs_write.h \
	fs/shfs_partition.h \
	fs/shfs_inode.h \
	fs/shfs_dir.h \
	shpref.h \
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
	$(top_srcdir)/src/share-lib/net/shnet_write.h \
	test/CuTest.h \
	test/shtest.h

common_sources = \
	share.c \
	mem/shmem.c \
	mem/shmem_buf.c \
	mem/shmem_pool.c \
	mem/shmem_meta.c \
	mem/shmem_crypt.c \
	mem/shmem_key.c \
	mem/shmem_lock.c \
	fs/shfs_read.c \
	fs/shfs.c \
	fs/shfs_meta.c \
	fs/shfs_journal.c \
	fs/shfs_write.c \
	fs/shfs_proc.c \
	fs/shfs_rev.c \
	fs/shfs_partition.c \
	fs/shfs_inode.c \
	fs/shfs_dir.c \
	delta/xdelta3.c \
	$(top_srcdir)/src/share-lib/net/shnet.c \
	$(top_srcdir)/src/share-lib/net/shnet_write.c \
	$(top_srcdir)/src/share-lib/net/shnet_read.c \
	$(top_srcdir)/src/share-lib/net/shnet_bind.c \
	$(top_srcdir)/src/share-lib/net/shnet_connect.c \
	$(top_srcdir)/src/share-lib/net/shnet_fcntl.c \
	$(top_srcdir)/src/share-lib/net/shnet_gethost.c \
	$(top_srcdir)/src/share-lib/net/shnet_close.c \
	$(top_srcdir)/src/share-lib/net/shnet_socket.c \
	$(top_srcdir)/src/share-lib/net/shnet_accept.c \
	$(top_srcdir)/src/share-lib/net/shnet_write.c \
	test/CuTest.c \
	$(top_srcdir)/src/java-share-lib/jshare_wrap.c


lib_LIBRARIES = libshare.a
libshare_a_LIBADD = ../gnu/libgnu.a
libshare_a_SOURCES = $(common_INCLUDES) $(common_sources) \
	$(top_srcdir)/src/share-lib/test/shtest.c
libshare_a_CFLAGS = $(AM_CFLAGS) \
	-D_DEBUG=1 \
	-DXD3_DEBUG=1 \
	-DXD3_POSIX=1 \
	-DXD3_USE_LARGEFILE64=1 \
	-DGENERIC_ENODE_TABLES=0

lib_LTLIBRARIES = libshare.la
libshare_la_LIBADD = ../gnu/libgnu.a
libshare_la_SOURCES = $(common_INCLUDES) $(common_sources) \
	$(top_srcdir)/src/share-lib/test/shtest.c
libshare_la_CFLAGS = $(AM_CFLAGS) \
	-D_DEBUG=1 \
	-DXD3_POSIX=1 \
	-DXD3_USE_LARGEFILE64=1 \
	-DGENERIC_ENODE_TABLES=0

#share_a_LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)
#libshare_a_LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)
#share_LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)

if DEBUG
AM_CFLAGS = -g3 -O0
AM_CXXFLAGS = -g3 -O0
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
	$(top_srcdir)/scripts/make-tests.sh $(top_srcdir)/src/share-lib/*/*.c > $(top_srcdir)/src/share-lib/test/shtest.c

clean-local:
	rm -f $(top_srcdir)/src/share-lib/test/shtest.c

