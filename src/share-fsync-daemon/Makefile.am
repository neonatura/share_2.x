
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

sbin_PROGRAMS = shfsyncd
noinst_PROGRAMS = shfsyncd_static

shfsyncd_SOURCES = $(common_headers) $(common_sources)
shfsyncd_LDADD = $(top_builddir)/src/share-lib/libshare.la
shfsyncd_CFLAGS = -DSHARE_PUB_DAEMON \
								-I$(top_srcdir)/src/share-lib \
								-I$(top_srcdir)/src/sexe \
								-I$(top_srcdir)/src/share-daemon/bits \
								-lcrypt -O3

shfsyncd_static_SOURCES = $(common_headers) $(common_sources)
shfsyncd_static_LDADD = $(top_builddir)/src/share-lib/libshare.a
shfsyncd_static_CFLAGS = -DSHARE_PUB_DAEMON \
								-I$(top_srcdir)/src/share-lib \
								-I$(top_srcdir)/src/sexe \
								-I$(top_srcdir)/src/share-daemon/bits \
								-lcrypt -g

