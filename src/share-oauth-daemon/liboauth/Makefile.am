ACLOCAL_AMFLAGS= -I m4

noinst_LIBRARIES = liboauth.a
liboauth_a_SOURCES=oauth.c config.h hash.c xmalloc.c xmalloc.h oauth_http.c oauth.h

if HAVE_CURL
liboauth_a_CFLAGS=-DHAVE_CURL
endif
