<pre>



A meta definition is part of a shmeta_t hashmap. More...
                           struct shmeta_entry_t
                           struct shmeta_index_t
                                  Data structure for iterating through a hash
                                  table. More...
                           struct shmeta_t
                                  The size of the array is always a power of
                                  two. More...
                           struct shmeta_value_v1_t
                                  The base of a version 1 shmeta hashmap entry
                                  value. More...
                          #define INITIAL_MAX 15
                                  The initial number of hashmap indexes to
                                  create.
                          #define SHMETA_VALUE_MAGIC 0x12345678
                                  Specifies a hard-coded value that identifies
                                  a shmeta_value_t data segment.
                          #define SHMETA_VALUE_NET_MAGIC htons(0x12345678)
                                  The network byte order representation of
                                  SHMETA_VALUE_MAGIC.
                          #define SHMETA_BIG_ENDIAN 0
                                  Specifies that a machine has a big endian
                                  architecture.
                          #define SHMETA_SMALL_ENDIAN 1
                                  Specifies that a machine has a small endian
                                  architecture.
                          #define SHMETA_VALUE_ENDIAN(_val)
                                  Determines whether the meta value originated
                                  from a big or small endian architecture.
                          #define SHPF_NONE 0
                                  A shmeta_value_t parameter specific to a
                                  indeterminate data segment.
                          #define SHPF_STRING 1
                                  A shmeta_value_t parameter specific to a
                                  null-terminated string value.
          typedef struct shmeta_t shmeta_t
                                  A hashmap table.
    typedef struct shmeta_index_t shmeta_index_t
                                  A hashmap index.
                                  Callback functions for calculating hash
                                  values.
    typedef struct shmeta_entry_t shmeta_entry_t
                                  A hashmap entry.
 typedef struct shmeta_value_v1_t shmeta_value_t
                                  Specifies a reference to the current version
                                  of a shmeta hashmap entry value.
                                  Create an instance of a meta definition
                                  hashmap.
                                  Free an instance of a meta definition
                                  hashmap.
                                  The default hashmap indexing function.
                                  Set a meta definition to a particular value.
                                  Get a meta definition value.
                                  Prints out a JSON representation of a meta
                                  definition hashmap.
                                  Creates a shmeta_value_t hashmap value from a
                                  string.
===============================================================================
A meta definition is part of a shmeta_t hashmap.
The share library meta definitions can be used to hash header information from
a socket stream, retaining access to the meta information by a token, and
allowing for efficient redelivery or caching.
In reference to the internal form of a hash table:
The table is an array indexed by the hash of the key; collisions are resolved
by hanging a linked list of hash entries off each element of the array.
Although this is a really simple design it isn't too bad given that pools have
a low allocation overhead.
  Note:
      A shfs_tree sharefs file system associates meta definition information
      with every shfs_node inode entry.
===============================================================================
#define SHMETA_BIG_ENDIAN 0
Specifies that a machine has a big endian architecture.
  See also:
      SHMETA_VALUE_ENDIAN
Definition at line 70 of file shmeta.h.
#define SHMETA_SMALL_ENDIAN 1
Specifies that a machine has a small endian architecture.
  See also:
      SHMETA_VALUE_ENDIAN
Definition at line 76 of file shmeta.h.
#define SHMETA_VALUE_ENDIAN ( _val  )
Value:
(_val->magic == SHMETA_VALUE_NET_MAGIC ? \
   SHMETA_BIG_ENDIAN : SHMETA_SMALL_ENDIAN)
Determines whether the meta value originated from a big or small endian
architecture.
  Returns:
      SHMETA_BIG_ENDIAN or SHMETA_SMALL_ENDIAN based on the meta value.
Definition at line 82 of file shmeta.h.
===============================================================================
Callback functions for calculating hash values.
  Parameters:
       key  The key.
       klen The length of the key.
Definition at line 111 of file shmeta.h.
===============================================================================
Free an instance of a meta definition hashmap.
  Parameters:
       meta_p A reference to the meta definition hashmap to be free'd.
                   shkey_t    sh_k
                 )
Get a meta definition value.
  Parameters:
       ht   The meta definition hashmap to retrieve from.
       sh_k The key of the meta definition value.
Create an instance of a meta definition hashmap.
  Returns:
      A shmeta_t meta definition hashmap.
                  )
Prints out a JSON representation of a meta definition hashmap.
  Note:
      The text buffer must be allocated by shbuf_init() first.
  Parameters:
       h        The meta map to print.
       ret_buff The text buffer to return the JSON string representation.
                  shkey_t          sh_k,
                )
Set a meta definition to a particular value.
  Parameters:
       ht   The meta definition hashmap to retrieve from.
       sh_k The key of the meta definition value.
       val  The meta definition value using a shmeta_value_t as a header.
Creates a shmeta_value_t hashmap value from a string.
shmeta_set
  Parameters:
       str The string to generated into a hashmap value.
  Returns:
      A meta definition shmeta_value_t value.
AllData_StructuresVariables

===============================================================================
     Generated on 6 Apr 2013 for libshare by [doxygen] 1.6.1
</pre>
