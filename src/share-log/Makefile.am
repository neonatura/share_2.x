AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS =

if HAVE_LIBSHARE_LOG_DAEMON

bin_PROGRAMS = shlog
sbin_PROGRAMS = shlogd

shlog_SOURCES = $(common_INCLUDES) $(common_SOURCES) sharelog.c
shlog_LDADD = $(top_builddir)/src/share-lib/libshare.la

shlogd_SOURCES = $(common_INCLUDES) $(common_SOURCES) $(server_SOURCES)
shlogd_LDADD = $(top_builddir)/src/share-lib/libshare.la

noinst_LIBRARIES = shlog_daemon.a
shlog_daemon_a_SOURCES = $(common_INCLUDES) $(common_SOURCES) $(server_SOURCES) 
shlog_daemon_a_LDFLAGS = -DSHLOGD_LIB

if HAVE_INITD_DIR

EXTRA_DIST = $(top_builddir)/scripts/initd/shlogd

install-exec-hook:
	rm -f $(INITD_DIR)/init.d/shlogd	
	rm -f $(INITD_DIR)/rc3.d/S86shlogd	
	rm -f $(INITD_DIR)/rc3.d/K86shlogd	
	rm -f $(INITD_DIR)/rc4.d/S86shlogd	
	rm -f $(INITD_DIR)/rc4.d/K86shlogd	
	rm -f $(INITD_DIR)/rc5.d/S86shlogd	
	rm -f $(INITD_DIR)/rc5.d/K86shlogd	
	cp $(top_builddir)/scripts/initd/shlogd $(INITD_DIR)/init.d/
	cd $(INITD_DIR)/rc3.d && $(LN_S) ../init.d/shlogd S86shlogd
	cd $(INITD_DIR)/rc3.d && $(LN_S) ../init.d/shlogd K86shlogd
	cd $(INITD_DIR)/rc4.d && $(LN_S) ../init.d/shlogd S86shlogd
	cd $(INITD_DIR)/rc4.d && $(LN_S) ../init.d/shlogd K86shlogd
	cd $(INITD_DIR)/rc5.d && $(LN_S) ../init.d/shlogd S86shlogd
	cd $(INITD_DIR)/rc5.d && $(LN_S) ../init.d/shlogd K86shlogd

uninstall-hook:
	rm -f $(INITD_DIR)/init.d/shlogd	
	rm -f $(INITD_DIR)/rc3.d/S86shlogd	
	rm -f $(INITD_DIR)/rc3.d/K86shlogd	
	rm -f $(INITD_DIR)/rc4.d/S86shlogd	
	rm -f $(INITD_DIR)/rc4.d/K86shlogd	
	rm -f $(INITD_DIR)/rc5.d/S86shlogd	
	rm -f $(INITD_DIR)/rc5.d/K86shlogd	
endif

endif

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


if DEBUG
AM_CFLAGS = -g3 -O0 \
	-I$(top_srcdir)/src/share-lib \
	-DSHARELIB \
	-I$(top_srcdir)/src/share-lib/net \
	-I$(top_srcdir)/src/share-lib/fs \
	-I$(top_srcdir)/src/share-lib/mem
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops \
	-I$(top_srcdir)/src/share-lib \
	-DSHARELIB \
	-I$(top_srcdir)/src/share-lib/net \
	-I$(top_srcdir)/src/share-lib/fs \
	-I$(top_srcdir)/src/share-lib/mem
AM_CXXFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops
endif

