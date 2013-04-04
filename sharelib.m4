# Configure paths for SHARE LIBRARY
# Owen Taylor     97-11-3

dnl AM_PATH_SHARE([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for SHARE, and define SHARE_CFLAGS and SHARE_LIBS, if "gmodule" or
dnl
AC_DEFUN(AM_PATH_SHARE,
[dnl
dnl Get the cflags and libraries from the share-config script
dnl
AC_ARG_WITH(share-prefix,[  --with-share-prefix=PFX   Prefix where SHARE is installed (optional)],
            share_config_prefix="$withval", share_config_prefix="")
AC_ARG_WITH(share-exec-prefix,[  --with-share-exec-prefix=PFX Exec prefix where SHARE is installed (optional)],
            share_config_exec_prefix="$withval", share_config_exec_prefix="")
AC_ARG_ENABLE(sharetest, [  --disable-sharetest       Do not try to compile and run a test SHARE program],
		    , enable_sharetest=yes)

  if test x$share_config_exec_prefix != x ; then
     share_config_args="$share_config_args --exec-prefix=$share_config_exec_prefix"
     if test x${SHARE_CONFIG+set} != xset ; then
        SHARE_CONFIG=$share_config_exec_prefix/bin/share-config
     fi
  fi
  if test x$share_config_prefix != x ; then
     share_config_args="$share_config_args --prefix=$share_config_prefix"
     if test x${SHARE_CONFIG+set} != xset ; then
        SHARE_CONFIG=$share_config_prefix/bin/share-config
     fi
  fi

  AC_PATH_PROG(SHARE_CONFIG, share-config, no)
  min_share_version=ifelse([$1], ,1.0.0,$1)
  AC_MSG_CHECKING(for SHARE - version >= $min_share_version)
  no_share=""
  if test "$SHARE_CONFIG" = "no" ; then
    no_share=yes
  else
    SHARE_CFLAGS=`$SHARE_CONFIG $share_config_args --cflags`
    SHARE_LIBS=`$SHARE_CONFIG $share_config_args --libs`
    share_config_major_version=`$SHARE_CONFIG $share_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    share_config_minor_version=`$SHARE_CONFIG $share_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    share_config_micro_version=`$SHARE_CONFIG $share_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_sharetest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $SHARE_CFLAGS"
      LIBS="$LIBS $SHARE_LIBS"
dnl
dnl Now check if the installed SHARE is sufficiently new. (Also sanity
dnl checks the results of share-config to some extent
dnl
      rm -f conf.sharetest
      AC_TRY_RUN([
#include <share.h>
#include <stdio.h>
#include <stdlib.h>

int
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.sharetest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_share_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_share_version");
     exit(1);
   }

  if ((share_major_version != $share_config_major_version) ||
      (share_minor_version != $share_config_minor_version) ||
      (share_micro_version != $share_config_micro_version))
    {
      printf("\n*** 'share-config --version' returned %d.%d.%d, but SHARE (%d.%d.%d)\n",
             $share_config_major_version, $share_config_minor_version, $share_config_micro_version,
             share_major_version, share_minor_version, share_micro_version);
      printf ("*** was found! If share-config was correct, then it is best\n");
      printf ("*** to remove the old version of SHARE. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If share-config was wrong, set the environment variable SHARE_CONFIG\n");
      printf("*** to point to the correct copy of share-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    }
  else if ((share_major_version != SHARE_MAJOR_VERSION) ||
	   (share_minor_version != SHARE_MINOR_VERSION) ||
           (share_micro_version != SHARE_MICRO_VERSION))
    {
      printf("*** SHARE header files (version %d.%d.%d) do not match\n",
	     SHARE_MAJOR_VERSION, SHARE_MINOR_VERSION, SHARE_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     share_major_version, share_minor_version, share_micro_version);
    }
  else
    {
      if ((share_major_version > major) ||
        ((share_major_version == major) && (share_minor_version > minor)) ||
        ((share_major_version == major) && (share_minor_version == minor) && (share_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of SHARE (%d.%d.%d) was found.\n",
               share_major_version, share_minor_version, share_micro_version);
        printf("*** You need a version of SHARE newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** SHARE is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the share-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of SHARE, but you can also set the SHARE_CONFIG environment to point to the\n");
        printf("*** correct copy of share-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_share=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_share" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])
  else
     AC_MSG_RESULT(no)
     if test "$SHARE_CONFIG" = "no" ; then
       echo "*** The share-config script installed by SHARE could not be found"
       echo "*** If SHARE was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the SHARE_CONFIG environment variable to the"
       echo "*** full path to share-config."
     else
       if test -f conf.sharetest ; then
        :
       else
          echo "*** Could not run SHARE test program, checking why..."
          CFLAGS="$CFLAGS $SHARE_CFLAGS"
          LIBS="$LIBS $SHARE_LIBS"
          AC_TRY_LINK([
#include <share.h>
#include <stdio.h>
],      [ return ((share_major_version) || (share_minor_version) || (share_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding SHARE or finding the wrong"
          echo "*** version of SHARE. If it is not finding SHARE, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the GTK package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps gtk gtk-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means SHARE was incorrectly installed"
          echo "*** or that you have moved SHARE since it was installed. In the latter case, you"
          echo "*** may want to edit the share-config script: $SHARE_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     SHARE_CFLAGS=""
     SHARE_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(SHARE_CFLAGS)
  AC_SUBST(SHARE_LIBS)
  rm -f conf.sharetest
])
