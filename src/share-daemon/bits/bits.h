
#ifndef __BITS__BITS_H__
#define __BITS__BITS_H__
/**
 * Network transaction operations
 * @brief shareademon_bits Network transaction operations
 * @addtogroup sharedaemon
 * @{
 */

#include <share.h>
#include <sexe.h>

#define MIN_TX_ONCE 256U

#define MAX_TX_ONCE 2147483647U 

#define MAX_TRANSACTIONS_PER_LEDGER 64

#define MAX_SCHEDULE_TASKS 4096

#define MAX_HASH_STRING_LENGTH 72



#define TX_NONE    0
#define TX_IDENT   1
#define TX_PEER    2 
#define TX_FILE    3
#define TX_WALLET  4
/**
 * A ward can be placed on another transaction to prohibit from being used.
 * @note Applying the identical ward causes the initial ward to be removed.
 */
#define TX_WARD 5

/**
 * 
*/
#define TX_SIGNATURE 6
#define TX_LEDGER 7



/**
 * Windows 32bit
 */
#define ARCH_W32 (1 << 0)
/**
 * Windows 64bit
 */
#define ARCH_W64 (1 << 1)
/**
 * Linux 32bit
 */
#define ARCH_L32 (1 << 2)
/**
 * Linux 64bit
 */
#define ARCH_L64 (1 << 3)
/**
 * Standard android MIPS chipset.
 */
#define ARCH_A32 (1 << 4)


/**
 * A predefined sharenet group restricted from access to this daemon only.
 */
#define TX_GROUP_PRIVATE \
  (shcrc("local", strlen("local")))

/**
 * A predefined sharenet group which only incorporates predisposed trust entities. */
#define TX_GROUP_PEER \
  (shcrc("peer", strlen("peer")))

/**
 * The default public-access sharenet group.
 */
#define TX_GROUP_PUBLIC \
  (shcrc("public", strlen("public")))

/**
 * Generates a sharenet group identifier from a string label. 
 * @returns A 64bit checksum representing a sharenet group.
 */
#define TX_GROUP(_label) \
  (shcrc((_label), strlen(_label)))

/**
*
*/
typedef struct tx_t
{
  /** The network protocol version of this transaction. */
  uint8_t tx_ver;
  /** Hash protocol used to generate transaction id.  */
  uint8_t tx_method;
  /** A hash string referencing this tranction. */
  char hash[MAX_HASH_STRING_LENGTH];
  /** The public peer key that initiated the transaction. */
  shkey_t tx_peer;
  /** Specifies the transaction is limited to a peer group. */
  uint64_t tx_group;
  /** The time-stamp pertaining to when the transaction was initiated. */
  shtime_t tx_stamp;
  /** The fee [in "shares"] neccessary to perform the transaction. */
  uint16_t tx_fee;
  /** The error state of the transaction (SHERR_XXX). */
  uint16_t tx_state;
  /** The kind of transaction being referenced. */
  uint16_t tx_op;
  /** Priority of transaction being processed. */
  uint16_t tx_prio;
  /** The nonce index used to generate or verify the hash. */
  uint32_t nonce;
} tx_t;

/**
*
*/
typedef struct tx_id_t 
{

  tx_t tx;

  tx_t id_tx;
  shtime_t id_stamp;
  shkey_t key_pub;
  shkey_t key_peer;
  shkey_t key_priv;

} tx_id_t;

/** 
 * An "identity" that holds "identities".
 */ 
typedef struct tx_account_t 
{
	/** transaction representing single instance of account. */
	tx_t tx; 

	/** a sha256 hash representing this account */
  tx_t acc_tx;
	/** the "root" identity */
  tx_id_t acc_id;
	/** the number of peers which have confirmed this account. */
	uint32_t acc_confirm;

} tx_account_t;



typedef struct tx_app_t 
{

  /** network transaction */
  tx_t tx;

  /** transaction of app's signature. */
  tx_t app_tx;
  /** unique application identifier. */
  shkey_t app_name;
  /** application birth timestamp */
  shtime_t app_stamp;
  /** application signature key */
  shkey_t app_sig;
  /** identity origin of the app. */
  tx_id_t app_id;
  /** arch of app origin. */
  uint32_t app_arch;
  /** number of confirmations of app's instance. */
  uint32_t app_confirm;

} tx_app_t;



/**
*
*/
typedef struct tx_ledger_t
{
  /* the transaction id associated with this ledger entry. */
  tx_t tx;
  /* a hash representation of this ledger entry. */
  char hash[64];
  /* the ledger entry with the preceding sequence number. */
  char parent_hash[64];
  /* the time-stamp of when the ledger was closed. */
  uint64_t ledger_stamp;
  /* the total fees of the combined transactions. */
  uint64_t ledger_fee;
  /* the numbers of ledger entries in this chain. */
  uint32_t ledger_seq;
  /* the number of hops that have confirmed this ledger. */
  uint32_t ledger_confirm;
  /* the number of transactions in this ledger entry. */
  uint32_t ledger_height;
  /* a block of @see tx_ledger_t.ledger_height transactions. */ 
  tx_t ledger_tx[0];
} tx_ledger_t;








typedef struct tx_trust_t 
{

  /** A transaction representing a current instance of the trust. */
  tx_t tx;

  /** A key id representing contextual data. */
  shkey_t trust_id;
  /** A key referencing the originating peer. */
  shkey_t trust_peer;
  /** A key representing the trust. */
  shkey_t trust_key;

  /** A persistent transaction referencing the trust. */
  tx_t trust_tx;
  /** The time-stamp when the trust was generated. */
  uint64_t trust_stamp;
  /** The number of peer confirmations for the trust. */
  uint32_t trust_ref;

} tx_trust_t;




typedef struct tx_ward_t {
  tx_t tx;
  tx_t ward_tx;
  tx_id_t ward_id;
  shtime_t ward_stamp;
  shpeer_t ward_peer;
  shsig_t ward_sig; 
} tx_ward_t;


typedef struct tx_event_t {
  tx_t tx;

  tx_t event_tx;
  shpeer_t event_peer;
  shtime_t event_stamp;
  shsig_t event_sig;
  uint32_t event_confirm;
} tx_event_t;

typedef struct tx_peer_t 
{
  /* a transaction representing a peer instance. */
  tx_t tx;
  
  /* a persitent transaction representing the peer. */
  tx_t peer_tx;

  /* a trust referencing the peer. */
  tx_trust_t peer_trust;

  /* the peer being referenced. */
  shpeer_t peer;

} tx_peer_t;

#define TXFILE_NONE 0
#define TXFILE_READ 1
#define TXFILE_WRITE 2
#define TXFILE_LIST 3
#define TXFILE_LINK 4
#define TXFILE_UNLINK 5
#define TXFILE_CHECKSUM 6

typedef struct tx_file_t
{
  /** a transaction id for the file operation. */
  tx_t tx;

  /** a transaction id for the file entity. */
  tx_t ino_tx;
  /** The shfs peer identity. */
  shpeer_t ino_peer;
  /** The shfs inode being referenced. */
  shfs_hdr_t ino;
  /** The inode operation being requested. */
  uint32_t ino_op;
  /** The time that the operation was requested. */
  shtime_t ino_stamp;

  uint32_t ino_size;
  uint32_t ino_of;
  uint8_t ino_data[0];
} tx_file_t;

typedef struct tx_sig_t {
  tx_t tx;
  shsig_t sig;
} tx_sig_t;











/** A virtual 64-bit memory-address operation. */
struct tx_mem_t
{
  /** A network tranaction referencing this memory operation. */
  tx_t tx;

  /** A transaction referencing with this memory address. */
  tx_t mem_tx;

  /** The destination thread of the memory address operation. */
  shkey_t mem_sink;

  /** The instruction memory operation being performed. */
  uint32_t mem_op;

  /** A status code for the memory operation. */
  uint32_t mem_status;

  /** The memory address's attributes. */
  sexe_mem_t mem;

  /** The content of the associated data payload. */
  unsigned char mem_data[0];
};
typedef struct tx_mem_t tx_mem_t;

/** A vm thread network operation. */
struct tx_thread_t
{

  /** Network transaction referencing the thread. */
  tx_t tx;
 
  /** The thread's unique transaction identifier. */
  tx_t th_tx;

  /** A unique id representing the thread. */
  shkey_t th_id;

  /** The privileged key of the app which initiated the task. */
  shpeer_t th_app;

  /** The timestamp when the thread completed. */
  shtime_t th_stamp;

  /** A proof-of-work signature. */
  shsig_t th_sig;

  /** The result code computed by the task. */
  tx_mem_t th_status;

  sexe_thread_t th;
};
typedef struct tx_thread_t tx_thread_t;

/** A thread task network operation. */
struct tx_task_t
{
  /** A network transaction representing the thread. */
  tx_t tx;

  /** A transaction representing this thread. */
  tx_t job_tx;

  /* the task operation to perform. */
  uint16_t task_op;

  /** The task's attributes. */
  sexe_task_t task;
};
typedef struct tx_task_t tx_task_t;

/** A network operation on a session's job. */
struct tx_job_t
{
  /** A network transaction representing the thread. */
  tx_t tx;

  /** A transaction representing this thread. */
  tx_t job_tx;

  /** A status (error code) in reference to the job. */
  uint32_t job_status;

  /** The job operation being performed. */
  uint16_t job_op;

  /** The priveleged key of the process which initiated the task. */
  shpeer_t job_app;

  /** The job attributes. */
  sexe_job_t job;
};
typedef struct tx_job_t tx_job_t;

/** A vm session network operation. */
typedef struct tx_sess_t
{
  /** A network transaction referencing a 'vm session' operation. */
  tx_t tx;
  /** The persistent transaction referencing the 'vm session' instance. */
  tx_t vm_tx;
  /** The file containing the 64-bit shared mem address heap/data. */
  tx_file_t sess_heap;
  /** The session's attributes. */
  sexe_sess_t sess;
} tx_sess_t;

/** A 'virtual machine' network operation. */
typedef struct tx_vm_t
{

  /** A network transaction referencing a 'virtual machine' operation. */
  tx_t tx;
  /** The persistent transaction referencing the 'virtual machine' instance. */
  tx_t vm_tx;
  /** The 'virtual machine' operation to perform. */
  uint32_t vm_op;
  /** The virtual machine's attributes. */
  sexe_vm_t vm;

} tx_vm_t;






#include "account.h"
#include "app.h"
#include "bits.h"
#include "identity.h"
#include "ledger.h"
#include "schedule.h"
#include "signature.h"
#include "thread.h"
#include "transaction.h"
#include "trust.h"
#include "ward.h"
#include "event.h"



/**
 * @}
 */

#endif /* ndef __BITS__BITS_H__ */

