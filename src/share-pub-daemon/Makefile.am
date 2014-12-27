
common_headers = \
	pub_server.h

common_sources = \
	pub_server.c

bin_PROGRAMS = shpubd

shpubd_SOURCES = $(common_headers) $(common_sources)
shpubd_LDADD = $(top_builddir)/src/share-lib/libshare.la
shpubd_CFLAGS = -DSHARELIB -DSHARE_PUB_DAEMON

