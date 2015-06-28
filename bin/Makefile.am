

noinst_PROGRAMS = \
	libshare.a libshare.so libshare.so.2 \
	libshare.jar shared \
	sx sxc readsexe sxsh \
	shattr \
	shcat \
	shcert \
	shcp \
	shdb \
	shdelta \
	shdiff \
	shfsck \
	shinfo \
	shln \
	shls \
	shpatch \
	shstat \
	shpref \
	shpasswd \
	shpkg \
	shrev \
	shrm \
	docs man \
	libshare.so.2.0.$(version_patch)

if DEBUG
else
noinst_PROGRAMS += shlog

shlog:
	ln -sf $(top_builddir)/src/share-log/shlog
endif

noinst_PROGRAMS += .notice

libshare.a:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.a 

libshare.so:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so

libshare.so.2:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so.2

libshare.so.2.0.$(version_patch):
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so.2.0.$(version_patch)

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

shpkg: $(top_builddir)/src/share-util/static/shpkg
	ln -sf $(top_builddir)/src/share-util/static/shpkg

shpasswd: $(top_builddir)/src/share-util/static/shpasswd
	ln -sf $(top_builddir)/src/share-util/static/shpasswd

shinfo: $(top_builddir)/src/share-util/static/shinfo
	ln -sf $(top_builddir)/src/share-util/static/shinfo

shcert: $(top_builddir)/src/share-util/static/shcert
	ln -sf $(top_builddir)/src/share-util/static/shcert

shdb: $(top_builddir)/src/share-util/static/shdb
	ln -sf $(top_builddir)/src/share-util/static/shdb

shfsck: $(top_builddir)/src/share-util/static/shfsck
	ln -sf $(top_builddir)/src/share-util/static/shfsck

docs:
	ln -sf $(top_builddir)/doc/libshare/html ./docs

man:
	ln -sf $(top_builddir)/doc/libshare/man ./man


.notice:
	@echo > .notice
	@echo ""
	@echo "The  libshare library and it's components has been built."
	@echo ""
	@echo "It is now safe to perform a 'make install'."
	@echo "Perform a 'make check' for additional tests."
	@echo ""
	@echo "Check out the 'bin/' directory for programs and libraries."
	@echo "Documentation is located in the docs/ directory."
	@echo ""

