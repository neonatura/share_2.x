

noinst_PROGRAMS = \
	libshare.a libshare.so libshare.so.2 \
	libshare.jar shared \
	shcat shls shrm shmkdir shcp shin shout shpref shping \
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


shared:
	ln -sf $(top_builddir)/src/share-daemon/shared

shcat: $(top_builddir)/src/share-util/shcat
	ln -sf $(top_builddir)/src/share-util/shcat

shin: $(top_builddir)/src/share-util/shin
	ln -sf $(top_builddir)/src/share-util/shin

shout: $(top_builddir)/src/share-util/shout
	ln -sf $(top_builddir)/src/share-util/shout

shls: $(top_builddir)/src/share-util/shls
	ln -sf $(top_builddir)/src/share-util/shls

shpref: $(top_builddir)/src/share-util/shpref
	ln -sf $(top_builddir)/src/share-util/shpref

shrm: $(top_builddir)/src/share-util/shrm
	ln -sf $(top_builddir)/src/share-util/shrm

shmkdir: $(top_builddir)/src/share-util/shmkdir
	ln -sf $(top_builddir)/src/share-util/shmkdir

shcp: $(top_builddir)/src/share-util/shcp
	ln -sf $(top_builddir)/src/share-util/shcp

shping: $(top_builddir)/src/share-util/shping
	ln -sf $(top_builddir)/src/share-util/shping

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

install-exec:
	@echo ""
	@echo "The  libshare library and it's components has been installed."
	@echo ""
	@echo "It is now safe to perform a 'make check'."
	@echo ""
