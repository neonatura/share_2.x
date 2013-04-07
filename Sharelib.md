


Utility functions to generate unique checksums of data. More...
           struct shpeer_t
                  The local or remote machine associated with the sharefs
                  partition. More...
           struct shbuf_t
                  A memory buffer. More...
                  The_'sharefs'_file-system.
                  The sharefs file system overlays ontop of your current
                  filesystem in order to provide extended file operations.

                  Meta_definition_hash_maps_(meta_maps).
                  A meta definition is part of a shmeta_t hashmap.

                  Network_socket_operations.
                  The libshare Socket Handling provides access to regular
                  socket operations with posix or convienence functions in
                  addition to access to the ESP network protocol.

          #define SHSK_PEER_LOCAL 0
                  The local machine.
          #define SHSK_PEER_IPV4 1
                  A remote IPv4 network destination.
          #define SHSK_PEER_IPV6 2
                  A remote IPv6 network destination.
          #define SHSK_PEER_VPN_IPV4 3
                  A IPv4 network destination on the sharenet VPN.
          #define SHSK_PEER_VPN_IPV6 4
                  A IPv6 network destination on the sharenet VPN.
          #define SHPREF_BASE_DIR "base-dir"
                  Specifies the preferred location of where the sharefs
                  filesystem is stored on the local file system.
          #define SHPREF_TRACK "track"
                  Specifies whether to track sharefs filesystem revisions.
          #define SHPREF_OVERLAY "overlay"
                  Specifies whether the sharefs file system references files on
                  the local filesystem.
          #define SHPREF_MAX 3
                  Specifies the number of preferences available.
          #define shpref_track() (0 == strcmp(shpref_get(SHPREF_TRACK), "true")
                  ? TRUE : FALSE)
                  Specifies whether to track sharefs filesystem revisions.
          #define shpref_set_track(opt) (opt ? shpref_set(SHPREF_TRACK, "true")
                  : shpref_set(SHPREF_TRACK, "false"))
                  Permanently sets the SHPREF_TRACK option.
          #define shpref_overlay() (0 == strcmp(shpref_get(SHPREF_OVERLAY),
                  "true") ? TRUE : FALSE)
                  Specifies whether to overlay the sharefs filesystem ontop of
                  the work directory on the local filesystem.
          #define shpref_set_overlay(opt) (opt ? shpref_set(SHPREF_OVERLAY,
                  "true") : shpref_set(SHPREF_OVERLAY, "false"))
                  Permanently sets the SHPREF_OVERLAY option.
          #define shpref_unset(pref) shpref_set(pref, NULL)
                  Persistently unset a libshare configuration option.
          #define shpref_sess_set(pref, value) shmeta_set(_pref, shkey_init_str
                  (pref), value)
                  Overwrite a preference for the current session.
          #define shpref_sess_unset(pref) shpref_sess_set(pref, NULL)
                  Temporarily unset a libshare configuration option.
 typedef uint64_t shsize_t
                  A specification of byte size.
 typedef uint64_t shkey_t
                  A key used to represent a hash code of an object.
 typedef uint64_t shtime_t
                  The libshare representation of a particular time.
                  An email address where bug reports can be submitted.
                  The current libshare library version.
                  The libshare library package name.
                  Initialize a memory buffer for use.
                  Inserts a string into a shbuf_t memory pool.
                  Inserts a binary data segment into a shbuf_t memory pool.
                  The current size of the data segement stored in the memory
                  buffer.
                  Clear the contents of a shbuf_t libshare memory buffer.
                  Frees the resources utilizited by the memory buffer.
                  Create a shkey_t hashmap key reference from kvalue kvalue The
                  string to generate into a shkey_t.
          shkey_t shkey_init_num (long kvalue)
                  Create a shkey_t hashmap key reference from a number.
          shkey_t shkey_init_unique (void)
                  Create a unique shkey_t hashmap key reference.
                  Specifies the preferred location of where the sharefs
                  filesystem is stored on the local file system.
                  The local filesystem path for storing configuration options.
              int shpref_init (void)
                  Initialize an instance of configuration options in memory.
             void shpref_free (void)
                  Free the configuration options loaded into memory.
                  Retrieve a configuration option value.
                  Set a persistent value for a particular libshare user-
                  specific configuration option.
           double shtime (void)
                  Generate a float-point precision representation of the
                  current time.
         shtime_t shtime64 (void)
                  Generate a 64bit representation integral of the current time
                  with millisecond precision.
===============================================================================
Utility functions to generate unique checksums of data.
Time operations are performed in order to store and compare timestamps in the
sharefs file system and for network operations.
Handles management of user-specific configuration options for the Share
Library.
Hash code token operations.
  Note:
      See the shpref_sess_set() function for information on overwriting an
      option values for the current process session. Specify user specific
      configuration items.
libshare_time Time calculating operations.
===============================================================================
#define SHPREF_BASE_DIR "base-dir"
Specifies the preferred location of where the sharefs filesystem is stored on
the local file system.
  Note:
      The default location is '$HOME/.share'.
      Use shpref_sess_set() to temporarily overwrite this value.
Definition at line 44 of file shpref.h.
#define shpref_overlay (  ) (0 == strcmp(shpref_get(SHPREF_OVERLAY), "true") ?
                            TRUE : FALSE)
Specifies whether to overlay the sharefs filesystem ontop of the work directory
on the local filesystem.
  Note:
      Disable this option to prevent libshare from writing outside of the base
      directory.
  Returns:
      A zero (0) when disabled and a non-zero value when enabled.
Definition at line 86 of file shpref.h.
#define SHPREF_OVERLAY "overlay"
Specifies whether the sharefs file system references files on the local
filesystem.
  Note:
      Use shpref_sess_set() to temporarily overwrite this value.
Definition at line 54 of file shpref.h.
#define shpref_set_overlay ( opt  ) (opt ? shpref_set(SHPREF_OVERLAY, "true") :
                                    shpref_set(SHPREF_OVERLAY, "false"))
Permanently sets the SHPREF_OVERLAY option.
  Parameters:
       opt A zero to disable the option and a non-zero to enable.
Definition at line 93 of file shpref.h.
#define shpref_set_track ( opt  ) (opt ? shpref_set(SHPREF_TRACK, "true") :
                                  shpref_set(SHPREF_TRACK, "false"))
Permanently sets the SHPREF_TRACK option.
  Parameters:
       opt A zero to disable the option and a non-zero to enable.
Definition at line 78 of file shpref.h.
#define shpref_track (  ) (0 == strcmp(shpref_get(SHPREF_TRACK), "true") ? TRUE
                          : FALSE)
Specifies whether to track sharefs filesystem revisions.
  Returns:
      A zero (0) when disabled and a non-zero value when enabled.
Definition at line 71 of file shpref.h.
#define SHPREF_TRACK "track"
Specifies whether to track sharefs filesystem revisions.
  Note:
      Use shpref_sess_set() to temporarily overwrite this value.
Definition at line 49 of file shpref.h.
===============================================================================
typedef uint64_t shsize_t
A specification of byte size.
shmeta_value_t.sz
Definition at line 94 of file share.h.
===============================================================================
Initialize a memory buffer for use.
  Note:
      A shbuf_t memory buffer handles automatic allocation of memory.
shkey_t shkey_init_num ( long kvalue )
Create a shkey_t hashmap key reference from a number.
kvalue The number to generate into a shkey_t
  Returns:
      A statically allocated version of
Create a shkey_t hashmap key reference from kvalue kvalue The string to
generate into a shkey_t.
  Returns:
      A shkey_t referencing a kvalue
shkey_t shkey_init_unique ( void  )
Create a unique shkey_t hashmap key reference.
  Returns:
      A shkey_t containing a unique key value.
Specifies the preferred location of where the sharefs filesystem is stored on
the local file system.
  Returns:
      The path to a directory on the local file-system.
void shpref_free ( void  )
Free the configuration options loaded into memory.
  Note:
      This will remove all temporar configuration settings that have been made
      this process session.
                 )
Retrieve a configuration option value.
  Parameters:
       pref          The name of the preference.
       default_value The default string value to return if the preference is
                     not set.
  Returns:
      The configuration option value.
int shpref_init ( void  )
Initialize an instance of configuration options in memory.
  Note:
      This function does not need to be called in order to retrieve or set
      configuration options.
  Returns:
      A zero (0) on success and a negative one (-1) on failure.
The local filesystem path for storing configuration options.
  Returns:
      The path to the location on the local file-system that contains user-
      specific libshare configuration options.
               )
Set a persistent value for a particular libshare user-specific configuration
option.
Specify user specific configuration items: SHPREF_BASE_DIR The base directory
to store sharefs file data. SHPREF_TRACK Whether to automatically track file
revisions. SHPREF_OVERLAY Whether to write outside of the base directory. Set a
configuration option value.
  Parameters:
       pref  The name of the preference.
       value The configuration option value.
  Returns:
      The configuration option value.
double shtime ( void  )
Generate a float-point precision representation of the current time.
  Returns:
      an double representing the milliseconds since 2012 UTC.
shtime_t shtime64 ( void  )
Generate a 64bit representation integral of the current time with millisecond
precision.
  Returns:
      an unsigned long representing the milliseconds since 2012 UTC.
AllData_StructuresVariables

===============================================================================
     Generated on 6 Apr 2013 for libshare by [doxygen] 1.6.1
