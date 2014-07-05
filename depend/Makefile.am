BUILT_SOURCES = \
	__libssl

noinst_PROGRAMS = \
	__libssl

__libssl:
	echo > __libssl
	(cd $(top_srcdir)/depend/openssl-1.0.1g; ./config; make)
