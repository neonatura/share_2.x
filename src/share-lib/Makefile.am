AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) -version-info @version_info@

#LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD) $(LIB_PTHREAD) -version-info @version_info@

noinst_PROGRAMS = test_libshare test_libshare_static test_xd3

#test_libshare
test_libshare_SOURCES = $(common_INCLUDES) test_libshare.c
test_libshare_LDADD = libshare.la

# test_libshare_static
test_libshare_static_SOURCES = $(common_INCLUDES) test_libshare.c
test_libshare_static_LDFLAGS = -static
test_libshare_static_LDADD = libshare.a

test_xd3_SOURCES = $(srcdir)/mem/delta/shxd_test.c $(xd3_headers)
test_xd3_CFLAGS = -DREGRESSION_TEST -DSHELL_TESTS=0 \
	$(XD3_CFLAGS)


TESTS = test_libshare_static test_libshare test_xd3

if LINUX
install-exec-hook:
	mkdir -p /var/lib/share
	chmod 0777 /var/lib/share
endif

arch_sources = \
	$(srcdir)/fs/arch/arith.h \
	$(srcdir)/fs/arch/buffer.c \
	$(srcdir)/fs/arch/common.h \
	$(srcdir)/fs/arch/create.c \
	$(srcdir)/fs/arch/extract.c \
	$(srcdir)/fs/arch/incremen.c \
	$(srcdir)/fs/arch/list.c \
	$(srcdir)/fs/arch/misc.c \
	$(srcdir)/fs/arch/names.c \
	$(srcdir)/fs/arch/paxlib.h \
	$(srcdir)/fs/arch/paxnames.c \
	$(srcdir)/fs/arch/suffix.c \
	$(srcdir)/fs/arch/system.c \
	$(srcdir)/fs/arch/system.h \
	$(srcdir)/fs/arch/system-ioctl.h \
	$(srcdir)/fs/arch/tar.h \
	$(srcdir)/fs/arch/transform.c \
	$(srcdir)/fs/arch/update.c \
	$(srcdir)/fs/arch/utf8.c \
	$(srcdir)/fs/arch/wordsplit.c \
	$(srcdir)/fs/arch/wordsplit.h

noinst_INCLUDES = \
	share.h sherr.h shtime.h shcrc.h shpeer.h shpref.h \
	shnet.h shfs.h shmem.h shsys.h

zlib_sources = \
	mem/zlib/adler32.c \
	mem/zlib/crc32.c \
	mem/zlib/deflate.c \
	mem/zlib/infback.c \
	mem/zlib/inffast.c \
	mem/zlib/inflate.c \
	mem/zlib/inftrees.c \
	mem/zlib/trees.c \
	mem/zlib/zutil.c \
	mem/zlib/compress.c \
	mem/zlib/uncompr.c

crypt_sources = \
	sys/crypt/crypt-sha256.c \
	sys/crypt/crypt-sha512.c \
	sys/crypt/crypt-misc.c \
	sys/crypt/crypt.h

common_INCLUDES = \
	$(noinst_INCLUDES) \
	$(top_builddir)/config.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/shpref.h \
	$(top_srcdir)/src/share-lib/fs/shfs_int.h \
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
	$(top_srcdir)/src/share-lib/mem/shmem_zlib.c \
	$(top_srcdir)/src/share-lib/mem/shmem_delta.c \
	$(top_srcdir)/src/share-lib/mem/shmem_diff.c \
	$(top_srcdir)/src/share-lib/fs/shfs_mem.c \
	$(top_srcdir)/src/share-lib/fs/shfs_meta.c \
	$(top_srcdir)/src/share-lib/fs/shfs_journal.c \
	$(top_srcdir)/src/share-lib/fs/shfs_proc.c \
	$(top_srcdir)/src/share-lib/fs/shfs_rev.c \
	$(top_srcdir)/src/share-lib/fs/shfs_partition.c \
	$(top_srcdir)/src/share-lib/fs/shfs_inode.c \
	$(top_srcdir)/src/share-lib/fs/shfs_link.c \
	$(top_srcdir)/src/share-lib/fs/shfs_dir.c \
	$(top_srcdir)/src/share-lib/fs/shfs_file.c \
	$(top_srcdir)/src/share-lib/fs/shfs_bin.c \
	$(top_srcdir)/src/share-lib/fs/shfs_zlib.c \
	$(top_srcdir)/src/share-lib/fs/shfs_ext.c \
	$(top_srcdir)/src/share-lib/fs/shfs_cache.c \
	$(top_srcdir)/src/share-lib/fs/shfs_aux.c \
	$(top_srcdir)/src/share-lib/fs/shfs_attr.c \
	$(top_srcdir)/src/share-lib/fs/shfs_ref.c \
	$(top_srcdir)/src/share-lib/fs/shfs_obj.c \
	$(top_srcdir)/src/share-lib/fs/shfs_home.c \
	$(top_srcdir)/src/share-lib/fs/shfs_access.c \
	$(top_srcdir)/src/share-lib/fs/shfs_arch.c \
	$(top_srcdir)/src/share-lib/fs/shfs_stream.c \
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
	$(top_srcdir)/src/share-lib/sys/shsys_msg.c \
	$(top_srcdir)/src/share-lib/sys/shsys_app.c \
	$(top_srcdir)/src/share-lib/sys/shsys_proc.c \
	$(top_srcdir)/src/share-lib/sys/shsys_crypt.c \
	$(top_srcdir)/src/share-lib/sys/shsys_pam.c \
	$(top_srcdir)/src/share-lib/sys/shsys_pam_shadow.c \
	$(top_srcdir)/src/share-lib/sys/shsys_log.c \
	$(top_srcdir)/src/share-lib/test/CuTest.c

xd3_headers = \
	$(srcdir)/mem/delta/xdelta3-blkcache.h \
	$(srcdir)/mem/delta/xdelta3-cfgs.h \
	$(srcdir)/mem/delta/xdelta3-decode.h \
	$(srcdir)/mem/delta/xdelta3-djw.h \
	$(srcdir)/mem/delta/xdelta3-fgk.h \
	$(srcdir)/mem/delta/xdelta3.h \
	$(srcdir)/mem/delta/xdelta3-hash.h \
	$(srcdir)/mem/delta/xdelta3-internal.h \
	$(srcdir)/mem/delta/xdelta3-list.h \
	$(srcdir)/mem/delta/xdelta3-lzma.h \
	$(srcdir)/mem/delta/xdelta3-main.h \
	$(srcdir)/mem/delta/xdelta3-merge.h \
	$(srcdir)/mem/delta/xdelta3-second.h \
	$(srcdir)/mem/delta/xdelta3-test.h

XD3_CFLAGS = \
	-DXD3_POSIX=1 \
	-DUSE_POSIX_FILES=1 \
	-DGENERIC_ENODE_TABLES=0


lib_LIBRARIES = libshare.a
libshare_a_LIBADD = $(top_builddir)/src/gnu/libgnu.a
libshare_a_SOURCES = $(common_INCLUDES) $(common_sources) $(zlib_sources) \
	$(crypt_sources) \
	$(arch_sources) $(top_srcdir)/src/share-lib/test/shtest.c
libshare_a_CFLAGS = $(AM_CFLAGS) \
	-DHAVE_CONFIG_H -DHAVE_HIDDEN -DZ_PREFIX \
	-DSHARELIB=1 \
	-DGENERIC_ENODE_TABLES=0 \
	-I$(srcdir)/net -I$(srcdir)/fs -I$(srcdir)/mem \
	-I$(srcdir)/mem/zlib -I$(srcdir)/fs/arch \
	-I$(top_srcdir)/src/gnu -I$(top_builddir)/src/gnu \
	$(XD3_CFLAGS)
#libshare_a_LDFLAGS =  $(LIB_ACL) $(LIB_CLOCK_GETTIME) $(LIBSOCKET)

lib_LTLIBRARIES = libshare.la
libshare_la_LIBADD = $(top_builddir)/src/gnu/libgnu.la
libshare_la_SOURCES = $(common_INCLUDES) $(common_sources) \
	$(srcdir)/java/libshare_java_wrap.c \
	$(zlib_sources) \
	$(crypt_sources) \
	$(arch_sources) $(top_srcdir)/src/share-lib/test/shtest.c
libshare_la_CFLAGS = $(AM_CFLAGS) \
	-DHAVE_CONFIG_H -DHAVE_HIDDEN -DZ_PREFIX \
	-DSHARELIB=1 \
	-DGENERIC_ENODE_TABLES=0 \
	-I$(srcdir)/net -I$(srcdir)/fs -I$(srcdir)/mem \
	-I$(srcdir)/mem/zlib -I$(srcdir)/fs/arch \
	-I$(top_srcdir)/src/gnu -I$(top_builddir)/src/gnu \
	$(XD3_CFLAGS)
#libshare_la_LDFLAGS =  $(LIB_ACL) $(LIB_CLOCK_GETTIME) $(LIBSOCKET)

#share_a_LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)
#libshare_a_LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)
#share_LDFLAGS = $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL) $(LTLIBTHREAD)

if DEBUG
AM_CFLAGS = -g3 -O0 -DDEBUG -Wno-discarded-qualifiers
AM_CXXFLAGS = -g3 -O0 -DDEBUG
else
AM_CFLAGS = \
  -fexpensive-optimizations -ffast-math \
  -fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops \
	-Wno-discarded-qualifiers
AM_CXXFLAGS = \
  -fexpensive-optimizations -ffast-math \
  -fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops
endif

CLEANFILES = $(top_srcdir)/src/share-lib/test/shtest.c 

$(top_srcdir)/src/share-lib/test/shtest.c: $(common_sources) 
	@mkdir -p $(top_srcdir)/src/share-lib/test/
	@echo "Generating 'test/shtest.c' for test suite."
	$(top_srcdir)/scripts/make-tests.sh $(top_srcdir)/src/share-lib/*.c $(top_srcdir)/src/share-lib/sys/*.c $(top_srcdir)/src/share-lib/mem/*.c $(top_srcdir)/src/share-lib/fs/*.c $(top_srcdir)/src/share-lib/net/*.c > $(top_srcdir)/src/share-lib/test/shtest.c

$(top_builddir)/src/gnu/libgnu.a: $(top_builddir)/src/gnu/.libs/libgnu.a
	$(LN_S) $(top_builddir)/src/gnu/.libs/libgnu.a $(top_builddir)/src/gnu/

clean-local:
	rm -f $(top_srcdir)/src/share-lib/test/shtest.c


