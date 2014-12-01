AUTOMAKE_OPTIONS = foreign
CFLAGS =
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

noinst_PROGRAMS = shkeystore shpeer shproc_lock

#	shfs_inode_remote_copy \
#	shfs_inode_remote_link

shkeystore_SOURCES = $(common_SOURCES) shkeystore.c
shkeystore_LDADD = $(top_builddir)/src/share-lib/libshare.a

shpeer_SOURCES = $(common_SOURCES) shpeer.c
shpeer_LDADD = $(top_builddir)/src/share-lib/libshare.a

shproc_lock_SOURCES = $(common_SOURCES) shproc_lock.c
shproc_lock_LDADD = $(top_builddir)/src/share-lib/libshare.a

#shfs_inode_remote_copy_SOURCES = $(common_SOURCES) shfs_inode_remote_copy.c
#shfs_inode_remote_copy_LDADD = $(top_builddir)/src/share-lib/libshare.a
#
#shfs_inode_remote_link_SOURCES = $(common_SOURCES) shfs_inode_remote_link.c
#shfs_inode_remote_link_LDADD = $(top_builddir)/src/share-lib/libshare.a


AM_CFLAGS = -g3 -O0 \
  -I$(top_srcdir)/src/share-lib \
  -Isrc/share-lib \
	-DSHARELIB \
	-I$(top_srcdir)/src/share-lib/net \
	-I$(top_srcdir)/src/share-lib/fs \
	-I$(top_srcdir)/src/share-lib/mem
AM_CXXFLAGS = -g3 -O0

