

include_HEADERS =
	share.h \
	sherr.h \
	shpeer.h \
	shtime.h \
	shcrc.h \
	shpref.h \
	shmem.h \
	shfs.h \
	shnet.h

share.h: .include-stamp
sherr.h: .include-stamp
shpeer.h: .include-stamp
shtime.h: .include-stamp
shpref.h: .include-stamp
shcrc.h: .include-stamp
shmem.h: .include-stamp
shfs.h: .include-stamp
shnet.h: .include-stamp


.include-stamp:
	ln -sf $(top_srcdir)/src/share-lib/share.h
	ln -sf $(top_srcdir)/src/share-lib/sherr.h
	ln -sf $(top_srcdir)/src/share-lib/shpeer.h
	ln -sf $(top_srcdir)/src/share-lib/shtime.h
	ln -sf $(top_srcdir)/src/share-lib/shcrc.h
	ln -sf $(top_srcdir)/src/share-lib/shpref.h
	ln -sf $(top_srcdir)/src/share-lib/mem/shmem.h
	ln -sf $(top_srcdir)/src/share-lib/fs/shfs.h
	ln -sf $(top_srcdir)/src/share-lib/net/shnet.h
	echo Timestamp > .include-stamp

clean-local: 
	rm -f $(noinst_INCLUDES) .include-stamp

all-local: .include-stamp
