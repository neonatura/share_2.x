

noinst_PROGRAMS = \
	libshare.a libshare.so libshare.so.2 \
	libshare.jar share shared \
	shcat shls shrm shmkdir shcp shin shpref \
	docs man \
	.notice

if HAVE_LIBSHARE_LOG_DAEMON
noinst_PROGRAMS += shlog shlogd

shlogd:
	ln -sf $(top_builddir)/src/share-log/shlogd

shlog:
	ln -sf $(top_builddir)/src/share-log/shlog
endif

if HAVE_LIBSHARE_COIN_DAEMON
noinst_PROGRAMS += shcoin shcoind

shcoind:
	ln -sf $(top_builddir)/src/share-coin/shcoind

shcoin:
	ln -sf $(top_builddir)/src/share-coin/shcoin
endif


libshare.a:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.a 

libshare.so:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so

libshare.so.2:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so.2

libshare.jar:
	ln -sf $(top_builddir)/src/java-share-lib/libshare.jar

share:
	ln -sf $(top_builddir)/src/share-util/share

shared:
	ln -sf $(top_builddir)/src/share-daemon/shared

shcat:
	ln -sf $(top_builddir)/scripts/shcat

shin:
	ln -sf $(top_builddir)/scripts/shin

shls:
	ln -sf $(top_builddir)/scripts/shls

shpref:
	ln -sf $(top_builddir)/scripts/shpref

shrm:
	ln -sf $(top_builddir)/scripts/shrm

shmkdir:
	ln -sf $(top_builddir)/scripts/shmkdir

shcp:
	ln -sf $(top_builddir)/scripts/shcp

shping:
	ln -sf $(top_builddir)/scripts/shping

docs:
	ln -sf $(top_builddir)/doc/html ./docs

man:
	ln -sf $(top_builddir)/doc/man


.notice:
	@echo > .notice
	@echo ""
	@echo "The  libshare library and it's components has been built."
	@echo ""
	@echo "It is now safe to perform a 'make install'."
	@echo ""
	@echo "Check out the 'bin/' directory for documentation and executables."
	@echo "Documentation can be found at: file://$(PWD)/docs/index.html"
	@echo ""


