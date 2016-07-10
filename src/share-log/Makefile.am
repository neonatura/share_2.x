AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = shlog
noinst_LIBRARIES = libshlogd.a

dep_libs =
if HAVE_LIBGMP
dep_libs += -lgmp
endif

shlog_SOURCES = $(common_INCLUDES) $(common_SOURCES) sharelog.c
shlog_LDADD = $(top_builddir)/src/share-lib/libshare.la
shlog_LDFLAGS = $(dep_libs)

libshlogd_a_SOURCES = $(common_INCLUDES) $(common_SOURCES) $(server_SOURCES) 
libshlogd_a_LIBADD = $(top_builddir)/src/share-lib/libshare.a
libshlogd_a_LDFLAGS = $(dep_libs)

common_INCLUDES = \
	$(top_builddir)/config.h \
	$(top_srcdir)/src/share-lib/share.h \
	$(top_srcdir)/src/share-lib/mem/shmem.h \
	$(top_srcdir)/src/share-lib/sherr.h \
	$(top_srcdir)/src/share-lib/share_base.h \
	$(top_srcdir)/src/share-lib/shpeer.h \
	$(top_srcdir)/src/share-lib/shtime.h \
	$(top_srcdir)/src/share-lib/shpref.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/shfile.h \
	$(top_srcdir)/src/share-lib/fs/shfs.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3.h \
	$(top_srcdir)/src/share-lib/net/shnet.h \
	sharelog.h \
	sharelog_file.h \
	sharelog_test.h \
	sharelog_pref.h \
	sharelog_scan.h

common_SOURCES = \
	sharelog_test.c \
	sharelog_pref.c \
	sharelog_scan.c \
	sharelog_file.c

server_SOURCES = \
	sharelog_server.h \
	sharelog_server.c \
	server/shlogd_daemon.c \
	server/shlogd_task.c \
	server/shlogd_msg.c

AM_CFLAGS = -g \
	-I$(top_srcdir)/src/share-lib
