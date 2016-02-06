
#ifndef __BITS__BITS_H__
#define __BITS__BITS_H__
/**
 * Network transaction operations
 * @brief shareademon_bits Network transaction operations
 * @addtogroup sharedaemon
 * @{
 */

#include "share.h"
#include "sexe.h"


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
 * Network state of transaction.
 */
typedef struct tx_net_t
{

  /** The destination peer or empty on a broadcast. */
  shkey_t tx_sink;

  /** The time-stamp of when transaction was prepared for transmission. */
  shtime_t tx_stamp;

  /** A checksum of the transaction header. */
  uint32_t tx_crc;

  uint32_t tx_magic;

  uint32_t tx_size;

  /** encoded format of transaction data */
  uint32_t tx_proto;

} tx_net_t;

/**
 * The base information for a transaction.
 */
typedef struct tx_t
{

  /** A hash string referencing this tranction. */
  char hash[MAX_SHARE_HASH_LENGTH];

  /** The public peer key that initiated the transaction. */
  shkey_t tx_peer;

  /** The time-stamp pertaining to when the transaction was initiated. */
  shtime_t tx_stamp;

#if 0
  /** The usde coin fee neccessary to perform the transaction. */
  uint64_t tx_fee;
#endif

  /** The kind of transaction being referenced. */
  uint32_t tx_op;

  /** The nonce index used to generate or verify the hash. */
  uint32_t nonce;

  /** Hash protocol used to generate transaction id.  */
  uint32_t tx_method;

  uint32_t __reserved_1__;
} tx_t;

struct tx_subscribe_t
{
  tx_t sub_tx;
  shpeer_t sub_peer;
  shkey_t sub_key;
  uint32_t sub_op;
  uint32_t sub_flag;
};
typedef struct tx_subscribe_t tx_subscribe_t;

/** require additional trust transaction */
#define TXF_TRUST (1 << 0)
struct tx_trust_t 
{
  /** A persistent transaction referencing the trust. */
  tx_t trust_tx;

  /** A signature validating the trust. */
  shkey_t trust_sig;

  /** A key id representing contextual data. */
  shkey_t trust_context;

  /** A key referencing the originating peer. */
  shkey_t trust_peer;
};
typedef struct tx_trust_t tx_trust_t; 

/**
 * Application-scope unique identity for account operations.
 */
struct tx_id_t 
{

  /** permanent transaction reference to identity */
  tx_t id_tx;

  /** The application the identity is registered for. */
  shpeer_t id_peer;

  /** A key referencing this identity's underlying information. */
  shkey_t id_key;

  /** When the identity was initially generated. */
  shtime_t id_stamp;

  /** The account user id. */
  uint64_t id_uid;

#if 0 
  char id_name[MAX_SHARE_NAME_LENGTH];
  char id_host[MAX_SHARE_NAME_LENGTH];
  char id_email[MAX_SHARE_NAME_LENGTH];
  /** known phone number */
  char id_phone[16]; /* (([2-9][0-8]\d-[2-9]\d{2}-[0-9]{4})|(([0-9]?){6,14}[0-9])) */

  /* known birth-date */
  time_t id_birth;

  /* known location */
  shgeo_t id_geo;
#endif

};
typedef struct tx_id_t tx_id_t; 

/** 
 * An "identity" that holds "identities".
 */ 
struct tx_account_t 
{

	/** a sha256 hash representing this account */
  tx_t acc_tx;

  /** The seed used to generate the account identity key. */
  shseed_t pam_seed;
};
typedef struct tx_account_t tx_account_t;

struct tx_app_t 
{

  /** transaction reference of app instance */
  tx_t app_tx;

  /** application's peer identifier. */
  shpeer_t app_peer;
  /** application birth timestamp (remove me, in tx) */
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
  /** 'successful app validations' minus 'unsuccessful app validations' */
  uint32_t app_trust;
uint32_t __reserved_1__;
};
typedef struct tx_app_t tx_app_t; 

struct tx_init_t
{

  /** transaction reference of iniorization notification */
  tx_t ini_tx;

  /** originating peer */
  shpeer_t ini_peer;

  /** Machine byte-order directive. */
  uint32_t ini_endian;

  /** The network protocol version of this transaction. */
  uint32_t ini_ver;

  /* The sequence number of the handshake operation */
  uint32_t ini_seq;

  uint32_t __reserved_1__;

  shtime_t ini_stamp;

  /* A checksum hash of the initialization info */
  char ini_hash[MAX_SHARE_HASH_LENGTH];

};
typedef struct tx_init_t tx_init_t;

/**
 * A ledger contains a list of transactions generated for a peer.
 */
struct tx_ledger_t
{
  /* a transaction representing of this ledger entry. */
  tx_t ledger_tx;
  /* the ledger entry with the preceding sequence number. */
  char parent_hash[MAX_SHARE_HASH_LENGTH];
  /** The peer that generated the ledger transactions. */
  shpeer_t ledger_peer;
  /** A signature validating a closed ledger. */
  shkey_t ledger_sig;
  /* the time-stamp of when the ledger was closed. */
  uint64_t ledger_stamp;
  /* the total fees of the combined transactions. */
  uint64_t ledger_fee;
  /* the numbers of ledger entries in this chain. */
  uint32_t ledger_seq;
  /* the number of transactions in this ledger entry. */
  uint32_t ledger_height;
  /* a block of @see tx_ledger_t.ledger_height transactions. */ 
  tx_t ledger[0];
};
typedef struct tx_ledger_t tx_ledger_t;


typedef struct tx_ward_t 
{
  /** unique transaction referencing the ward */
  tx_t ward_tx;

  /** The transaction operation this ward is suppressing. */
  uint16_t ward_op; 
  /** The transaction hash of the operation being warded. */
  char ward_hash[MAX_SHARE_HASH_LENGTH];

  /** ward peer */
  shpeer_t ward_peer;
  /** ward signature validation (ward_stamp + ward_id) */
  shsig_t ward_sig; 
  /** originating ward identity */
  tx_id_t ward_id;
  /** timestamp when ward was assigned. */
  shtime_t ward_stamp;
} tx_ward_t;

typedef struct tx_event_t
{
  tx_t event_tx;
  shpeer_t event_peer;
  shtime_t event_stamp;
  shsig_t event_sig;
} tx_event_t;

#define TXBOND_NONE 0
#define TXBOND_PENDING 10
#define TXBOND_CONFIRM 20
#define TXBOND_FINAL 30
#define TXBOND_SYNC 40
#define TXBOND_COMPLETE 50

#define BOND_CREDIT_VALUE(_value) \
  ( (double)(_value) * 0.00000001 )



typedef struct tx_bond_t
{
  /** unique transaction referencing the bond */
  tx_t bond_tx;
  
  /** Hash reference to currency destination. */
  char bond_sink[MAX_SHARE_HASH_LENGTH];
  /** supplementary comment */
  char bond_label[MAX_SHARE_NAME_LENGTH];
  /** A signature confirmation of the bond. */
  shkey_t bond_sig;
  /** When the bond was initiated. */
  shtime_t bond_stamp;
  /** When the bond matures. */
  shtime_t bond_expire;
  /** USDe currency amount value of bond. */
  uint64_t bond_credit;
  /** The accumulated confirmed credit value of bond. */
  uint64_t bond_value;
  /** The current state of the bond. */
  uint32_t bond_state;
  /** The interest rate described in 100th of a basis point. */
  uint32_t bond_basis;
  
} tx_bond_t;


struct tx_peer_t 
{
  
  /* a persistent transaction representing the peer. */
  tx_t peer_tx;

  /* the peer being referenced. */
  shpeer_t peer;

  /* the system-time of the originating server */
  shtime_t peer_stamp;
};
typedef struct tx_peer_t tx_peer_t;


#define SHMETRIC_NONE 0
#define SHMETRIC_CARD 1
#define SHMETRIC_ZTEX 2

struct tx_metric_t
{
  /** a persistent transaction representing the metric. */
  tx_t met_tx;

  /** The type of metric (SHMETRIC_XX). */
  uint32_t met_type;

  /** Type specific flags for the metric instance. */
  uint32_t met_flags; 

  /** A self-identifying sub-type of the metric. */
  char met_name[8];

  /** The time-stamp of when the metric expires. */
  shtime_t met_expire;

  /** The account being referenced. */
  uint64_t met_acc; 

  /** A key reference to this instance. */
  shkey_t met_id;

  /*
   * A signature validating the metric instance.
   */ 
  shkey_t met_sig;
};
typedef struct tx_metric_t tx_metric_t;


struct tx_wallet_t
{
  tx_t wal_tx;

  /* A self-identifying name. */
  char wal_name[16];

  /* The identity associated with this wallet. */
  shkey_t wal_id;

  /* A signature verifying the integrity of this wallet account. */
  shkey_t wal_sig;
};
typedef struct tx_wallet_t tx_wallet_t;

/** A no-op operation. */
#define TXFILE_NONE 0
/** A binary data segment request operation. */
#define TXFILE_READ 1
/** A binary data segment transmission operation. */
#define TXFILE_WRITE 2
/** An inode container list operation. */
#define TXFILE_LIST 3
/** An inode link operaton. */
#define TXFILE_LINK 4
/** An inode unlink operation. */
#define TXFILE_UNLINK 5
/** An inode synchronization request operation. */
#define TXFILE_CHECKSUM 6
/** An inode operation indicating synchronized. */
#define TXFILE_SYNC 6
/** A transmission fee negotiation operation. */
#define TXFILE_FEE 7

#define CALC_TXFILE_FEE(_size, _create) \
  (double)(0.00000001 * (double)_size / shtimef(shtime() - _create))
#define NO_TXFILE_FEE(_value) \
  ((_value) < 0.00000001)

  

typedef struct tx_file_t
{

  /** a transaction id for the file entity. */
  tx_t ino_tx;

  char ino_path[SHFS_PATH_MAX];
  /** The shfs peer identity. */
  shpeer_t ino_peer;
  /** The shfs inode being referenced. */
  shfs_hdr_t ino;
  /** The inode operation being requested. */
  uint32_t ino_op;
  /** The time that the operation was requested. */
  shtime_t ino_stamp;

  uint64_t ino_crc;
  uint32_t ino_size;
  uint32_t ino_of;
  uint8_t ino_data[0];
} tx_file_t;


struct tx_license_t
{
  /** Permanent transaction reference of this license. */
  tx_t lic_tx;
  /** The certificate that is used to grant the license. */
  shcert_t lic_cert;
  /** The license being referenced. */
  shlic_t lic;
  /** The identity that the license is applicable for. */
  shkey_t lic_id;
};
typedef struct tx_license_t tx_license_t;

struct tx_device_t
{
  tx_t dev_tx;

  /* The X.500 distinguished name of the computing device being identified. */
  char dev_name[16];

  char dev_host[16];

  uint16_t dev_vendor;
  uint16_t dev_product;

  uint64_t dev_net[4];

};
typedef struct tx_device_t tx_device_t;

struct tx_host_t
{

  tx_t host_tx;

  char host_name[64];
  uint64_t host_net_of[4];
  uint64_t host_net_max[4];
  uint64_t host_mac[8];

  
};
typedef struct tx_host_t tx_host_t;

struct tx_asset_t
{
  /** Permanent transaction reference to this asset. */
  tx_t ass_tx;

  char host_url[MAX_SHARE_HASH_LENGTH]; /* [a-zA-Z]{2,3}(-([a-zA-Z]{2}|[0-9]{3}))? */

  /* external asset barcode reference */
  char ass_code[16];

  char ass_locale[16]; /* [a-zA-Z]{2,3}(-([a-zA-Z]{2}|[0-9]{3}))? */

  /** The license granting ownership of the asset. */
  shkey_t ass_lic;

  /** The location where the asset resides. */
  shgeo_t ass_loc;

  /* identity key of originating creator */
  shkey_t ass_id;

  /** A signature key verifying the underlying contents. */
  shkey_t ass_sig;

  /** Time-stamp of when asset was created. */
  shtime_t ass_birth;

  /** When the information was last known to be correct. */
  shtime_t ass_stamp;

  /** Type of asset (TX_BOND, TX_TITLE, TX_LICENSE) */
  uint32_t ass_type;

  /** asset content byte size */
  uint32_t ass_size;

  /* asset content */
  unsigned char ass_data[0];
};
typedef struct tx_asset_t tx_asset_t;

/* The circuit is terminated by the device. */
#define SHLINK_TERMINATION_DEVICE (1 << 0)
/* The circuit is owner/operated by the organization or the service is provided by the software. */
#define SHLINK_SERVICE_PROVIDER (1 << 1) /* circuit/organization service/software */
/* The circuit ends at the network. */
#define SHLINK_NET_TERM_POINT (1 << 2)
/* The database or website is served by the service. */
#define SHLINK_SERVED_BY (1 << 3)
/* The software is installed on the computing device. */
#define SHLINK_ON_DEVICE (1 << 4)
/* The system is connected to the network. */
#define SHLINK_NET_CONNECTION (1 << 5)
/* The person or organization owns the IT asset. */
#define SHLINK_OWNER (1 << 6)
/* The person is the system administrator of the computing device or system. */
#define SHLINK_ADMINISTRATOR (1 << 7)
/* The person is in some way a part of the organization. */
#define SHLINK_INVOLVED (1 << 8)
/* The computing device or system is connected to the system. */
#define SHLINK_CONNECTION (1 << 9)

struct tx_link_t
{
  tx_t tx;
  int link_flag;
  shkey_t link_type; /* TX_XXX */
  shkey_t link_key;
  shkey_t link_ref;
};
typedef struct tx_link_t tx_link_t;


typedef struct tx_session_t
{
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


/**
 * A thread runs a pre-defined SEXE task as a vm runtime operation. 
 */
struct tx_thread_t
{

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

/**
 * A declaration of a runtime operation.
 */
struct tx_task_t
{

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
  /** The persistent transaction referencing the 'vm session' instance. */
  tx_t sess_tx;
  /** The file containing the 64-bit shared mem address heap/data. */
  tx_file_t sess_heap;
  /** The session's attributes. */
  sexe_sess_t sess;
} tx_sess_t;

/** A 'virtual machine' network operation. */
typedef struct tx_vm_t
{

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
#include "asset.h"


/**
 * Obtain an identifying key referencing for a licence transaction.
 */
#define get_license_key(_lic) \
  (&(_lic)->lic.lic_sig)

/**
 * Obtain an identifying key refrence for a file transaction.
 */ 
#define get_file_key(_ino) \
  (&(_ino)->ino.name)

/**
 * Obtain an identifying key reference to a app transaction.
 */
#define get_app_key(_app) \
  (&(_app)->app_sig)

#define get_bond_key(_bond) \
  (&(_bond)->bond_sig);

#define get_ward_key(_ward) \
  (&(_ward)->ward_sig.sig_key)

#define get_ident_key(_ident) \
  (&(_ident)->id_key)

/**
 * Obtain an identifying key reference to a user account.
 */
#define get_account_key(_acc) \
  (&(_acc)->pam_seed.seed_sig)

/**
 * Obtain an identifying key reference to a SEXE runtime task.
 */
#define get_task_key(_task) \
  (&(_task)->task.task_id)

#define get_thread_key(_th) \
  (&(_th)->th.th_job)

#define get_trust_key(_trust) \
  (&(_trust)->trust_sig)

#define get_event_key(_event) \
  (&(_event)->event_sig.sig_key)

#define get_session_key(_sess) \
  (&(_sess)->sess_key)

/**
 * Obtain an identifying key reference to a metric definition instance.
 */
#define get_metric_key(_metric) \
  (&(_metric)->met_id)

#define get_asset_key(_asset) \
  (&(_asset)->ass_sig)

#define get_wallet_key(_sig) \
  (&(_sig)->wal_sig)





/**
 * @}
 */

#endif /* ndef __BITS__BITS_H__ */

