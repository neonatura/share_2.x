AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = libshare_php_wrap.c php_share_php.h share_php.php

if LIBSHARE_PHP
lib_LTLIBRARIES = libshare_php.la
libshare_php_la_LIBADD = $(top_builddir)/src/share-lib/libshare.la
libshare_php_la_SOURCES = $(swig_sources)
libshare_php_la_LDFLAGS = -no-undefined -version-info @version_info@
TESTS = test_libshare_php.sh test_libshare_fs.sh
endif

SWIG_CONF = libshare_php.i
SWIG_PHP_OPT = -php 

common_includes = \
	$(top_srcdir)/src/share-lib/share.h \
	$(top_srcdir)/src/share-lib/sherr.h \
	$(top_srcdir)/src/share-lib/share_base.h \
	$(top_srcdir)/src/share-lib/shpeer.h \
	$(top_srcdir)/src/share-lib/shtime.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/mem/shmem.h \
	$(top_srcdir)/src/share-lib/fs/shfs.h \
	$(top_srcdir)/src/share-lib/shpref.h \
  $(top_srcdir)/src/share-lib/net/shnet.h

swig_sources = \
	$(common_includes) \
	php_share_php.h \
	libshare_php_wrap.c

libshare_php_wrap.c: .stamp
share_php.php: .stamp
php_share_php.h: .stamp

.stamp: libshare_php.i
	$(CP) $(top_srcdir)/src/php-share-lib/prebuild/* .
	$(SWIG_PROG) $(SWIG_PHP_OPT) $(SWIG_CONF)
	echo > .stamp

clean-all:
	rm .stamp libshare_php_wrap.c share_php.php php_share_php.h

if X86_64
install-exec-hook:
	$(LN_S) -f $(libdir)/libshare_php.so /usr/lib64/php/modules/share_php.so 
	[ "$(libdir)" = "/usr/lib64" ] || $(LN_S) -f $(libdir)/libshare.so.2 /usr/lib64/libshare.so.2
else
install-exec-hook:
	$(LN_S) -f $(libdir)/libshare_php.so /usr/lib/php/modules/share_php.so 
	[ "$(libdir)" = "/usr/lib" ] || $(LN_S) -f $(libdir)/libshare.so.2 /usr/lib/libshare.so.2
endif
