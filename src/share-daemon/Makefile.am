
AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS =

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
