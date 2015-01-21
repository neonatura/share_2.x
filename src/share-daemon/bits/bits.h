
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

#define SHARENET_PROTOCOL_VERSION 1


#if 0
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
#endif


/**
*
*/
typedef struct tx_t
{
  /** A hash string referencing this tranction. */
  char hash[MAX_SHARE_HASH_LENGTH];
  /** The public peer key that initiated the transaction. */
  shkey_t tx_peer;
  /** The time-stamp pertaining to when the transaction was initiated. */
  shtime_t tx_stamp;
  /** The fee [in "shares"] neccessary to perform the transaction. */
  uint32_t tx_fee;
  /** The nonce index used to generate or verify the hash. */
  uint32_t nonce;
  /** The network protocol version of this transaction. */
  uint16_t tx_ver;
  /** Hash protocol used to generate transaction id.  */
  uint16_t tx_method;
  /** The error state of the transaction (SHERR_XXX). */
  uint16_t tx_state;
  /** The kind of transaction being referenced. */
  uint16_t tx_op;
} tx_t;

struct tx_trust_t 
{
  /** A transaction representing a current instance of the trust. */
  tx_t tx;
  /** A persistent transaction referencing the trust. */
  tx_t trust_tx;

  /** The time-stamp when the trust was generated. */
  shtime_t trust_stamp;
  /** The number of peer confirmations for the trust. */
  uint64_t trust_ref;

  /** A key id representing contextual data. */
  shkey_t trust_id;
  /** A key referencing the originating peer. */
  shkey_t trust_peer;
};
typedef struct tx_trust_t tx_trust_t; 

/**
 * Application-scope unique identity for account operations.
 */
struct tx_id_t 
{
  /** network transaction reference to identity */
  tx_t tx;

  /** permanent transaction reference to identity */
  tx_t id_tx;

  /** The account's name in reference to this identity. */
  char id_label[MAX_SHARE_NAME_LENGTH];

  /** An auxillary hash string associated with the identity. */
  char id_hash[MAX_SHARE_HASH_LENGTH];

  /** The application the identity is registered for. */
  shpeer_t id_peer;

  /** A key reference to the associated account. */
  shkey_t id_seed;

  /** A key reference to a particular identity */
  shkey_t id_key;

};
typedef struct tx_id_t tx_id_t; 

/** 
 * An "identity" that holds "identities".
 */ 
struct tx_account_t 
{
	/** transaction representing single instance of account. */
	tx_t tx; 

	/** a sha256 hash representing this account */
  tx_t acc_tx;

  /** A key reference to the account's "username". */
  shkey_t acc_user;

  /** A seed for generating an authorized session token. */
  shkey_t acc_seed;
};
typedef struct tx_account_t tx_account_t;

struct tx_app_t 
{

  /** network transaction */
  tx_t tx;
  /** transaction reference of app instance */
  tx_t app_tx;

  /** application's peer identifier. */
  shpeer_t app_peer;
  /** application birth timestamp */
  shtime_t app_birth;
  /** application 'last successful validation' time-stamp. */
  shtime_t app_stamp;
  /** application signature key */
  shkey_t app_sig;
  /** application's supplemental context */
  shkey_t app_context;
  /* application flags (SHAPP_XXX) */ 
  uint32_t app_flags;
  /** arch of app origin (SHARCH_XXX) */
  uint32_t app_arch;
  /** total app reference server confirmations. */
  uint32_t app_hop;
  /** 'successful app validations' minus 'unsuccessful app validations' */
  uint32_t app_trust;
};
typedef struct tx_app_t tx_app_t; 

/**
 *
 */
struct tx_ledger_t
{
  /* the transaction id associated with this ledger entry. */
  tx_t tx;
  /* a transaction representing of this ledger entry. */
  tx_t ledger_tx;
  /* the ledger entry with the preceding sequence number. */
  char parent_hash[MAX_SHARE_HASH_LENGTH];
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
  tx_t ledger[0];
};
typedef struct tx_ledger_t tx_ledger_t;


typedef struct tx_ward_t 
{
  /** ward network transaction */
  tx_t tx;
  /** unique transaction referencing the ward */
  tx_t ward_tx;

  /** The transaction operation this ward is suppressing. */
  uint16_t ward_op; 
  /** The transaction hash of the operation being warded. */
  char ward_hash[MAX_SHARE_HASH_LENGTH];

  /** timestamp when ward was assigned. */
  shtime_t ward_stamp;
  /** originating ward identity */
  tx_id_t ward_id;
  /** ward signature validation (ward_stamp + ward_id) */
  shsig_t ward_sig; 
} tx_ward_t;

typedef struct tx_event_t
{
  tx_t tx;
  tx_t event_tx;
  shpeer_t event_peer;
  shtime_t event_stamp;
  shsig_t event_sig;
} tx_event_t;

typedef struct tx_bond_t
{
  /** bond network transaction */
  tx_t tx;
  /** unique transaction referencing the bond */
  tx_t bond_tx;
  
  /** Hash reference to currency destination. */
  char bond_sink[MAX_SHARE_HASH_LENGTH];
  /** supplementary comment */
  char bond_label[MAX_SHARE_NAME_LENGTH];
  /** Bond source session token. */
  shkey_t bond_sess;
  /** A signature confirmation of the bond. */
  shsig_t bond_sig;
  /** When the bond was initiated. */
  shtime_t bond_stamp;
  /** When the bond matures. */
  shtime_t bond_expire;
  /** USDe currency amount value of bond. */
  uint64_t bond_credit;
  /** The interest rate described in basis points. */
  uint64_t bond_basis;
  
} tx_bond_t;


struct tx_peer_t 
{
  /* a transaction representing a peer instance. */
  tx_t tx;
  
  /* a persistent transaction representing the peer. */
  tx_t peer_tx;

  /* the peer being referenced. */
  shpeer_t peer;
};
typedef struct tx_peer_t tx_peer_t;


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


struct tx_license_t
{
  /** Network transaction reference of this license. */
  tx_t tx;
  /** Permanent transaction reference of this license. */
  tx_t lic_tx;
  /** The originating peer granting the license. */  
  shpeer_t lic_peer;
  /** The digital signature the licence is granting access for. */
  shsig_t lic_sig;
  /** The key reference to the licensing content. */
  shkey_t lic_name;
  /** The identity that the license is applicable for. */
  shkey_t lic_id;
  /** A key referencing this license instance. */
  shkey_t lic_key;
  /** When the license expires. */
  shtime_t lic_expire;
};
typedef struct tx_license_t tx_license_t;




typedef struct tx_session_t
{
  /** Network transaction reference of this session. */
  tx_t tx;
  /** Permanent transaction reference for this session. */
  tx_t sess_tx;
  /** The identity the session is authorized for. */
  shkey_t sess_id;
  /** Session's certificate token key. */
  shkey_t sess_key;
  /** When the session expires. */
  shtime_t sess_stamp;
} tx_session_t;



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
#include "session.h"



/**
 * @}
 */

#endif /* ndef __BITS__BITS_H__ */

