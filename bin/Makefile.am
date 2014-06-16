

noinst_PROGRAMS = \
	libshare.a libshare.so libshare.so.0 libshare.so.0.0.0 \
	libshare.jar test_libshare_java test_libshare_java.class \
	test_libshare \
	share shared shcoind shcat shls shrm shmkdir shcp shin \
	docs man \
	.notice

libshare.a:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.a 

libshare.so:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so

libshare.so.0:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so.0

libshare.so.0.0.0:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so.0.0.0

libshare.jar:
	ln -sf $(top_builddir)/src/java-share-lib/libshare.jar

test_libshare_java:
	ln -sf $(top_builddir)/src/java-share-lib/test_libshare_java

test_libshare_java.class:
	ln -sf $(top_builddir)/src/java-share-lib/test_libshare_java.class

test_libshare:
	ln -sf $(top_builddir)/testing/test_libshare

share:
	ln -sf $(top_builddir)/src/share-util/share

shared:
	ln -sf $(top_builddir)/src/share-daemon/shared

shcoind:
	ln -sf $(top_builddir)/src/share-coin/shcoind

shcat:
	ln -sf $(top_builddir)/scripts/shcat

shin:
	ln -sf $(top_builddir)/scripts/shin

shls:
	ln -sf $(top_builddir)/scripts/shls

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


