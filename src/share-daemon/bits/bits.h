
#ifndef __BITS__BITS_H__
#define __BITS__BITS_H__
/**
 * Network transaction operations
 * @brief shareademon_bits Network transaction operations
 * @addtogroup sharedaemon
 * @{
 */

#include "share.h"
//#include "../sharedaemon.h"

/** 64 characters. */
#define HASH_STRING_LENGTH 72


#define MIN_TX_ONCE 256U

#define MAX_TX_ONCE 2147483647U 

#define MAX_TRANSACTIONS_PER_LEDGER 64

#define MAX_SCHEDULE_TASKS 4096

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
typedef struct sh_tx_t
{
  char hash[HASH_STRING_LENGTH];
  /** The originating peer that initiated the transaction. */
  shkey_t tx_peer;
  uint64_t tx_id;
  /** Specifies a sharenet group id, i.e. @see TX_GROUP_PUBLIC(), TX_GROUP_PEER(), TX_GOUP_PRIVATE(), TX_GROUP(). */
  uint64_t tx_group;
  /** The time-stamp pertaining to when the transaction was initiated. */
  uint64_t tx_stamp;
  /** The fee [in "shares"] neccessary to perform the transaction. */
  uint64_t tx_fee;
  /** The error state of the transaction (SHERR_XXX). */
  uint32_t tx_state;
  uint32_t tx_op;
  uint32_t tx_prio;
  uint32_t nonce;
} sh_tx_t;

/**
*
*/
typedef struct sh_id_t 
{
  char hash[HASH_STRING_LENGTH];
  sh_tx_t tx;
  shkey_t key_pub;
  shkey_t key_peer;
  shkey_t key_priv;
} sh_id_t;

/** 
 * An "identity" that holds "identities".
 */ 
typedef struct sh_account_t 
{
	/** confirmation of account identity */
	sh_tx_t tx; 
	/** the "root" identity */
  sh_id_t id;
	/** the number of peers which have confirmed this account. */
	uint32_t confirm;
	/** a sha256 hash representing this account */
  char hash[HASH_STRING_LENGTH];
} sh_account_t;



typedef struct sh_app_t 
{

  /** transaction for the app's current operation. */
  sh_tx_t tx;
  /** identity origin of the app. */
  sh_id_t id;
  /** unique application identifier. */
  shkey_t app_name;
  /** transaction representing the app's current instance. */
  sh_tx_t app_tx;
  /** supported architectures (ARCH_XX flags). */
  uint32_t app_arch; 

} sh_app_t;



/**
*
*/
typedef struct sh_ledger_t
{
  /* the transaction id associated with this ledger entry. */
  sh_tx_t tx;
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
  /* a block of @see sh_ledger_t.ledger_height transactions. */ 
  sh_tx_t ledger_tx[0];
} sh_ledger_t;

typedef struct sh_sig_t
{
  shkey_t sig_id;
  shkey_t sig_peer;
  shkey_t sig_key;
  uint64_t sig_stamp;
  uint32_t sig_ref;
  char sig_tx[HASH_STRING_LENGTH];
} sh_sig_t;

typedef struct sh_task_t
{
  sh_tx_t tx;
  shbuf_t *buf;
  sh_sig_t sig;
} sh_task_t;


typedef struct tx_thread_t 
{

  /** The thread's unique transaction identifier. */
  sh_tx_t tx;
  /** The transaction id of the originating application operation. */
  sh_tx_t app_tx;
  /** The application the thread is originating from. */
  shkey_t app_name;
  /** Hash code representing the proof of thread execution (stamp+ret_code). */
  char thread_hash[HASH_STRING_LENGTH];
  /** The machine platform associated with this thread's execution. */
  uint32_t thread_arch;
  /** The length of the thread's processing stack. */
  uint32_t thread_stacklen;
  /** The content of the thread's proccessing stack. */
  uint8_t thread_stack[0];

} tx_thread_t;


typedef struct sh_trust_t 
{
  shkey_t trust_id;
  shkey_t trust_peer;
  shkey_t trust_key;
  char trust_tx[HASH_STRING_LENGTH];
  uint64_t trust_stamp;
  uint32_t trust_ref;
} sh_trust_t;






typedef struct sh_ward_t {
  sh_tx_t tx;
  sh_tx_t ward_tx;
  sh_id_t ward_id;
} sh_ward_t;


typedef struct sh_event_t {
  sh_tx_t tx;
  sh_tx_t event_tx;
  sh_id_t event_id;
  uint64_t event_stamp;
} sh_event_t;



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

