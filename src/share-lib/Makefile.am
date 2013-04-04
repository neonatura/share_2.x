

AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS =


common_INCLUDES = \
	$(top_builddir)/src/share-lib/share.h \
	key/shkey.h \
	meta/shmeta.h \
	meta/shmeta_value.h \
	buffer/shbuf.h \
	delta/xdelta3.h \
	shfs/shfs_meta.h \
	shfs/shfs_read.h \
	shfs/shfs_write.h \
	shfs/shfs_adler.h \
	shfs/shfs_time64.h \
	socket/sockbuff.h \
	socket/socket.h \
	socket/connect.h \
	socket/shfcntl.h \
	socket/gethost.h

common_SOURCES = \
	share.c \
	key/shkey.c \
	fs/shfs_read.c \
	fs/shfs.c \
	fs/shfs_time64.c \
	fs/shfs_meta.c \
	fs/shfs_write.c \
	fs/shfs_proc.c \
	fs/shfs_adler.c \
	delta/xdelta3.c \
	socket/shfcntl.c \
	socket/connect.c \
	socket/accept.c \
	socket/sockbuff.c \
	socket/gethost.c \
	socket/write.c \
	socket/read.c \
	socket/socket.c \
	socket/close.c \
	socket/bind.c \
	buffer/shbuf.c \
	svn/shsvn_hash.c.orig \
	svn/shsvn.c.orig \
	meta/shmeta.c \
	meta/shmeta_value.c

lib_LIBRARIES = libshare.a
libshare_a_SOURCES = $(common_INCLUDES) $(common_SOURCES)

libshare_a_CFLAGS = $(AM_CFLAGS)

lib_LTLIBRARIES = libshare.la
libshare_la_SOURCES = $(common_INCLUDES) $(common_SOURCES)

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
	key/shkey.h \
	share_version.h \
	pref/shpref.h \
	fs/shfs_adler.h \
	fs/shfs.h \
	fs/shfs_read.h \
	fs/shfs_meta.h \
	fs/shfs_time64.h \
	fs/shfs_write.h \
	fs/shfs_proc.h \
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
	socket/sockbuff.h \
	socket/accept.h \
	buffer/shbuf.h \
	meta/shmeta.h \
	meta/shmeta_value.h
