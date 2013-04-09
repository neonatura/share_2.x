

AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS =


common_INCLUDES = \
	$(top_builddir)/config.h \
	share.h \
	sherr.h \
	share_macro.h \
	shtime.h \
	shkey.h \
	mem/shmem.h \
	mem/shmem_buf.h \
	mem/shmem_pool.h \
	mem/shmem_meta.h \
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
	mem/shmem.c \
	mem/shmem_buf.c \
	mem/shmem_pool.c \
	mem/shmem_meta.c \
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
	test/CuTest.c \
	$(top_srcdir)/src/java-share-lib/jshare_wrap.c

lib_LIBRARIES = libshare.a
libshare_a_SOURCES = $(common_INCLUDES) $(common_SOURCES) \
	$(top_srcdir)/src/share-lib/test/shtest.c

libshare_a_CFLAGS = $(AM_CFLAGS) \
	-D_DEBUG=1 \
	-DXD3_DEBUG=1 \
	-DXD3_POSIX=1 \
	-DXD3_USE_LARGEFILE64=1 \
	-DGENERIC_ENODE_TABLES=0


lib_LTLIBRARIES = libshare.la
libshare_la_SOURCES = $(common_INCLUDES) $(common_SOURCES) \
	$(top_srcdir)/src/share-lib/test/shtest.c
libshare_la_CFLAGS = $(AM_CFLAGS) \
	-D_DEBUG=1 \
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

CLEANFILES = $(top_srcdir)/src/share-lib/test/shtest.c 

$(top_srcdir)/src/share-lib/test/shtest.c: $(common_SOURCES) 
	@mkdir -p $(top_srcdir)/src/share-lib/test/
	@echo "Generating 'test/shtest.c' for test suite."
	$(top_srcdir)/scripts/make-tests.sh $(top_srcdir)/src/share-lib/*/*.c > $(top_srcdir)/src/share-lib/test/shtest.c

clean-local:
	rm -f $(top_srcdir)/src/share-lib/test/shtest.c

