
common_headers = \
	pub_server.h \
	pub_user.h \
	pub_cycle.h \
	pub_scan.h \
	pub_file.h

common_sources = \
	pub_server.c \
	pub_user.c \
	pub_cycle.c \
	pub_scan.c \
	pub_file.c

bin_PROGRAMS = shpubd
noinst_PROGRAMS = shpubd_static

shpubd_SOURCES = $(common_headers) $(common_sources)
shpubd_LDADD = $(top_builddir)/src/share-lib/libshare.la
shpubd_CFLAGS = -DSHARELIB -DSHARE_PUB_DAEMON \
								-I$(top_srcdir)/src/share-lib \
								-I$(top_srcdir)/src/share-daemon/bits \
								-lcrypt -O3

shpubd_static_SOURCES = $(common_headers) $(common_sources)
shpubd_static_LDADD = $(top_builddir)/src/share-lib/libshare.a
shpubd_static_CFLAGS = -DSHARELIB -DSHARE_PUB_DAEMON \
								-I$(top_srcdir)/src/share-lib \
								-I$(top_srcdir)/src/share-daemon/bits \
								-lcrypt -g

