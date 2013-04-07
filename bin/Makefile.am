

noinst_PROGRAMS = \
	libshare.a libshare.so libshare.so.0 libshare.so.0.0.0 \
	shnet test_libshare shnetd shcat \
	.notice

libshare.a:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.a 

libshare.so:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so

libshare.so.0:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so.0

libshare.so.0.0.0:
	ln -sf $(top_builddir)/src/share-lib/.libs/libshare.so.0.0.0

shnet:
	ln -sf $(top_builddir)/src/share-util/shnet

test_libshare:
	ln -sf $(top_builddir)/testing/test_libshare

shnetd:
	ln -sf $(top_builddir)/scripts/shnetd

shcat:
	ln -sf $(top_builddir)/scripts/shcat

.notice:
	@echo > .notice
	@echo ""
	@echo "The  libshare library and it's components has been built."
	@echo ""
	@echo "It is now safe to perform a 'make install'."
	@echo ""
	@echo "Documentation can be found in the 'doc/' directory."
	@echo ""
	@echo "Check out the 'bin/' directory for additional resources."
	@echo ""



