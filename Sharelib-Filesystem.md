


The sharefs file system overlays ontop of your current filesystem in order to
provide extended file operations. More...
                struct shfs_inode_hdr_t
                struct shfs_hdr_t
                       A sharefs filesystem inode header. More...
                struct shfs_ino_t
                       A sharefs filesystem inode. More...
                struct shfs_t
                       The sharefs filesystem structure. More...
                struct shfs_journal_data_t
                       A memory segment containing a journal's data. More...
                struct shfs_journal_t
                       A sharefs filesystem journal. More...
                struct shrev_t
                       Describes a particular revision of a data segment.
                       More...
               #define SHFS_LEVEL_PUBLIC 0
               #define SHFS_MAX_LEVELS 1
               #define NAME_MAX 4095
               #define PATH_MAX NAME_MAX
               #define SHFS_OVERLAY (1 << 0)
                       Overlay sharefs on top of current filesystem.
               #define SHFS_TRACK (1 << 1)
                       Track all revisions of file modifications.
               #define SHFS_PRIVATE (1 << 2)
                       A sharefs filesystem that is externally unaccessible
                       beyond the scope of this application.
               #define SHFS_SYNC (1 << 3)
                       Disabling caching and asynchronous file operations.
               #define SHFS_REMOTE (1 << 4)
                       The partition is located on a remote machine.
               #define SHINODE_APP 100
                       Inode is in reference to an application-specific
                       directory.
               #define SHINODE_PARTITION 101
                       Inode is the root of an entire sharefs partition.
               #define SHINODE_PEER 102
                       Inode is a reference to a remote sharefs partition.
               #define SHINODE_DELTA 103
                       Inode is a reference to a binary delta revision.
               #define SHINODE_ARCHIVE 104
                       An archive of files and/or directories.
               #define SHINODE_REFERENCE 105
                       A reference to another inode.
               #define SHINODE_META 106
                       A meta definition hashmap (meta map).
               #define SHINODE_DIRECTORY 107
                       A directory containing multiple file references.
               #define SHFS_BLOCK_SIZE 1024
                       The maximum size a single block can contain.
               #define SHFS_BLOCK_DATA_SIZE 992
                       The size of the data segment each inode contains.
               #define SHFS_MAX_BLOCK 57344
                       The maximum number of blocks in a sharefs journal.
               #define SHFS_MAX_JOURNAL 65536
                       The number of journals a sharefs filesystem contains.
                       The maximum number of bytes in a sharefs file-system
                       journal.
               #define shfs_journal_index(_inode) ((shfs_inode_off_t)(shcrc
                       (_inode->d_raw.name, NAME_MAX) % SHFS_MAX_JOURNAL))
                       Identify the default journal number for a inode's name.
               #define shfs_meta_free(_meta_p) shmeta_free(_meta_p)
                       Free an instance to a sharedfs meta definition hashmap.
 typedef struct shfs_t shfs_t
                       A type defintion for the sharefs filesytem structure.
    typedef __uint16_t shfs_inode_off_t
                       A sharefs filesystem inode or journal reference.
    typedef __uint32_t shfs_ino_type_t
                       A sharefs inode type definition.
    typedef __uint64_t shfs_size_t
                       A sharefs inode data size definition.
    typedef __uint32_t shfs_crc_t
                       A sharefs inode data checksum type definition.
       typedef uint8_t shfs_block_t [1024]
                       A single block of data inside a journal.
                       Strips the absolute parent from app_name.
                       Creates a reference to a sharefs filesystem.
                       Free a reference to a sharefs partition.
                       Retrieve a sharefs inode directory entry based on a
                       given parent inode and path name.
                       Writes a single inode block to a sharefs filesystem
                       journal.
                       Stores a data segment to a sharefs filesystem inode.
                       Retrieve a single data block from a sharefs filesystem
                       inode.
                       Retrieve a data segment of a sharefs filesystem inode.
                       The local file-system path where a sharefs journal is
                       stored.
                       Returns an instance to a sharefs filesystem journal.
                       Initializes a sharefs filesystem journal for use.
                       Sync a sharefs journal to the local file-system.
                       Obtain a reference to the meta definition hashmap
                       associated with the inode entry.
                       Flush the inode's meta map to disk.
                       Obtain an exclusive lock to a process with the same
                       process_path and runtime_mode.
                       Read a file from the local filesystem into memory.
                       fd)
                       Writes the file contents of the inode to the file
                       stream.
===============================================================================
The sharefs file system overlays ontop of your current filesystem in order to
provide extended file operations.
libshare_fs_inode The 'sharefs' inode sub-system.
Filesystem Modes libshare_fs_mode The sharefs file system modes.
The sharefs file system.
===============================================================================
#define SHFS_BLOCK_DATA_SIZE 992
The size of the data segment each inode contains.
  Note:
      992 = (SHFS_BLOCK_SIZE - sizeof(shfs_hdr_t))
Definition at line 162 of file shfs.h.
#define SHFS_BLOCK_SIZE 1024
The maximum size a single block can contain.
  Note:
      Each block segment is 1024 bytes which is equal to the size of shfs_ino_t
      structure. Blocks are kept at 1k in order to reduce overhead on the IP
      protocol.
Definition at line 156 of file shfs.h.
                                       ((shfs_inode_off_t)(shcrc(_inode-
#define shfs_journal_index ( _inode  ) >d_raw.name, NAME_MAX) %
                                       SHFS_MAX_JOURNAL))
Identify the default journal number for a inode's name.
  Returns:
      A sharefs filesystem journal index number.
Definition at line 113 of file shfs_journal.h.
#define SHFS_MAX_JOURNAL 65536
The number of journals a sharefs filesystem contains.
shfs_journal_t.index
Definition at line 39 of file shfs_journal.h.
#define shfs_meta_free ( _meta_p  ) shmeta_free(_meta_p)
Free an instance to a sharedfs meta definition hashmap.
  Note:
      Directly calls shmeta_free().
Definition at line 56 of file shfs_meta.h.
#define SHFS_OVERLAY (1 << 0)
Overlay sharefs on top of current filesystem.
  Note:
      Use 'shnet --nosync' for example behavior of this flag.
Definition at line 61 of file shfs.h.
#define SHFS_PRIVATE (1 << 2)
A sharefs filesystem that is externally unaccessible beyond the scope of this
application.
  Note:
      Use 'shnet --hidden' for example behavior of this flag.
Definition at line 74 of file shfs.h.
#define SHFS_TRACK (1 << 1)
Track all revisions of file modifications.
  Note:
      Use 'shnet --track' for example behavior of this flag.
Definition at line 67 of file shfs.h.
#define SHINODE_APP 100
Inode is in reference to an application-specific directory.
  Note:
      See also: shfs_node.d_type
Definition at line 109 of file shfs.h.
#define SHINODE_ARCHIVE 104
An archive of files and/or directories.
  Note:
      See also: shfs_node.d_type
Definition at line 133 of file shfs.h.
#define SHINODE_DELTA 103
Inode is a reference to a binary delta revision.
  Note:
      See also: shfs_node.d_type
Definition at line 127 of file shfs.h.
#define SHINODE_META 106
A meta definition hashmap (meta map).
  Note:
      The referenced inode may be local or remote.
Definition at line 145 of file shfs.h.
#define SHINODE_PARTITION 101
Inode is the root of an entire sharefs partition.
  Note:
      See also: shfs_node.d_type
Definition at line 115 of file shfs.h.
#define SHINODE_PEER 102
Inode is a reference to a remote sharefs partition.
  Note:
      See also: shfs_node.d_type
Definition at line 121 of file shfs.h.
#define SHINODE_REFERENCE 105
A reference to another inode.
  Note:
      The referenced inode may be local or remote.
  Examples:
      shfs_inode_remote_link.c.
Definition at line 139 of file shfs.h.
===============================================================================
typedef uint8_t shfs_block_t[1024]
A single block of data inside a journal.
shfs_journal_t.data
Definition at line 50 of file shfs_journal.h.
===============================================================================
Strips the absolute parent from app_name.
  Note:
      "/test/one/two" becomes "two"
  Parameters:
       app_name The running application's executable path
  Returns:
      Relative filename of executable.
Free a reference to a sharefs partition.
  Parameters:
       tree_p A reference to the sharefs partition instance to free.
  Examples:
      shfs_inode_remote_link.c.
                    int    flags
                  )
Creates a reference to a sharefs filesystem.
app_name The application's executable name. flags A combination of SHFS_XXX
flags.
  Returns:
      shfs_t The sharefs filesystem.
  Examples:
      shfs_inode_mkdir.c, shfs_inode_remote_copy.c, and
      shfs_inode_remote_link.c.
                         int          mode
                       )
Retrieve a sharefs inode directory entry based on a given parent inode and path
name.
  Note:
      inode.
      A new inode is created if a pre-existing one is not found.
  Parameters:
       parent The parent inode such as a directory where the file presides.
       name   The relational pathname of the file being referenced.
       mode   The type of information that this inode is referencing
              (SHINODE_XX).
  Returns:
      A shfs_node is returned based on the parent, name, and mode specified. If
      one already exists it will be returned, and otherwise a new entry will be
      created.
  Examples:
      shfs_inode_mkdir.c, shfs_inode_remote_copy.c, and
      shfs_inode_remote_link.c.
                          size_t       data_of,
                          size_t       data_len
                        )
Retrieve a data segment of a sharefs filesystem inode.
  Parameters:
       tree     The sharefs partition allocated by shfs_init().
       inode    The inode whose data is being retrieved.
       ret_buff The shbuf_t return buffer.
       data_of  The offset to begin reading data from the inode.
       data_len The length of data to be read.
  Returns:
      The number of bytes read on success, and a (-1) if the file does not
      exist.
                          )
Retrieve a single data block from a sharefs filesystem inode.
  Parameters:
       tree  The sharefs partition allocated by shfs_init().
       inode The inode whose data is being retrieved.
       hdr   A specification of where the block is location in the sharefs
             filesystem partition.
       inode The inode block data to be filled in.
  Returns:
      Returns 0 on success and -1 on failure. Check the errno for additional
      information.
                           shfs_size_t  data_of,
                           shfs_size_t  data_len
                         )
Stores a data segment to a sharefs filesystem inode.
  Parameters:
       tree     The sharefs partition allocated by shfs_init().
       inode    The inode whose data is being retrieved.
       data     The data segment to write to the inode.
       data_of  The offset to begin reading data from the inode.
       data_len The length of data to be read.
  Returns:
      The number of bytes written on success, and a (-1) if the file cannot be
      written to.
                       )
Initializes a sharefs filesystem journal for use.
  Note:
      This may not free resources if cached in a shfs_t partition.
  Parameters:
       tree   The sharefs partition.
       jrnl_p A reference to the journal instance to be free'd.
                        int      jno
                      )
  Parameters:
       tree The sharefs filesystem partition.
       jno  The index number of the journal.
  Returns:
      A inode index number or (-1) on failure.
                       )
Sync a sharefs journal to the local file-system.
  Note:
      Check errno for additional error-state information on failure.
  Parameters:
       tree The sharefs partition.
       jrnl The sharefs journal.
  Returns:
      A zero (0) on success and a negative one (-1) on failure.
              )
Obtain a reference to the meta definition hashmap associated with the inode
entry.
  Note:
      The shfs_ino_t inode will cache the hashmap reference.
  Parameters:
       ent   The inode entry.
       val_p A memory reference to the meta definition hashmap being filled in.
                   )
Flush the inode's meta map to disk.
  Parameters:
       The inode associated with the meta map.
       val The meta map to store to disk.
  Returns:
      A zero (0) on success and a negative one (-1) on failure.
                   )
Obtain an exclusive lock to a process with the same process_path and
runtime_mode.
  Parameters:
       process_path The path to the process's executable file. (i.e. argv[0] in
                    main() or static string)
       runtime_mode An optional method to clarify between multiple process
                    locks.
  Returns:
      A zero (0) on success and a negative one (-1) on failure.
                       int          fd
                     )
Writes the file contents of the inode to the file stream.
  Parameters:
       tree  The sharefs filesystem partition allocation by shfs_init().
       inode The sharefs filesystem inode to print from.
       fd    A posix file descriptor number representing a socket or local
             filesystem file reference.
  Returns:
      A zero (0) on success and a negative one (-1) on error. On error one of
      the following error codes will be set: EBADF fd is not a valid file
      descriptor or is not open for writing.
  Examples:
      shfs_inode_remote_copy.c, and shfs_inode_remote_link.c.
AllData_StructuresVariables

===============================================================================
     Generated on 6 Apr 2013 for libshare by [doxygen] 1.6.1
