AUTOMAKE_OPTIONS = foreign
JAVAC_OPT=-d $(top_builddir)/src/java-share-lib
AM_GCJFLAGS = -Wno-all -fjni

INCLUDES = -I$(top_srcdir)/src/share-lib \
	-I$(top_srcdir)/src/share-lib/fs \
	-I$(top_srcdir)/src/share-lib/net \
	-I$(top_srcdir)/src/share-lib/mem \
	-I$(top_srcdir)/src/share-lib/sys

if LIBSHARE_JAVA
BUILT_SOURCES = libshare.jar
libshare_java_DATA = libshare.jar 
libshare_javadir = $(libdir)

noinst_PROGRAMS = test_libshare_java
test_libshare_java_SOURCES = libshare.jar test_libshare_java.java
test_libshare_java_LDFLAGS = --main=test_libshare_java -fjni

TESTS = test_libshare_java.sh

jsl_classes = \
	jshare/option/SOption.class \
	jshare/style/SToolTip.class \
	jshare/style/SFont.class \
	jshare/action/SAction.class \
	jshare/action/SPeerAction.class \
	jshare/action/SMainAction.class \
	jshare/action/SExitAction.class \
	jshare/action/SViewLayerAction.class \
	jshare/action/SHelpAction.class \
	jshare/gui/SButton.class \
	jshare/gui/SList.class \
	jshare/gui/SListItem.class \
	jshare/gui/SListRender.class \
	jshare/gui/SDashedBorder.class \
	jshare/gui/SDialog.class \
	jshare/gui/SFrame.class \
	jshare/gui/SFormattedTextField.class \
	jshare/gui/SHelpButton.class \
	jshare/gui/SLabel.class \
	jshare/gui/SLargeButton.class \
	jshare/gui/SPanel.class \
	jshare/gui/SFramePanel.class \
	jshare/gui/SFramePanelListener.class \
	jshare/gui/STextField.class \
	jshare/gui/STextPane.class \
	jshare/gui/SLayeredPane.class \
	jshare/gui/menu/SMenu.class \
	jshare/gui/menu/SPeerMenu.class \
	jshare/gui/menu/SIndexMenu.class \
	jshare/gui/panel/SMenuPanel.class \
	jsl_desktop.class

jsl_sources = \
	 $(srcdir)/jshare/option/SOption.java \
	 $(srcdir)/jshare/style/SToolTip.java \
	 $(srcdir)/jshare/style/SFont.java \
	 $(srcdir)/jshare/action/SAction.java \
	 $(srcdir)/jshare/action/SPeerAction.java \
	 $(srcdir)/jshare/action/SMainAction.java \
	 $(srcdir)/jshare/action/SExitAction.java \
	 $(srcdir)/jshare/action/SViewLayerAction.java \
	 $(srcdir)/jshare/action/SHelpAction.java \
	 $(srcdir)/jshare/gui/SButton.java \
	 $(srcdir)/jshare/gui/SList.java \
	 $(srcdir)/jshare/gui/SListItem.java \
	 $(srcdir)/jshare/gui/SListRender.java \
	 $(srcdir)/jshare/gui/SDashedBorder.java \
	 $(srcdir)/jshare/gui/SDialog.java \
	 $(srcdir)/jshare/gui/SFrame.java \
	 $(srcdir)/jshare/gui/SFormattedTextField.java \
	 $(srcdir)/jshare/gui/SHelpButton.java \
	 $(srcdir)/jshare/gui/SLabel.java \
	 $(srcdir)/jshare/gui/SLargeButton.java \
	 $(srcdir)/jshare/gui/SPanel.java \
	 $(srcdir)/jshare/gui/SFramePanel.java \
	 $(srcdir)/jshare/gui/SFramePanelListener.java \
	 $(srcdir)/jshare/gui/STextField.java \
	 $(srcdir)/jshare/gui/STextPane.java \
	 $(srcdir)/jshare/gui/SLayeredPane.java \
	 $(srcdir)/jshare/gui/menu/SMenu.java \
	 $(srcdir)/jshare/gui/menu/SPeerMenu.java \
	 $(srcdir)/jshare/gui/menu/SIndexMenu.java \
	 $(srcdir)/jshare/gui/panel/SMenuPanel.java \
	 $(srcdir)/jsl_desktop.java

# "java share lib" utility program - in development
noinst_PROGRAMS += jsl
#bin_PROGRAMS = jsl
jsl_SOURCES = libshare.jar $(jsl_classes) jsl.java
jsl_LDFLAGS = --main=jsl -fjni
endif

javasources = \
	$(top_srcdir)/src/share-lib/java/share_java.java \
	$(top_srcdir)/src/share-lib/java/share_javaJNI.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_fd_set.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_hostent.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_long.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_char.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shfs_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shjson_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shkey_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shlock_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shmeta_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shpeer_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shpool_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_unsigned_char.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shbuf_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shfs_ino_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shfs_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shjson_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shkey_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shlock_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shmeta_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shpeer_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shpool_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shsig_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shsize_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_size_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_sockaddr.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_socklen_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_ssize_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_timeval.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_unsigned_char.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_unsigned_int.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_void.java

javaclasses = \
	net/sharelib/share_java.class \
	net/sharelib/share_javaJNI.class \
	net/sharelib/SWIGTYPE_p_fd_set.class \
	net/sharelib/SWIGTYPE_p_hostent.class \
	net/sharelib/SWIGTYPE_p_long.class \
	net/sharelib/SWIGTYPE_p_p_char.class \
	net/sharelib/SWIGTYPE_p_p_shfs_t.class \
	net/sharelib/SWIGTYPE_p_p_shjson_t.class \
	net/sharelib/SWIGTYPE_p_p_shkey_t.class \
	net/sharelib/SWIGTYPE_p_p_shlock_t.class \
	net/sharelib/SWIGTYPE_p_p_shmeta_t.class \
	net/sharelib/SWIGTYPE_p_p_shpeer_t.class \
	net/sharelib/SWIGTYPE_p_p_shpool_t.class \
	net/sharelib/SWIGTYPE_p_p_unsigned_char.class \
	net/sharelib/SWIGTYPE_p_shbuf_t.class \
	net/sharelib/SWIGTYPE_p_shfs_ino_t.class \
	net/sharelib/SWIGTYPE_p_shfs_t.class \
	net/sharelib/SWIGTYPE_p_shjson_t.class \
	net/sharelib/SWIGTYPE_p_shkey_t.class \
	net/sharelib/SWIGTYPE_p_shlock_t.class \
	net/sharelib/SWIGTYPE_p_shmeta_t.class \
	net/sharelib/SWIGTYPE_p_shpeer_t.class \
	net/sharelib/SWIGTYPE_p_shpool_t.class \
	net/sharelib/SWIGTYPE_p_shsig_t.class \
	net/sharelib/SWIGTYPE_p_shsize_t.class \
	net/sharelib/SWIGTYPE_p_size_t.class \
	net/sharelib/SWIGTYPE_p_sockaddr.class \
	net/sharelib/SWIGTYPE_p_socklen_t.class \
	net/sharelib/SWIGTYPE_p_ssize_t.class \
	net/sharelib/SWIGTYPE_p_timeval.class \
	net/sharelib/SWIGTYPE_p_unsigned_char.class \
	net/sharelib/SWIGTYPE_p_unsigned_int.class \
	net/sharelib/SWIGTYPE_p_void.class

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

api_sources = \
	$(srcdir)/net/sharelib/Share.java \
	$(srcdir)/net/sharelib/SHTime.java \
	$(srcdir)/net/sharelib/SHCRC64.java \
	$(srcdir)/net/sharelib/mem/SHKey.java \
	$(srcdir)/net/sharelib/mem/SHBuffer.java \
	$(srcdir)/net/sharelib/mem/SHBufferInputStream.java \
	$(srcdir)/net/sharelib/mem/SHBufferOutputStream.java \
	$(srcdir)/net/sharelib/fs/SHInode.java

api_classes = \
	net/sharelib/Share.class \
	net/sharelib/SHTime.class \
	net/sharelib/SHCRC64.class \
	net/sharelib/mem/SHKey.class \
	net/sharelib/mem/SHBuffer.class \
	net/sharelib/mem/SHBufferInputStream.class \
	net/sharelib/mem/SHBufferOutputStream.class \
	net/sharelib/fs/SHInode.class

libshare.so:
	$(LN_S) -f $(top_builddir)/src/share-lib/.libs/libshare.so

libshare.jar: libshare.so
	$(JAVAC) $(JAVAC_OPT) $(javasources) $(api_sources) test_libshare_java.java
	$(JAR) cf libshare.jar $(javaclasses) $(api_classes)


.stamp: $(jsl_sources)
	$(JAVAC) -d . $(jsl_sources)

jshare/option/SOption.class: .stamp
jshare/style/SToolTip.class: .stamp
jshare/style/SFont.class: .stamp
jshare/action/SAction.class: .stamp
jshare/action/SPeerAction.class: .stamp
jshare/action/SMainAction.class: .stamp
jshare/action/SExitAction.class: .stamp
jshare/action/SViewLayerAction.class: .stamp
jshare/action/SHelpAction.class: .stamp
jshare/gui/SButton.class: .stamp
jshare/gui/SList.class: .stamp
jshare/gui/SListItem.class: .stamp
jshare/gui/SListRender.class: .stamp
jshare/gui/SDashedBorder.class: .stamp
jshare/gui/SDialog.class: .stamp
jshare/gui/SFrame.class: .stamp
jshare/gui/SFormattedTextField.class: .stamp
jshare/gui/SHelpButton.class: .stamp
jshare/gui/SLabel.class: .stamp
jshare/gui/SLargeButton.class: .stamp
jshare/gui/SPanel.class: .stamp
jshare/gui/SFramePanel.class: .stamp
jshare/gui/SFramePanelListener.class: .stamp
jshare/gui/STextField.class: .stamp
jshare/gui/STextPane.class: .stamp
jshare/gui/SLayeredPane.class: .stamp
jshare/gui/menu/SMenu.class: .stamp
jshare/gui/menu/SPeerMenu.class: .stamp
jshare/gui/menu/SIndexMenu.class: .stamp
jshare/gui/panel/SMenuPanel.class: .stamp
jsl_desktop.class: .stamp

clean-local:
	rm -f net/sharelib/*.class net/sharelib/*/*.class libshare.jar libshare_java.so *.class .stamp

