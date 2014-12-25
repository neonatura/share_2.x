AUTOMAKE_OPTIONS = foreign
CFLAGS = -I$(top_srcdir)/src/share-lib -DSHARELIB -g
LDFLAGS =

common_INCLUDES = \
  $(top_srcdir)/src/share-lib/share.h \
  $(top_srcdir)/src/share-lib/shtime.h \
  $(top_srcdir)/src/share-lib/shkey.h \
  $(top_srcdir)/src/share-lib/shpref.h \
  $(top_srcdir)/src/share-lib/shbuf.h \
  $(top_srcdir)/src/share-lib/fs/shfs.h \
  $(top_srcdir)/src/share-lib/delta/xdelta3.h \
  $(top_srcdir)/src/share-lib/meta/shmeta.h \
  $(top_srcdir)/src/share-lib/meta/shmeta_value.h

noinst_PROGRAMS = shkeystore shpeer shproc_lock shtrace shacc

#	shfs_inode_remote_copy \
#	shfs_inode_remote_link

shkeystore_SOURCES = $(common_SOURCES) shkeystore.c
shkeystore_LDADD = $(top_builddir)/src/share-lib/libshare.a

shpeer_SOURCES = $(common_SOURCES) shpeer.c
shpeer_LDADD = $(top_builddir)/src/share-lib/libshare.a

shproc_lock_SOURCES = $(common_SOURCES) shproc_lock.c
shproc_lock_LDADD = $(top_builddir)/src/share-lib/libshare.a

shtrace_SOURCES = $(common_SOURCES) shtrace.c
shtrace_LDADD = $(top_builddir)/src/share-lib/libshare.a
shtrace_CFLAGS = -I$(top_srcdir)/src/share-daemon/bits

shacc_SOURCES = $(common_SOURCES) shacc.c
shacc_LDADD = $(top_builddir)/src/share-lib/libshare.a
shacc_CFLAGS = -I$(top_srcdir)/src/share-daemon/bits

