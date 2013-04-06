
AUTOMAKE_OPTIONS = foreign
CFLAGS =
LDFLAGS =

common_INCLUDES = \
  $(top_builddir)/src/share-lib/share.h \
  $(top_srcdir)/src/share-lib/key/shkey.h \
  $(top_srcdir)/src/share-lib/share_version.h \
  $(top_srcdir)/src/share-lib/pref/shpref.h \
  $(top_srcdir)/src/share-lib/fs/shfs_adler.h \
  $(top_srcdir)/src/share-lib/fs/shfs.h \
  $(top_srcdir)/src/share-lib/fs/shfs_read.h \
  $(top_srcdir)/src/share-lib/fs/shfs_meta.h \
  $(top_srcdir)/src/share-lib/fs/shfs_time64.h \
  $(top_srcdir)/src/share-lib/fs/shfs_write.h \
  $(top_srcdir)/src/share-lib/fs/shfs_proc.h \
  $(top_srcdir)/src/share-lib/delta/xdelta3.h \
  $(top_srcdir)/src/share-lib/socket/write.h \
  $(top_srcdir)/src/share-lib/socket/read.h \
  $(top_srcdir)/src/share-lib/socket/bind.h \
  $(top_srcdir)/src/share-lib/socket/connect.h \
  $(top_srcdir)/src/share-lib/socket/shfcntl.h \
  $(top_srcdir)/src/share-lib/socket/gethost.h \
  $(top_srcdir)/src/share-lib/socket/close.h \
  $(top_srcdir)/src/share-lib/socket/socket.h \
  $(top_srcdir)/src/share-lib/socket/sockbuff.h \
  $(top_srcdir)/src/share-lib/socket/accept.h \
  $(top_srcdir)/src/share-lib/buffer/shbuf.h \
  $(top_srcdir)/src/share-lib/meta/shmeta.h \
  $(top_srcdir)/src/share-lib/meta/shmeta_value.h

noinst_PROGRAMS = test_libshare
#bin_PROGRAMS = test_libshare
test_libshare_SOURCES = $(common_SOURCES) test_libshare.c
test_libshare_LDADD = $(top_builddir)/src/share-lib/libshare.a

AM_CFLAGS = -g3 -O0 \
  -I$(top_srcdir)/src/share-lib \
  -Isrc/share-lib
AM_CXXFLAGS = -g3 -O0


