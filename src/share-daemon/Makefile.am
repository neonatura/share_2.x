
AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS =

common_INCLUDES = \
	$(top_builddir)/config.h \
	$(top_srcdir)/src/share-lib/share.h \
	$(top_srcdir)/src/share-lib/mem/shmem.h \
	$(top_srcdir)/src/share-lib/sherr.h \
	$(top_srcdir)/src/share-lib/share_base.h \
	$(top_srcdir)/src/share-lib/shpeer.h \
	$(top_srcdir)/src/share-lib/shtime.h \
	$(top_srcdir)/src/share-lib/shpref.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/shfile.h \
	$(top_srcdir)/src/share-lib/fs/shfs.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3.h \
	$(top_srcdir)/src/share-lib/net/shnet.h \
	sharedaemon.h \
	sharedaemon_server.h \
	sharedaemon_file.h \
	sharedaemon_client.h \
	sharedaemon_device.h \
	sharedaemon_app.h \
	sharedaemon_store.h \
	bits/bits.h \
	bits/app.h \
	bits/transaction.h \
	bits/event.h \
	bits/signature.h \
	bits/metric.h \
	bits/trust.h \
	bits/account.h \
	bits/identity.h \
	bits/session.h \
	bits/ledger.h \
	bits/schedule.h \
	bits/ward.h \
	dev/dev.h \
	dev/ntp_clock.h \
	dev/ntp_proto.h \
	dev/card_usb.h \
	dev/card_kmap.h \
	dev/fpga_usb.h \
	dev/leitch_serial.h

common_SOURCES = \
	sharedaemon.c \
	sharedaemon_client.c \
	sharedaemon_device.c \
	sharedaemon_server.c \
	sharedaemon_app.c \
	sharedaemon_store.c \
	sharedaemon_file.c \
	bits/app.c \
	bits/transaction.c \
	bits/event.c \
	bits/signature.c \
	bits/metric.c \
	bits/trust.c \
	bits/account.c \
	bits/identity.c \
	bits/session.c \
	bits/ledger.c \
	bits/schedule.c \
	bits/ward.c \
	dev/ntp_clock.c \
	dev/dev_usb.c \
	dev/card_usb.c \
	dev/card_kmap.c \
	dev/fpga_usb.c \
	dev/leitch_serial.c

sbin_PROGRAMS = shared

shared_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shared_LDADD = $(top_builddir)/src/share-lib/libshare.a -lusb-1.0

if HAVE_INITD_DIR
EXTRA_DIST = $(top_builddir)/scripts/initd/shared

install-exec-hook:
	rm -f $(INITD_DIR)/init.d/shared  
	rm -f $(INITD_DIR)/rc3.d/S85shared  
	rm -f $(INITD_DIR)/rc3.d/K85shared  
	rm -f $(INITD_DIR)/rc4.d/S85shared  
	rm -f $(INITD_DIR)/rc4.d/K85shared  
	rm -f $(INITD_DIR)/rc5.d/S85shared  
	rm -f $(INITD_DIR)/rc5.d/K85shared  
	cp $(top_builddir)/scripts/initd/shared $(INITD_DIR)/init.d/
	cd $(INITD_DIR)/rc3.d && $(LN_S) ../init.d/shared S85shared
	cd $(INITD_DIR)/rc3.d && $(LN_S) ../init.d/shared K85shared
	cd $(INITD_DIR)/rc4.d && $(LN_S) ../init.d/shared S85shared
	cd $(INITD_DIR)/rc4.d && $(LN_S) ../init.d/shared K85shared
	cd $(INITD_DIR)/rc5.d && $(LN_S) ../init.d/shared S85shared
	cd $(INITD_DIR)/rc5.d && $(LN_S) ../init.d/shared K85shared

uninstall-hook:
	rm -f $(INITD_DIR)/init.d/shared  
	rm -f $(INITD_DIR)/rc3.d/S85shared  
	rm -f $(INITD_DIR)/rc3.d/K85shared  
	rm -f $(INITD_DIR)/rc4.d/S85shared  
	rm -f $(INITD_DIR)/rc4.d/K85shared  
	rm -f $(INITD_DIR)/rc5.d/S85shared  
	rm -f $(INITD_DIR)/rc5.d/K85shared  
endif


if DEBUG
AM_CFLAGS = -g3 -O0 -Wno-discarded-qualifiers \
	-I$(top_srcdir)/src/share-lib \
	-I$(top_srcdir)/src/sexe -DSHARELIB
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops \
	-Wno-discarded-qualifiers \
	-I$(top_srcdir)/src/share-lib \
	-I$(top_srcdir)/src/sexe -DSHARELIB
AM_CXXFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops
endif

