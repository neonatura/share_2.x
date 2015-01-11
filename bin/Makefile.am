

noinst_PROGRAMS = \
	libshare.a libshare.so libshare.so.2 \
	libshare.jar shared \
	sx sxc readsexe sxsh \
	shcat shstat shls shattr shrm shrev shcp shdiff shdelta shpatch shln shar unshar shpref \
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

sx: $(top_builddir)/src/sexe/sx
	ln -sf $(top_builddir)/src/sexe/sx

sxc: $(top_builddir)/src/sexe/sxc
	ln -sf $(top_builddir)/src/sexe/sxc

readsexe: $(top_builddir)/src/sexe/readsexe
	ln -sf $(top_builddir)/src/sexe/readsexe

sxsh: $(top_builddir)/src/sexe/shell/sxsh
	ln -sf $(top_builddir)/src/sexe/shell/sxsh

shcat: $(top_builddir)/src/share-util/static/shcat
	ln -sf $(top_builddir)/src/share-util/static/shcat

shstat: $(top_builddir)/src/share-util/static/shstat
	ln -sf $(top_builddir)/src/share-util/static/shstat

shls: $(top_builddir)/src/share-util/static/shls
	ln -sf $(top_builddir)/src/share-util/static/shls

shattr: $(top_builddir)/src/share-util/static/shattr
	ln -sf $(top_builddir)/src/share-util/static/shattr

shpref: $(top_builddir)/src/share-util/static/shpref
	ln -sf $(top_builddir)/src/share-util/static/shpref

shrm: $(top_builddir)/src/share-util/static/shrm
	ln -sf $(top_builddir)/src/share-util/static/shrm

shrev: $(top_builddir)/src/share-util/static/shrev
	ln -sf $(top_builddir)/src/share-util/static/shrev

shcp: $(top_builddir)/src/share-util/static/shcp
	ln -sf $(top_builddir)/src/share-util/static/shcp

shdiff: $(top_builddir)/src/share-util/static/shdiff
	ln -sf $(top_builddir)/src/share-util/static/shdiff

shdelta: $(top_builddir)/src/share-util/static/shdelta
	ln -sf $(top_builddir)/src/share-util/static/shdelta

shpatch: $(top_builddir)/src/share-util/static/shpatch
	ln -sf $(top_builddir)/src/share-util/static/shpatch

shln: $(top_builddir)/src/share-util/static/shln
	ln -sf $(top_builddir)/src/share-util/static/shln

shar: $(top_builddir)/src/share-util/static/shar
	ln -sf $(top_builddir)/src/share-util/static/shar

unshar: $(top_builddir)/src/share-util/static/unshar
	ln -sf $(top_builddir)/src/share-util/static/unshar

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
	@echo "Perform a 'make check' for additional tests."
	@echo ""
	@echo "Check out the 'bin/' directory for documentation and executables."
	@echo "Documentation can be found at: file://$(PWD)/docs/index.html"
	@echo ""

