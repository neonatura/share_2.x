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

shadmin_classes = \
	jshare/action/SPeerAction.class \
	jshare/action/SGeoAction.class \
	jshare/action/SAccountAction.class \
	jshare/option/SOption.class \
	jshare/style/SToolTip.class \
	jshare/style/SFont.class \
	jshare/action/SAction.class \
	jshare/action/SMainAction.class \
	jshare/action/SExitAction.class \
	jshare/action/SViewLayerAction.class \
	jshare/action/SHelpAction.class \
	jshare/gui/SButton.class \
	jshare/gui/SList.class \
	jshare/gui/SListItem.class \
	jshare/gui/SListModel.class \
	jshare/gui/SListRender.class \
	jshare/gui/SDashedBorder.class \
	jshare/gui/SDialog.class \
	jshare/gui/SFrame.class \
	jshare/gui/SMainFrame.class \
	jshare/gui/SFormattedTextField.class \
	jshare/gui/SHelpButton.class \
	jshare/gui/SLabel.class \
	jshare/gui/SLargeButton.class \
	jshare/gui/SPanel.class \
	jshare/gui/SFramePanel.class \
	jshare/gui/SFramePanelListener.class \
	jshare/gui/STextField.class \
	jshare/gui/STextPane.class \
	jshare/gui/SDesktopPane.class \
	jshare/gui/SScrollPane.class \
	jshare/gui/menu/SMenu.class \
	jshare/gui/menu/SPeerMenu.class \
	jshare/gui/menu/SIndexMenu.class \
	jshare/gui/panel/SMenuPanel.class \
	jshare/gui/panel/SActionPanel.class \
	jshare/gui/panel/SLayeredPane.class \
	jshare/param/SFileMap.class \
	jshare/param/SKeyTable.class \
	jshare/param/SParam.class \
	jshare/param/SParamKey.class \
	jshare/param/SParamLock.class \
	jshare/param/SParamTable.class \
	jshare/param/SPerm.class \
	jshare/param/SPermTable.class \
	jshare/param/SLocation.class \
	jshare/param/SGeo.class \
	jshare/param/SGeoTable.class \
	jshare/net/SClient.class \
	jshare/net/SLocalClient.class \
	jshare/net/SNetClient.class \
	jshare/net/SHttpClient.class \
	jshare/net/SStream.class \
	jshare/net/SByteStream.class \
	jshare/engine/SEngine.class \
	jshare/engine/SLocalEngine.class \
	jshare/engine/SSystemEngine.class \
	jshare/engine/SNetEngine.class \
	jshare/engine/SSharenetEngine.class \
	jshare/engine/SSharecoinEngine.class \
	jsl_desktop.class

shadmin_sources = \
	$(srcdir)/jshare/action/SPeerAction.java \
	$(srcdir)/jshare/action/SGeoAction.java \
	$(srcdir)/jshare/action/SAccountAction.java \
	$(srcdir)/jshare/option/SOption.java \
	$(srcdir)/jshare/style/SToolTip.java \
	$(srcdir)/jshare/style/SFont.java \
	$(srcdir)/jshare/action/SAction.java \
	$(srcdir)/jshare/action/SMainAction.java \
	$(srcdir)/jshare/action/SExitAction.java \
	$(srcdir)/jshare/action/SViewLayerAction.java \
	$(srcdir)/jshare/action/SHelpAction.java \
	$(srcdir)/jshare/gui/SButton.java \
	$(srcdir)/jshare/gui/SList.java \
	$(srcdir)/jshare/gui/SListItem.java \
	$(srcdir)/jshare/gui/SListModel.java \
	$(srcdir)/jshare/gui/SListRender.java \
	$(srcdir)/jshare/gui/SDashedBorder.java \
	$(srcdir)/jshare/gui/SDialog.java \
	$(srcdir)/jshare/gui/SFrame.java \
	$(srcdir)/jshare/gui/SMainFrame.java \
	$(srcdir)/jshare/gui/SFormattedTextField.java \
	$(srcdir)/jshare/gui/SHelpButton.java \
	$(srcdir)/jshare/gui/SLabel.java \
	$(srcdir)/jshare/gui/SLargeButton.java \
	$(srcdir)/jshare/gui/SPanel.java \
	$(srcdir)/jshare/gui/SFramePanel.java \
	$(srcdir)/jshare/gui/SFramePanelListener.java \
	$(srcdir)/jshare/gui/STextField.java \
	$(srcdir)/jshare/gui/STextPane.java \
	$(srcdir)/jshare/gui/SDesktopPane.java \
	$(srcdir)/jshare/gui/SScrollPane.java \
	$(srcdir)/jshare/gui/menu/SMenu.java \
	$(srcdir)/jshare/gui/menu/SPeerMenu.java \
	$(srcdir)/jshare/gui/menu/SIndexMenu.java \
	$(srcdir)/jshare/gui/panel/SMenuPanel.java \
	$(srcdir)/jshare/gui/panel/SActionPanel.java \
	$(srcdir)/jshare/gui/panel/SLayeredPane.java \
	$(srcdir)/jshare/param/SFileMap.java \
	$(srcdir)/jshare/param/SKeyTable.java \
	$(srcdir)/jshare/param/SParam.java \
	$(srcdir)/jshare/param/SParamKey.java \
	$(srcdir)/jshare/param/SParamLock.java \
	$(srcdir)/jshare/param/SParamTable.java \
	$(srcdir)/jshare/param/SPerm.java \
	$(srcdir)/jshare/param/SPermTable.java \
	$(srcdir)/jshare/param/SLocation.java \
	$(srcdir)/jshare/param/SGeo.java \
	$(srcdir)/jshare/param/SGeoTable.java \
	$(srcdir)/jshare/net/SStream.java \
	$(srcdir)/jshare/net/SByteStream.java \
	$(srcdir)/jshare/net/SClient.java \
	$(srcdir)/jshare/net/SLocalClient.java \
	$(srcdir)/jshare/net/SNetClient.java \
	$(srcdir)/jshare/net/SHttpClient.java \
	$(srcdir)/jshare/engine/SEngine.java \
	$(srcdir)/jshare/engine/SLocalEngine.java \
	$(srcdir)/jshare/engine/SSystemEngine.java \
	$(srcdir)/jshare/engine/SNetEngine.java \
	$(srcdir)/jshare/engine/SSharenetEngine.java \
	$(srcdir)/jshare/engine/SSharecoinEngine.java \
	$(srcdir)/jsl_desktop.java

xz_sources = \
	$(srcdir)/org/tukaani/xz/LZMA2Decoder.java \
	$(srcdir)/org/tukaani/xz/ARMOptions.java \
	$(srcdir)/org/tukaani/xz/ARMThumbOptions.java \
	$(srcdir)/org/tukaani/xz/BCJCoder.java \
	$(srcdir)/org/tukaani/xz/BCJDecoder.java \
	$(srcdir)/org/tukaani/xz/BCJEncoder.java \
	$(srcdir)/org/tukaani/xz/BCJOptions.java \
	$(srcdir)/org/tukaani/xz/BlockInputStream.java \
	$(srcdir)/org/tukaani/xz/BlockOutputStream.java \
	$(srcdir)/org/tukaani/xz/check/Check.java \
	$(srcdir)/org/tukaani/xz/check/CRC32.java \
	$(srcdir)/org/tukaani/xz/check/CRC64.java \
	$(srcdir)/org/tukaani/xz/check/None.java \
	$(srcdir)/org/tukaani/xz/check/SHA256.java \
	$(srcdir)/org/tukaani/xz/common/DecoderUtil.java \
	$(srcdir)/org/tukaani/xz/common/EncoderUtil.java \
	$(srcdir)/org/tukaani/xz/common/StreamFlags.java \
	$(srcdir)/org/tukaani/xz/common/Util.java \
	$(srcdir)/org/tukaani/xz/CorruptedInputException.java \
	$(srcdir)/org/tukaani/xz/CountingInputStream.java \
	$(srcdir)/org/tukaani/xz/CountingOutputStream.java \
	$(srcdir)/org/tukaani/xz/delta/DeltaCoder.java \
	$(srcdir)/org/tukaani/xz/delta/DeltaDecoder.java \
	$(srcdir)/org/tukaani/xz/delta/DeltaEncoder.java \
	$(srcdir)/org/tukaani/xz/DeltaCoder.java \
	$(srcdir)/org/tukaani/xz/DeltaDecoder.java \
	$(srcdir)/org/tukaani/xz/DeltaEncoder.java \
	$(srcdir)/org/tukaani/xz/DeltaInputStream.java \
	$(srcdir)/org/tukaani/xz/DeltaOptions.java \
	$(srcdir)/org/tukaani/xz/DeltaOutputStream.java \
	$(srcdir)/org/tukaani/xz/FilterCoder.java \
	$(srcdir)/org/tukaani/xz/FilterDecoder.java \
	$(srcdir)/org/tukaani/xz/FilterEncoder.java \
	$(srcdir)/org/tukaani/xz/FilterOptions.java \
	$(srcdir)/org/tukaani/xz/FinishableOutputStream.java \
	$(srcdir)/org/tukaani/xz/FinishableWrapperOutputStream.java \
	$(srcdir)/org/tukaani/xz/IA64Options.java \
	$(srcdir)/org/tukaani/xz/index/BlockInfo.java \
	$(srcdir)/org/tukaani/xz/index/IndexBase.java \
	$(srcdir)/org/tukaani/xz/index/IndexDecoder.java \
	$(srcdir)/org/tukaani/xz/index/IndexEncoder.java \
	$(srcdir)/org/tukaani/xz/index/IndexHash.java \
	$(srcdir)/org/tukaani/xz/index/IndexRecord.java \
	$(srcdir)/org/tukaani/xz/IndexIndicatorException.java \
	$(srcdir)/org/tukaani/xz/lz/BT4.java \
	$(srcdir)/org/tukaani/xz/lz/CRC32Hash.java \
	$(srcdir)/org/tukaani/xz/lz/Hash234.java \
	$(srcdir)/org/tukaani/xz/lz/HC4.java \
	$(srcdir)/org/tukaani/xz/lz/LZDecoder.java \
	$(srcdir)/org/tukaani/xz/lz/LZEncoder.java \
	$(srcdir)/org/tukaani/xz/lz/Matches.java \
	$(srcdir)/org/tukaani/xz/lzma/LZMACoder.java \
	$(srcdir)/org/tukaani/xz/lzma/LZMADecoder.java \
	$(srcdir)/org/tukaani/xz/lzma/LZMAEncoder.java \
	$(srcdir)/org/tukaani/xz/lzma/LZMAEncoderFast.java \
	$(srcdir)/org/tukaani/xz/lzma/LZMAEncoderNormal.java \
	$(srcdir)/org/tukaani/xz/lzma/Optimum.java \
	$(srcdir)/org/tukaani/xz/lzma/State.java \
	$(srcdir)/org/tukaani/xz/LZMA2Coder.java \
	$(srcdir)/org/tukaani/xz/LZMA2Encoder.java \
	$(srcdir)/org/tukaani/xz/LZMA2InputStream.java \
	$(srcdir)/org/tukaani/xz/LZMA2Options.java \
	$(srcdir)/org/tukaani/xz/LZMA2OutputStream.java \
	$(srcdir)/org/tukaani/xz/LZMAInputStream.java \
	$(srcdir)/org/tukaani/xz/LZMAOutputStream.java \
	$(srcdir)/org/tukaani/xz/MemoryLimitException.java \
	$(srcdir)/org/tukaani/xz/PowerPCOptions.java \
	$(srcdir)/org/tukaani/xz/rangecoder/RangeCoder.java \
	$(srcdir)/org/tukaani/xz/rangecoder/RangeDecoder.java \
	$(srcdir)/org/tukaani/xz/rangecoder/RangeDecoderFromBuffer.java \
	$(srcdir)/org/tukaani/xz/rangecoder/RangeDecoderFromStream.java \
	$(srcdir)/org/tukaani/xz/rangecoder/RangeEncoder.java \
	$(srcdir)/org/tukaani/xz/rangecoder/RangeEncoderToBuffer.java \
	$(srcdir)/org/tukaani/xz/rangecoder/RangeEncoderToStream.java \
	$(srcdir)/org/tukaani/xz/RawCoder.java \
	$(srcdir)/org/tukaani/xz/SeekableFileInputStream.java \
	$(srcdir)/org/tukaani/xz/SeekableInputStream.java \
	$(srcdir)/org/tukaani/xz/SeekableXZInputStream.java \
	$(srcdir)/org/tukaani/xz/simple/ARM.java \
	$(srcdir)/org/tukaani/xz/simple/ARMThumb.java \
	$(srcdir)/org/tukaani/xz/simple/IA64.java \
	$(srcdir)/org/tukaani/xz/simple/PowerPC.java \
	$(srcdir)/org/tukaani/xz/simple/SimpleFilter.java \
	$(srcdir)/org/tukaani/xz/simple/SPARC.java \
	$(srcdir)/org/tukaani/xz/simple/X86.java \
	$(srcdir)/org/tukaani/xz/SimpleInputStream.java \
	$(srcdir)/org/tukaani/xz/SimpleOutputStream.java \
	$(srcdir)/org/tukaani/xz/SingleXZInputStream.java \
	$(srcdir)/org/tukaani/xz/SPARCOptions.java \
	$(srcdir)/org/tukaani/xz/UncompressedLZMA2OutputStream.java \
	$(srcdir)/org/tukaani/xz/UnsupportedOptionsException.java \
	$(srcdir)/org/tukaani/xz/X86Options.java \
	$(srcdir)/org/tukaani/xz/XZ.java \
	$(srcdir)/org/tukaani/xz/XZFormatException.java \
	$(srcdir)/org/tukaani/xz/XZInputStream.java \
	$(srcdir)/org/tukaani/xz/XZIOException.java \
	$(srcdir)/org/tukaani/xz/XZOutputStream.java

xz_classes = \
	org/tukaani/xz/LZMA2Decoder.class \
	org/tukaani/xz/ARMOptions.class \
	org/tukaani/xz/ARMThumbOptions.class \
	org/tukaani/xz/BCJCoder.class \
	org/tukaani/xz/BCJDecoder.class \
	org/tukaani/xz/BCJEncoder.class \
	org/tukaani/xz/BCJOptions.class \
	org/tukaani/xz/BlockInputStream.class \
	org/tukaani/xz/BlockOutputStream.class \
	org/tukaani/xz/check/Check.class \
	org/tukaani/xz/check/CRC32.class \
	org/tukaani/xz/check/CRC64.class \
	org/tukaani/xz/check/None.class \
	org/tukaani/xz/check/SHA256.class \
	org/tukaani/xz/common/DecoderUtil.class \
	org/tukaani/xz/common/EncoderUtil.class \
	org/tukaani/xz/common/StreamFlags.class \
	org/tukaani/xz/common/Util.class \
	org/tukaani/xz/CorruptedInputException.class \
	org/tukaani/xz/CountingInputStream.class \
	org/tukaani/xz/CountingOutputStream.class \
	org/tukaani/xz/delta/DeltaCoder.class \
	org/tukaani/xz/delta/DeltaDecoder.class \
	org/tukaani/xz/delta/DeltaEncoder.class \
	org/tukaani/xz/DeltaCoder.class \
	org/tukaani/xz/DeltaDecoder.class \
	org/tukaani/xz/DeltaEncoder.class \
	org/tukaani/xz/DeltaInputStream.class \
	org/tukaani/xz/DeltaOptions.class \
	org/tukaani/xz/DeltaOutputStream.class \
	org/tukaani/xz/FilterCoder.class \
	org/tukaani/xz/FilterDecoder.class \
	org/tukaani/xz/FilterEncoder.class \
	org/tukaani/xz/FilterOptions.class \
	org/tukaani/xz/FinishableOutputStream.class \
	org/tukaani/xz/FinishableWrapperOutputStream.class \
	org/tukaani/xz/IA64Options.class \
	org/tukaani/xz/index/BlockInfo.class \
	org/tukaani/xz/index/IndexBase.class \
	org/tukaani/xz/index/IndexDecoder.class \
	org/tukaani/xz/index/IndexEncoder.class \
	org/tukaani/xz/index/IndexHash.class \
	org/tukaani/xz/index/IndexRecord.class \
	org/tukaani/xz/IndexIndicatorException.class \
	org/tukaani/xz/lz/BT4.class \
	org/tukaani/xz/lz/CRC32Hash.class \
	org/tukaani/xz/lz/Hash234.class \
	org/tukaani/xz/lz/HC4.class \
	org/tukaani/xz/lz/LZDecoder.class \
	org/tukaani/xz/lz/LZEncoder.class \
	org/tukaani/xz/lz/Matches.class \
	org/tukaani/xz/lzma/LZMACoder.class \
	org/tukaani/xz/lzma/LZMADecoder.class \
	org/tukaani/xz/lzma/LZMAEncoder.class \
	org/tukaani/xz/lzma/LZMAEncoderFast.class \
	org/tukaani/xz/lzma/LZMAEncoderNormal.class \
	org/tukaani/xz/lzma/Optimum.class \
	org/tukaani/xz/lzma/State.class \
	org/tukaani/xz/LZMA2Coder.class \
	org/tukaani/xz/LZMA2Encoder.class \
	org/tukaani/xz/LZMA2InputStream.class \
	org/tukaani/xz/LZMA2Options.class \
	org/tukaani/xz/LZMA2OutputStream.class \
	org/tukaani/xz/LZMAInputStream.class \
	org/tukaani/xz/LZMAOutputStream.class \
	org/tukaani/xz/MemoryLimitException.class \
	org/tukaani/xz/PowerPCOptions.class \
	org/tukaani/xz/rangecoder/RangeCoder.class \
	org/tukaani/xz/rangecoder/RangeDecoder.class \
	org/tukaani/xz/rangecoder/RangeDecoderFromBuffer.class \
	org/tukaani/xz/rangecoder/RangeDecoderFromStream.class \
	org/tukaani/xz/rangecoder/RangeEncoder.class \
	org/tukaani/xz/rangecoder/RangeEncoderToBuffer.class \
	org/tukaani/xz/rangecoder/RangeEncoderToStream.class \
	org/tukaani/xz/RawCoder.class \
	org/tukaani/xz/SeekableFileInputStream.class \
	org/tukaani/xz/SeekableInputStream.class \
	org/tukaani/xz/SeekableXZInputStream.class \
	org/tukaani/xz/simple/ARM.class \
	org/tukaani/xz/simple/ARMThumb.class \
	org/tukaani/xz/simple/IA64.class \
	org/tukaani/xz/simple/PowerPC.class \
	org/tukaani/xz/simple/SimpleFilter.class \
	org/tukaani/xz/simple/SPARC.class \
	org/tukaani/xz/simple/X86.class \
	org/tukaani/xz/SimpleInputStream.class \
	org/tukaani/xz/SimpleOutputStream.class \
	org/tukaani/xz/SingleXZInputStream.class \
	org/tukaani/xz/SPARCOptions.class \
	org/tukaani/xz/UncompressedLZMA2OutputStream.class \
	org/tukaani/xz/UnsupportedOptionsException.class \
	org/tukaani/xz/X86Options.class \
	org/tukaani/xz/XZ.class \
	org/tukaani/xz/XZFormatException.class \
	org/tukaani/xz/XZInputStream.class \
	org/tukaani/xz/XZIOException.class \
	org/tukaani/xz/XZOutputStream.class

# "java share lib" utility program - in development
noinst_PROGRAMS += shadmin
#bin_PROGRAMS = shadmin
shadmin_SOURCES = libshare.jar $(shadmin_classes) jsl.java
shadmin_LDFLAGS = --main=jsl -fjni
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


.stamp: $(shadmin_sources)
	$(JAVAC) -d . $(shadmin_sources)


jshare/action/SPeerAction.class: .stamp
jshare/action/SGeoAction.class: .stamp
jshare/option/SOption.class: .stamp
jshare/style/SToolTip.class: .stamp
jshare/style/SFont.class: .stamp
jshare/action/SAction.class: .stamp
jshare/action/SMainAction.class: .stamp
jshare/action/SExitAction.class: .stamp
jshare/action/SViewLayerAction.class: .stamp
jshare/action/SHelpAction.class: .stamp
jshare/gui/SButton.class: .stamp
jshare/gui/SList.class: .stamp
jshare/gui/SListItem.class: .stamp
jshare/gui/SListModel.class: .stamp
jshare/gui/SListRender.class: .stamp
jshare/gui/SDashedBorder.class: .stamp
jshare/gui/SDialog.class: .stamp
jshare/gui/SFrame.class: .stamp
jshare/gui/SMainFrame.class: .stamp
jshare/gui/SFormattedTextField.class: .stamp
jshare/gui/SHelpButton.class: .stamp
jshare/gui/SLabel.class: .stamp
jshare/gui/SLargeButton.class: .stamp
jshare/gui/SPanel.class: .stamp
jshare/gui/SFramePanel.class: .stamp
jshare/gui/SFramePanelListener.class: .stamp
jshare/gui/STextField.class: .stamp
jshare/gui/STextPane.class: .stamp
jshare/gui/SDesktopPane.class: .stamp
jshare/gui/menu/SMenu.class: .stamp
jshare/gui/menu/SPeerMenu.class: .stamp
jshare/gui/menu/SIndexMenu.class: .stamp
jshare/gui/panel/SMenuPanel.class: .stamp
jshare/gui/panel/SActionPanel.class: .stamp
jshare/gui/panel/SLayeredPane.class: .stamp
jshare/net/SStream.class: .stamp
jshare/net/SByteStream.class: .stamp
jshare/net/SClient.class: .stamp
jshare/net/SLocalClient.class: .stamp
jshare/net/SNetClient.class: .stamp
jshare/net/SHttpClient.class: .stamp
jshare/engine/SEngine.class: .stamp
jshare/engine/SLocalEngine.class: .stamp
jshare/engine/SSystemEngine.class: .stamp
jshare/engine/SNetEngine.class: .stamp
jshare/engine/SSharenetEngine.class: .stamp
jshare/engine/SSharecoinEngine.class: .stamp
jsl_desktop.class: .stamp

clean-local:
	rm -f net/sharelib/*.class net/sharelib/*/*.class libshare.jar libshare_java.so *.class .stamp $(xz_classes)

