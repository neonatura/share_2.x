
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
	sharedaemon.h \
	sharedaemon_server.h \
	sharedaemon_file.h \
	sharedaemon_test.h \
	sharedaemon_ping.h \
	sharedaemon_scan.h \
	bits/bits.h \
	bits/transaction.h \
	bits/event.h \
	bits/identity.h \
	bits/signature.h \
	bits/trust.h \
	bits/account.h \
	bits/ledger.h \
	bits/schedule.h \
	bits/ward.h

common_SOURCES = \
	sharedaemon.c \
	sharedaemon_test.c \
	sharedaemon_server.c \
	sharedaemon_ping.c \
	sharedaemon_scan.c \
	sharedaemon_file.c \
	bits/transaction.c \
	bits/event.c \
	bits/identity.c \
	bits/signature.c \
	bits/trust.c \
	bits/account.c \
	bits/ledger.c \
	bits/schedule.c \
	bits/ward.c

bin_PROGRAMS = shared

shared_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shared_LDADD = $(top_builddir)/src/share-lib/libshare.a

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
