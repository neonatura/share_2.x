BUILT_SOURCES = \
	__libssl

noinst_PROGRAMS = \
	__libssl

if OPENSSL_DIR
COMPILE_OPENSSL=true
else
COMPILE_OPENSSL=cd $(top_srcdir)/depend/openssl-1.0.1g; ./config; make
endif

__libssl:
	echo > __libssl
	$(COMPILE_OPENSSL)
