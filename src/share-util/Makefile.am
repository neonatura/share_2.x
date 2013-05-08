
AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS =

common_INCLUDES = \
	$(top_builddir)/config.h \
	$(top_srcdir)/src/share-lib/share.h \
	$(top_srcdir)/src/share-lib/mem/shmem_buf.h \
	$(top_srcdir)/src/share-lib/mem/shmem_pool.h \
	$(top_srcdir)/src/share-lib/mem/shmem_meta.h \
	$(top_srcdir)/src/share-lib/mem/shmem_crypt.h \
	$(top_srcdir)/src/share-lib/mem/shmem_key.h \
	$(top_srcdir)/src/share-lib/mem/shmem_lock.h \
	$(top_srcdir)/src/share-lib/sherr.h \
	$(top_srcdir)/src/share-lib/share_macro.h \
	$(top_srcdir)/src/share-lib/shpeer.h \
	$(top_srcdir)/src/share-lib/shtime.h \
	$(top_srcdir)/src/share-lib/shpref.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/shfile.h \
	$(top_srcdir)/src/share-lib/fs/shfs.h \
	$(top_srcdir)/src/share-lib/fs/shfs_partition.h \
	$(top_srcdir)/src/share-lib/fs/shfs_inode.h \
	$(top_srcdir)/src/share-lib/fs/shfs_link.h \
	$(top_srcdir)/src/share-lib/fs/shfs_dir.h \
	$(top_srcdir)/src/share-lib/fs/shfs_file.h \
	$(top_srcdir)/src/share-lib/fs/shfs_cache.h \
	$(top_srcdir)/src/share-lib/fs/shfs_read.h \
	$(top_srcdir)/src/share-lib/fs/shfs_meta.h \
	$(top_srcdir)/src/share-lib/fs/shfs_write.h \
	$(top_srcdir)/src/share-lib/fs/shfs_proc.h \
	$(top_srcdir)/src/share-lib/fs/shfs_rev.h \
	$(top_srcdir)/src/share-lib/fs/shfs_aux.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3.h \
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
	sharetool.h \
	sharetool_server.h \
	sharetool_file.h \
	sharetool_test.h \
	sharetool_ping.h \
	sharetool_scan.h

common_SOURCES = \
	sharetool.c \
	sharetool_test.c \
	sharetool_server.c \
	sharetool_ping.c \
	sharetool_scan.c \
	sharetool_file.c

bin_PROGRAMS = share

share_SOURCES = $(common_INCLUDES) $(common_SOURCES)
share_LDADD = $(top_builddir)/src/share-lib/libshare.a

if DEBUG
AM_CFLAGS = -g3 -O0 \
	-I$(top_srcdir)/src/share-lib \
	-Isrc/share-lib 
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops \
	-I$(top_srcdir)/src/share-lib \
	-Isrc/share-lib 
AM_CXXFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops
endif

#man1_MANS = share.1
#
#EXTRA_DIST = \
#	shcat

##	testing/share-test.py \
##	share.1 \
##	share.i \
##	share.vcxproj \
##	share.wxi \
##	share.wxs
#
#
