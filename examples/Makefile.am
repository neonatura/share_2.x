AUTOMAKE_OPTIONS = foreign
CFLAGS = -DSHARELIB -g \
	-I$(top_srcdir)/src/share-lib \
	-I$(top_srcdir)/src/share-lib/mem \
	-I$(top_srcdir)/src/share-lib/net \
	-I$(top_srcdir)/src/share-lib/fs \
	-I$(top_srcdir)/src/sexe \
	-I$(top_builddir)/src/gnu \
	-I$(top_srcdir)/src/gnu

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

noinst_PROGRAMS = shkeystore shpeer shproc_lock card_login esl_client esl_server

#	shfs_inode_remote_copy \
#	shfs_inode_remote_link

shkeystore_SOURCES = $(common_SOURCES) shkeystore.c
shkeystore_LDADD = $(top_builddir)/src/share-lib/libshare.a
#$(top_builddir)/src/gnu/libgnu.a -lrt

shpeer_SOURCES = $(common_SOURCES) shpeer.c
shpeer_LDADD = $(top_builddir)/src/share-lib/libshare.a
#	$(top_builddir)/src/gnu/libgnu.a -lrt

shproc_lock_SOURCES = $(common_SOURCES) shproc_lock.c
shproc_lock_LDADD = $(top_builddir)/src/share-lib/libshare.a
#	$(top_builddir)/src/gnu/libgnu.a -lrt

card_login_SOURCES = $(common_SOURCES) card_login.c
card_login_LDADD = $(top_builddir)/src/share-lib/libshare.a
#	$(top_builddir)/src/gnu/libgnu.a -lrt
card_login_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src/share-daemon/bits

esl_client_SOURCES = esl/esl_client.c
esl_client_LDADD = $(top_builddir)/src/share-lib/libshare.a
esl_client_CFLAGS = -I$(top_srcdir)/include 

esl_server_SOURCES = esl/esl_server.c
esl_server_LDADD = $(top_builddir)/src/share-lib/libshare.a
esl_server_CFLAGS = -I$(top_srcdir)/include 

