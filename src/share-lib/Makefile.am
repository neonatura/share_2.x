

AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS =


common_INCLUDES = \
	$(top_builddir)/config.h \
	share.h \
	share_macro.h \
	shtime.h \
	shkey.h \
	meta/shmeta.h \
	meta/shmeta_value.h \
	shbuf.h \
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
	fs/shfs_inode.h \
	shpref.h \
	socket/shsk.h \
	socket/socket.h \
	socket/connect.h \
	socket/shfcntl.h \
	socket/gethost.h \
	socket/shsk_write.h \
	test/CuTest.h \
	test/shtest.h

common_SOURCES = \
	share.c \
	fs/shfs_read.c \
	fs/shfs.c \
	fs/shfs_meta.c \
	fs/shfs_journal.c \
	fs/shfs_write.c \
	fs/shfs_proc.c \
	fs/shfs_rev.c \
	fs/shfs_inode.c \
	delta/xdelta3.c \
	socket/shsk.c \
	socket/shfcntl.c \
	socket/connect.c \
	socket/accept.c \
	socket/gethost.c \
	socket/shsk_write.c \
	socket/read.c \
	socket/socket.c \
	socket/close.c \
	socket/bind.c \
	meta/shmeta.c \
	meta/shmeta_value.c \
	test/CuTest.c

lib_LIBRARIES = libshare.a
libshare_a_SOURCES = $(common_INCLUDES) $(common_SOURCES) \
	$(top_srcdir)/src/share-lib/test/shtest.c

libshare_a_CFLAGS = $(AM_CFLAGS) \
	-DXD3_DEBUG=1 \
	-DXD3_POSIX=1 \
	-DXD3_USE_LARGEFILE64=1 \
	-DGENERIC_ENODE_TABLES=0


lib_LTLIBRARIES = libshare.la
libshare_la_SOURCES = $(common_INCLUDES) $(common_SOURCES) \
	$(top_srcdir)/src/share-lib/test/shtest.c
libshare_la_CFLAGS = $(AM_CFLAGS) \
	-DXD3_DEBUG=1 \
	-DXD3_POSIX=1 \
	-DXD3_USE_LARGEFILE64=1 \
	-DGENERIC_ENODE_TABLES=0

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

EXTRA_DIST = \
	shtime.h \
	shkey.h \
	shcrc.h \
	shpref.h \
	fs/shfs.h \
	fs/shfs_read.h \
	fs/shfs_meta.h \
	fs/shfs_time64.h \
	fs/shfs_write.h \
	fs/shfs_proc.h \
	fs/shfs_rev.h \
	delta/xdelta3.h \
	delta/xdelta3-main.h \
	delta/xdelta3-test.h \
	delta/xdelta3-merge.h \
	delta/xdelta3-cfgs.h \
	delta/xdelta3-lzma.h \
	delta/xdelta3-list.h \
	delta/xdelta3-second.h \
	delta/xdelta3-blkcache.h \
	socket/write.h \
	socket/read.h \
	socket/bind.h \
	socket/connect.h \
	socket/shfcntl.h \
	socket/gethost.h \
	socket/close.h \
	socket/socket.h \
	socket/accept.h \
	shbuf.h \
	meta/shmeta.h \
	meta/shmeta_value.h

CLEANFILES = $(top_srcdir)/src/share-lib/test/shtest.c 

$(top_srcdir)/src/share-lib/test/shtest.c: $(common_SOURCES) 
	@mkdir -p $(top_srcdir)/src/share-lib/test/
	@echo "Generating 'test/shtest.c' for test suite."
	$(top_srcdir)/scripts/make-tests.sh $(top_srcdir)/src/share-lib/*/*.c > $(top_srcdir)/src/share-lib/test/shtest.c

clean-local:
	rm -f $(top_srcdir)/src/share-lib/test/shtest.c

