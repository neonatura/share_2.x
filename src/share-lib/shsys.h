
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/neonatura/share)
 *        
 *  The Share Library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  The Share Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  @endcopyright
*/  

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif


#ifndef __MEM__SHSYS_H__
#define __MEM__SHSYS_H__



/**
 * System-level routines.
 * @ingroup libshare
 * @defgroup libshare_sys
 * @{
 */


/**
 * Perform geodetic calculations involving location metrics.
 *
 * @ingroup libshare_sys
 * @defgroup libshare_sysgeo Geodetic Calculations
 * @{
 */

/** Location precision of a 'regional area'. Roughly 3000 square miles. */
#define SHGEO_PREC_REGION 0 /* 69 LAT * 44.35 LON = 3k sq-miles */
/** Location precision of a 'zone'. Roughly 30 square miles. */
#define SHGEO_PREC_ZONE 1 /* 6.9 LAT * 4.43 LON = 30.567 sq-miles  */
/** Location precision of a 'district of land'. Roughly 0.3 square miles. */
#define SHGEO_PREC_DISTRICT 2 /* 3643.2 LAT * 2339 LON = 8.5mil sq-feet */
/** Location precision of a 'land site'. Roughly 85,000 square feet. */
#define SHGEO_PREC_SITE 3 /* 364.32 LAT * 233.9 LON = 85k sq-feet */
/** Location precision of a 'section of area'. Roughly 800 square feet. */
#define SHGEO_PREC_SECTION 4 /* 36.43 LAT * 22.7 LON = 827 sq-feet */
/** Location precision of a 'spot of land'. Roughly 8 square feet. */
#define SHGEO_PREC_SPOT 5 /* 3.64 LAT * 2.27 LON = 8.2628 sq-feet */
/** Location precision of a single point. Roughly 10 square inches. */
#define SHGEO_PREC_POINT 6 /* 4 LAT * 2.72448 LON = 10.897 sq-inches */
/** The number of precision specifications available. */
#define SHGEO_MAX_PRECISION 6


/** The system-level geodetic database. */
#define SHGEO_SYSTEM_DATABASE_NAME "geo"
/** The user-level geodetic database. */
#define SHGEO_USER_DATABASE_NAME "geo.usr"

/** A database table containing common north-america zipcodes. */
#define SHGEO_ZIPCODE "sys_zipcode_NA"
/** A database table containing common north-america places. */
#define SHGEO_COMMON "sys_common_NA"
/** A database table containing common north-america IP address locations. */
#define SHGEO_NETWORK "sys_network_NA"
/** A database table containing common north-america city names. */
#define SHGEO_CITY "sys_city_NA"
#if 0
/** A database table containing user-supplied locations. */
#define SHGEO_USER "user"
#endif


/** A specific location with optional altitude and time-stamp. */
struct shgeo_t
{
  /** The time-stamp of when geodetic location was established. */
  shtime_t geo_stamp;
  /** A latitude position. */
  uint64_t geo_lat;
  /** A longitude position. */
  uint64_t geo_lon;
  /** An altitude (in feet) */
  uint32_t geo_alt;
  /** The timezone associated with the geodetic location. */
  uint32_t __reserved__;
};

typedef struct shgeo_t shgeo_t;


/** A contextual description of a specific location. */
struct shloc_t
{
  char loc_name[MAX_SHARE_NAME_LENGTH];
  char loc_summary[MAX_SHARE_NAME_LENGTH];
  char loc_locale[MAX_SHARE_NAME_LENGTH];
  char loc_zone[MAX_SHARE_NAME_LENGTH];
  char loc_type[MAX_SHARE_NAME_LENGTH];
  uint32_t loc_prec;
  uint32_t __reserved_0__;

  struct shgeo_t loc_geo;
};

typedef struct shloc_t shloc_t;


/**
 * Establish a geodetic location based off a latitude, longitude, and optional altitude.
 */
void shgeo_set(shgeo_t *geo, shnum_t lat, shnum_t lon, int alt);

/**
 * Obtain the latitude, longitude, and altitude for a geodetic location.
 */
void shgeo_loc(shgeo_t *geo, shnum_t *lat_p, shnum_t *lon_p, int *alt_p);

/**
 * The duration since the geodetic location was established in seconds.
 */
time_t shgeo_lifespan(shgeo_t *geo);

/**
 * A 'key tag' representing the geodetic location in reference to a particular precision.
 */
shkey_t *shgeo_tag(shgeo_t *geo, int prec);


/**
 * Compare two geodetic locations for overlap based on precision specified.
 */
int shgeo_cmp(shgeo_t *geo, shgeo_t *cmp_geo, int prec);

int shgeo_cmpf(shgeo_t *geo, double lat, double lon);

/** The combined latitude and longitude distances between two geodetic locations. */
double shgeo_radius(shgeo_t *f_geo, shgeo_t *t_geo);

/** Reduce the precision of a geodetic location. */
void shgeo_dim(shgeo_t *geo, int prec);

/**
 * Obtain the device's current location.
 */
void shgeo_local(shgeo_t *geo, int prec);


/**
 * Manually set the device's current location.
 */
void shgeo_local_set(shgeo_t *geo);


/** Search an area for a known geodetic location. */
int shgeodb_scan(shnum_t lat, shnum_t lon, shnum_t radius, shgeo_t *geo);

/** Search for a known geoetic location based on a location name. */
int shgeodb_place(const char *name, shgeo_t *geo);

/** Search for a known geodetic location given an IP or Host network address. */
int shgeodb_host(const char *name, shgeo_t *geo);

/** Search for a known geodetic location given an IP or Host network address. */
int shgeodb_loc(shgeo_t *geo, shloc_t *loc);

/** Set custom location information for a particular geodetic location. */
int shgeodb_loc_set(shgeo_t *geo, shloc_t *loc);

int shgeodb_loc_unset(shgeo_t *geo);


/** A formal description of a particular place code. */
const char *shgeo_place_desc(char *code);

/** The geometric precision for a particular place type. */
int shgeo_place_prec(char *code);

/** An array of codes signifying difference types of places. */
const char **shgeo_place_codes(void);


/** Obtain a rowid for a particular geodetic location in a given database. */
int shgeodb_rowid(shdb_t *db, const char *table, shgeo_t *geo, shdb_idx_t *rowid_p);

/** Obtain a geodetic location from a location name in a given database. */
int shgeodb_name(shdb_t *db, char *table, const char *name, shgeo_t *geo);


/**
 * @}
 */






/**
 * Permission access management.
 * @ingroup libshare_sys
 * @defgroup libshare_syspam
 * @{
 */

#define SHPAM_DELETE (1 << 0)
#define SHPAM_EXPIRE (1 << 1)
#define SHPAM_LOCK (1 << 2)
#define SHPAM_STATUS (1 << 3)
#define SHPAM_SESSION (1 << 4)
#define SHPAM_UNLOCK (1 << 5)
#define SHPAM_UPDATE (1 << 6)
#define SHPAM_CREATE (1 << 7)

struct shadow_t 
{
  shkey_t sh_sess; 
  shkey_t sh_id;
  shtime_t sh_expire;
  uint64_t sh_uid;

  /** Geodetic cordinates of the primary location. */
  shgeo_t sh_geo;
  /* An account name alias. */
  char sh_name[MAX_SHARE_NAME_LENGTH];
  /* A person name or organization. */
  char sh_realname[MAX_SHARE_NAME_LENGTH];
  /** A email account. */
  char sh_email[MAX_SHARE_NAME_LENGTH];
  /** A share-coin coin address. */
  char sh_sharecoin[MAX_SHARE_HASH_LENGTH];
};
typedef struct shadow_t shadow_t;



/** A unique reference to a share account. */
uint64_t shpam_uid(char *username);

/** An identity key referencing an account for an application. */
shkey_t *shpam_ident_gen(uint64_t uid, shpeer_t *peer);

/** The 'root' identity for an application. */
shkey_t *shpam_ident_root(shpeer_t *peer);

/** Verify that an identity key references an application account. */
int shpam_ident_verify(shkey_t *id_key, uint64_t uid, shpeer_t *peer);

shkey_t *shpam_sess_gen(shkey_t *pass_key, shtime_t stamp, shkey_t *id_key);

int shpam_sess_verify(shkey_t *sess_key, shkey_t *pass_key, shtime_t stamp, shkey_t *id_key);


/** Generate a random salt to be used to perterb a password key. */
uint64_t shpam_salt(void);

/** Generate a key salt from the data content provided. */
uint64_t shpam_salt_gen(unsigned char *data, size_t data_len);

/** The current user's system account name. */
const char *shpam_username_sys(void);

/** Generate a password seed from the username, passphrase, and salt provided. */
shseed_t *shpam_pass_gen(char *username, char *passphrase, uint64_t salt);

/** Generate a password seed from the system login username specified. */
shseed_t *shpam_pass_sys(char *username);

/** Verify a password seed references a username and password. */
int shpam_pass_verify(shseed_t *seed, char *username, char *passphrase);


/**
 * @}
 */








/**
 * Shadow password file management.
 * @ingroup libshare_sys
 * @defgroup libshare_sysshadow
 * @{
 */

/** Obtain the default file on a sharefs partition for storing credentials. */
shfs_ino_t *shpam_shadow_file(shfs_t **fs_p);

/** Create a new shadow file credential. */
int shpam_shadow_create(shfs_ino_t *file, uint64_t uid, shadow_t *ret_shadow);

/** Load an existing shadow file credential. */
int shpam_shadow_load(shfs_ino_t *file, uint64_t uid, shadow_t *shadow);

/** Update an existing shadow file credential. */
int shpam_shadow_store(shfs_ino_t *file, shadow_t *shadow);

/** Remove a credential from a shadow file. */
int shpam_shadow_remove(shfs_ino_t *file, uint64_t uid, shkey_t *sess_key);

/** Create a new shadow password entry. */
int shpam_pshadow_create(shfs_ino_t *file, shseed_t *seed);

/** Generate a new shadow password entry from a username and passphrase. */
int shpam_pshadow_new(shfs_ino_t *file, char *username, char *passphrase);

/** Load a pre-existing shadow password entry into memory. */
int shpam_pshadow_load(shfs_ino_t *file, uint64_t uid, shseed_t *ret_seed);

/** Update a shadow password entry. */
int shpam_pshadow_store(shfs_ino_t *file, shseed_t *seed);

/** Set a new password key for an existing shadow password entry. */
int shpam_pshadow_set(shfs_ino_t *file, shseed_t *seed, shkey_t *sess_key);

/** Remove a shadow password entry. */
int shpam_pshadow_remove(shfs_ino_t *file, uint64_t rem_uid);

/* Generate a new identity. */ 
int shpam_shadow_session_new(shfs_ino_t *file, char *acc_name, char *passphrase);

/** Generate a new identity session. */
int shpam_shadow_session(shfs_ino_t *file, shseed_t *seed, shkey_t **sess_p, shtime_t *expire_p);

int shpam_shadow_session_verify(shfs_ino_t *file, uint64_t uid, shkey_t *sess_key);

/** Set a new session key to an account identity. */
int shpam_shadow_session_set(shfs_ino_t *file, uint64_t uid, shkey_t *id_key, uint64_t sess_stamp, shkey_t *sess_key);

/** Expire the current identity session. */
int shpam_shadow_session_expire(shfs_ino_t *file, uint64_t uid, shkey_t *sess_key);

/** Validate a username and passphrase and retrieve a session key. */
int shpam_shadow_login(shfs_ino_t *file, char *acc_name, char *acc_pass, shkey_t **sess_key_p);



/**
 * @}
 */




/**
 * Application state management.
 * @ingroup libshare_sys
 * @defgroup libshare_sysapp
 * @{
 */

/** An application that is not intended to be publically accessible. */
#define SHAPP_LOCAL (1 << 0)

/** Indicates that the "soft" resource limitations set by OS should be utilized. */
#define SHAPP_RLIMIT (1 << 1) 


/**
 * Strips the absolute parent from @a app_name
 * @note "/test/one/two" becomes "two"
 * @param app_name The running application's executable path
 * @returns Relative filename of executable.
 */
char *shapp_name(char *app_name);

/**
 * Initialize the share library runtime for an application.
 * @param exec_path The process's executable path.
 * @param host The host that the app runs on or NULL for localhost.
 * @param flags application flags
 */
shpeer_t *shapp_init(char *exec_path, char *host, int flags);

int shapp_register(shpeer_t *peer);

int shapp_listen(int tx, shpeer_t *peer);

int shapp_account(const char *username, char *passphrase, shseed_t **seed_p);

int shapp_ident(uint64_t uid, shkey_t **id_key_p);

int shapp_session(shseed_t *seed, shkey_t **sess_key_p);

int shapp_account_create(char *acc_name, char *acc_pass, shkey_t **id_key_p);

int shapp_account_login(char *acc_name, char *acc_pass, shkey_t **sess_key_p);

int shapp_account_setpass(char *acc_name, char *opass, char *pass);

int shapp_account_remove(char *acc_name, char *acc_pass);

int shapp_account_info(uint64_t uid, shadow_t *shadow, shseed_t *seed);

int shapp_account_set(char *acc_name, shkey_t *sess_key, shgeo_t *geo, char *rname, char *email, char *shc_addr);

shjson_t *shapp_account_json(shadow_t *shadow);

/**
 * @}
 */




/**
 * Perform posix crypt algorythm for sha256, and sha512.
 * @ingroup libshare_sys
 * @defgroup libshare_syscrypt
 * @{
 */

char *shcrypt(const char *passwd, const char *salt);

char *shcrypt_sha256_r(const char *key, const char *salt, char *buffer, int buflen);

char *shcrypt_sha256(const char *key, const char *salt);

char *shcrypt_sha512_r(const char *key, const char *salt, char *buffer, int buflen);

char *shcrypt_sha512(const char *key, const char *salt);

/**
 * @}
 */



/**
 * Application logging calls.
 * @ingroup libshare_fs
 * @defgroup libshare_fslog
 * @{
 */
#define SHLOG_INFO 1
#define SHLOG_WARNING 2
#define SHLOG_ERROR 3
#define SHLOG_RUSAGE 4

/** Perform a generic logging operation. */
int shlog(int level, int err_code, char *log_str);

/** Log a libshare error code (SHERR_XXX) and an error message. */
void sherr(int err_code, char *log_str);

/** Log a warning message. */
void shwarn(char *log_str);

/** Log a informational message.  */
void shinfo(char *log_str);
/**
 * @}
 */




/**
 * libshare IPC message-queue IO calls.
 * @ingroup libshare_sys
 * @defgroup libshare_sysmsg
 * @{
 */


#define MAX_MESSAGE_QUEUES 512
/** The maximum size of an individual libshare message queue. */
#define MESSAGE_QUEUE_SIZE 4096000
/** The maximum number of messages a message queue can contain. */
#define MAX_MESSAGES_PER_QUEUE 2048


/** remove a message queue's resources. */
#define SHMSGF_RMID (1 << 0)

/** discard stale messages when queue is full. */
#define SHMSGF_OVERFLOW (1 << 1)

/** allow for receiving messages sent by one self. */
#define SHMSGF_ANONYMOUS (1 << 2)

/** unused */
#define SHMSGF_AUTH (1 << 4)





struct shmsg_t 
{

  /** source peer of message. */
  shkey_t src_key;

  /** destination peer of message. */
  shkey_t dest_key;

  /** message queue id */
  uint32_t msg_qid;

  /** total size of message content */
  uint32_t msg_size;

  /** offset of message data */
  uint32_t msg_of;

  /** type of message */
  uint32_t __reserved_1__;

};

typedef struct shmsg_t shmsg_t;

typedef struct shmsgq_t {
  /** expiration of lock or 0 if unlocked. */
  shtime_t lock_t;

  /** message queue flags SHMSGF_XX */
  uint32_t flags;

  /* reserved for future use */
  uint32_t __reserved_1__;

  /** read msg seek offset */
  uint32_t read_idx;

  /** write msg seek offset */
  uint32_t write_idx;

  /** read data seek offset */
  uint32_t read_of;

  /** write data seek offset */
  uint32_t write_of;

  /** table of message definitions */
  shmsg_t msg[MAX_MESSAGES_PER_QUEUE];

  /** raw message content data */
  unsigned char data[0];
} shmsgq_t;

/**
 * Obtain the message queue id from a share library peer.
 * @param peer The destination peer message queue.
 */
int shmsgget(shpeer_t *peer);

/**
 * Send a message to a share library peer.
 * @param msg_qid The share library message queue id.
 * @param msg_type A non-zero user-defined categorical number.
 * @see shmsgget()
 */
int shmsgsnd(int msqid, const void *msgp, size_t msgsz);

/**
 * Send a message to a share library peer.
 * @param dest_key Peer key of message destination. Specifying NULL indicates to use the peer used to open the message queue.
 * @see shmsgget()
 */
int shmsg_write(int msg_qid, shbuf_t *msg_buff, shkey_t *dest_key);

/**
 * Receive a message from a share library peer.
 */
int shmsgrcv(int msqid, void *msgp, size_t msgsz);

/**
 * Receive a message from a share library peer.
 */
int shmsg_read(int msg_qid, shkey_t *src_key, shbuf_t *msg_buff);

/**
 * Set or retrieve message queue control attributes.
 */
int shmsgctl(int msg_qid, int cmd, int value);


/**
 * @}
 */



/* __SYS__SHSYS_PROC_H__ */

/**
 * libshare spawned process management
 * @ingroup libshare_sys
 * @defgroup libshare_sysproc
 * @{
 */


#define SHPROC_NONE 0
#define SHPROC_IDLE 1
#define SHPROC_PEND 2
#define SHPROC_RUN 3
#define MAX_SHPROC_STATES 4

/** A control option which manages the maximum number of processes spawned. */
#define SHPROC_MAX 100
#define SHPROC_PRIO 101

/** The default maximum number of processes spawned. */
#define SHPROC_POOL_DEFAULT_SIZE 16

typedef int (*shproc_op_t)(int, shbuf_t *buff);

struct shproc_req_t 
{
  uint32_t state;
  uint32_t error;
  uint32_t crc;
  uint32_t data_len;
  uint32_t user_fd;
};
typedef struct shproc_req_t shproc_req_t;

struct shproc_t
{
  int proc_pid;
  int proc_state;
  int proc_fd;
  int proc_idx;
  int proc_error;
  int proc_prio;

  int user_fd;
  int dgram_fd;

  shproc_op_t proc_req;
  shproc_op_t proc_resp;
  shtime_t proc_stamp;
  shbuf_t *proc_buff;

  struct shproc_stat_t {
    int in_tot;
    int out_tot;
    double span_tot[MAX_SHPROC_STATES];
    int span_cnt[MAX_SHPROC_STATES];
  } stat;
};
typedef struct shproc_t shproc_t;


struct shproc_pool_t
{
  int pool_max;
  int pool_lim;
  int pool_prio;
  /* callbacks */
  shproc_op_t pool_req;
  shproc_op_t pool_resp;
  /* process list */
  shproc_t *proc;
};
typedef struct shproc_pool_t shproc_pool_t;

/**
 * Create a new pool to manage process workers.
 */
shproc_pool_t *shproc_init(shproc_op_t req_f, shproc_op_t resp_f);

/**
 * Configure a process pool's attributes.
 *  - SHPROC_MAX The maximum number of processes that can be spawned in the pool.
 *  - SHPROC_PRIO A value in the range -20  to  19. A lower priority indicates a more favorable scheduling. 
 *  @param type The configuration option value to set or get.
 *  @param val Zero to indicate a 'Get Value' request; otherwise the parameter specifies the value to the option to.
 */
int shproc_conf(shproc_pool_t *pool, int type, int val);

/**
 * Obtain currrent pool, if any, that has been initialized.
 */
shproc_pool_t *shproc_pool(void);

/**
 * Start a new process to handle worker requests.
 */
shproc_t *shproc_start(shproc_pool_t *pool);

/**
 * Terminate a running worker process.
 */
int shproc_stop(shproc_t *proc);

/**
 * Obtain a process slot from the pool based on process state.
 */
shproc_t *shproc_get(shproc_pool_t *pool, int state);

int shproc_schedule(shproc_t *proc, unsigned char *data, size_t data_len);

/**
 * Obtain a process from the pool that is ready for work.
 */
shproc_t *shproc_pull(shproc_pool_t *pool);

/**
 * Perform a request against a process ready for work.
 */
int shproc_push(shproc_pool_t *pool, int fd, unsigned char *data, size_t data_len);

/**
 * deallocate resources for a process pool
 */
void shproc_free(shproc_pool_t **pool_p);

/**
 * Set a custom signal handler for worker process.
 */
void shproc_signal(void *sig_f);

/**
 * Process pending communications with worker process(es).
 */
void shproc_poll(shproc_pool_t *pool);

void shproc_rlim_set(void);

uint64_t shproc_rlim(int mode);

/**
 * @}
 */









/* __SYS__SHSYS_PROC_H__ */


/**
 * Encode an authoritative certificate.
 * @ingroup libshare_sys
 * @defgroup libshare_syscert
 * @{
 */

typedef struct shasset_t
{
  char host_url[MAX_SHARE_HASH_LENGTH]; /* [a-zA-Z]{2,3}(-([a-zA-Z]{2}|[0-9]{3}))? */

  /* external asset barcode reference */
  char ass_code[16];

  char ass_locale[16]; /* [a-zA-Z]{2,3}(-([a-zA-Z]{2}|[0-9]{3}))? */

  /** The certificate pertaining to this asset. */
  shkey_t ass_cert;

  /** The location where the asset resides. */
  shgeo_t ass_loc;

  /* identity key of originating creator */
  shkey_t ass_id;

  /* originating peer (priveleged key) */
  shkey_t ass_peer;

  /** A signature key verifying the underlying contents. */
  shkey_t ass_sig;

  /** Time-stamp of when asset was created. */
  shtime_t ass_birth;

  /** When the asset information invalidates. */
  shtime_t ass_expire;
} shasset_t;

/** A symbolic reference of with no systematic use. */
#define TXREF_SYMBOL 0 
/** A symbolic reference for testing-use only. */
#define TXREF_TEST 1
/* a sharenet transaction hash (u224) */
#define TXREF_TX 10
/* a SHC block-chain transaction hash (u256) */
#define TXREF_SHCTX 20
/* public key coin address (uint160) */
#define TXREF_PUBADDR 30
/* a certificate hash reference (uint160) */
#define TXREF_CERT 31

/**
 * A label tag for a particular key.
 */
typedef struct shref_t
{
  /** A plain-text name in reference to a key. */
  char ref_name[MAX_SHARE_NAME_LENGTH];

  /** The key/hash being referenced. */
  char ref_hash[MAX_SHARE_HASH_LENGTH];

  /** A key referencing the server which issued the reference. */
  shkey_t ref_peer;

  /** Auxillary context associated with the reference. */
  shkey_t ref_ctx;

  /** The time-stamp of when the reference is no longer valid. */
  shtime_t ref_expire;

  /* a particular type of reference (TXREF_XXX) */
  uint32_t ref_type;

  /* a sub-type specific to the reference type */
  uint32_t ref_level;
} shref_t;

typedef struct shent_t
{

  /** serial number (128-bit number) */
  uint8_t ent_ser[16];

  uint8_t __reserved_0__[496];

  /** The name of the entity or a pseudonym. */
  char ent_name[MAX_SHARE_NAME_LENGTH];

  /** A network peer reference of the entity. */
  struct shpeer_t ent_peer;

  /** A reference to a signature or key. */
  struct shsig_t ent_sig;

  /** The byte-size of the context that was used to generate the signature. */
  uint32_t ent_len;

} shent_t;

typedef struct shcert_t
{

  /* The signatory of the certificate. */
  shent_t cert_sub;

  /* The CA entity which authorizes the certificate. */
  shent_t cert_iss;

  uint64_t __reserved_0__;
  uint64_t __reserved_1__;

  /** total coins to liense this certificate. */
  uint64_t cert_fee;

  /** certificate attributes - SHCERT_XXX */
  uint32_t cert_flag;

  /** certificate version */
  uint32_t cert_ver;

} shcert_t;


/** Obtain the subject's signature key from a share certificate. */
#define shcert_sub_sig(_cert) \
  (&(_cert)->cert_sub.ent_sig.sig_key)

/** The share time-stamp of when the certificate subject's signature becomes valid. */
#define shcert_sub_stamp(_cert) \
  ((_cert)->cert_sub.ent_sig.sig_stamp)

/** The share time-stamp of when the certificate subject's signature validicity expires. */
#define shcert_sub_expire(_cert) \
  ((_cert)->cert_sub.ent_sig.sig_expire)

/** Obtain the subject's signature algorithm from a share certificate. */
#define shcert_sub_alg(_cert) \
  ((_cert)->cert_sub.ent_sig.sig_key.alg)

/** Obtain the serial number of the certificate. */
#define shcert_sub_ser(_cert) \
  ((_cert)->cert_sub.ent_ser)

/** Obtain the length of the context used to create the signature. */
#define shcert_sub_len(_cert) \
  ((_cert)->cert_sub.ent_len)

/** Obtain the issuer's signature key from a share certificate. */
#define shcert_iss_sig(_cert) \
  (&(_cert)->cert_iss.ent_sig.sig_key)

/** Obtain the issuer's signature algorithm from a share certificate. */
#define shcert_iss_alg(_cert) \
  ((_cert)->cert_iss.ent_sig.sig_key.alg)

/** Obtain the length of the context used to create the private signature. */
#define shcert_iss_len(_cert) \
  ((_cert)->cert_iss.ent_len)

/** The share time-stamp of when the certificate issuer's signature becomes valid. */
#define shcert_iss_stamp(_cert) \
  ((_cert)->cert_iss.ent_sig.sig_stamp)

/** The share time-stamp of when the certificate issuer's signature validicity expires. */
#define shcert_iss_expire(_cert) \
  ((_cert)->cert_iss.ent_sig.sig_expire)

/** Obtain the serial number of the issuer's certificate. */
#define shcert_iss_ser(_cert) \
  ((_cert)->cert_iss.ent_ser)




int shcert_sign_verify(shcert_t *cert, shcert_t *parent);

int shcert_sign(shcert_t *cert, shcert_t *parent);

int shcert_verify(shcert_t *cert, shcert_t *parent);

int shfs_cert_apply(SHFL *file, shcert_t *cert);

void shcert_free(shcert_t **cert_p);

int shcert_ca_init(shcert_t *cert, char *entity, uint64_t fee, int alg, int flags);

int shcert_init(shcert_t *cert, char *entity, uint64_t fee, int alg, int flags);

const char *shcert_serialno(shcert_t *cert);

void shcert_print(shcert_t *cert, shbuf_t *pr_buff);

int shcert_init_default(shcert_t *cert);

/* shfs_cert.c */

int shfs_cert_save(shcert_t *cert, char *ref_path);

shcert_t *shfs_cert_load(char *serial_no);

shcert_t *shfs_cert_load_ref(char *ref_path);

int shfs_cert_get(SHFL *fl, shcert_t **cert_p, shlic_t **lic_p);

shkey_t *shfs_cert_sig(shcert_t *cert);


/* shsys_lic.c */

int shlic_get(SHFL *file, shcert_t *lic_cert_p, shcert_t *cert_p, shlic_t *lic_p);

/**
 * Validates authorized licensing of a file.
 */
int shlic_validate(SHFL *file);

/**
 * Apply a licensing certificate to a shfs file.
 */
int shlic_set(SHFL *file, shcert_t *cert);

/**
 * Save a license certificate to the system directory.
 * @param cert The licensing (parent) certificate.
 * @param lic The licensee certificate.
 */
int shlic_save(shcert_t *cert, shcert_t *lic);

int shlic_save_sig(shkey_t *sig_key, shcert_t *lic);





/**
 * @}
 */



/**
 * Tempoarily store binary data segments.
 * @ingroup libshare_sys
 * @defgroup libshare_syscache
 * @{
 */

char *shcache_path(const char *tag);

int shcache_write(const char *tag, shbuf_t *buff);

int shcache_read(const char *tag, shbuf_t *buff);

int shcache_fresh(const char *tag);

void shcache_purge(char *path);

time_t shcache_ttl(void);

/**
 * @}
 */



/**
 * Manage a libshare file package.
 * @ingroup libshare_sys
 * @defgroup libshare_syspkg
 * @{
 */

#define SHPKGOP_EXTRACT &shpkg_extract_op
#define SHPKGOP_SIGN &shpkg_sign_op
#define SHPKGOP_UNSIGN &shpkg_unsign_op
#define SHPKGOP_REMOVE &shpkg_remove_op
#define SHPKGOP_LIST &shpkg_list_op

/**
 * A libshare package definition pertaining to a set of files and instructions on how to install those files.
 * @note Structure is the header for a '.spk' libshare package file.
 */
typedef struct shpkg_info_t
{
  /** The unique name of the package */
  char pkg_name[MAX_SHARE_NAME_LENGTH];
  /** The version number. */
  char pkg_ver[MAX_SHARE_NAME_LENGTH];
  /** The certificate used to license extracted files. */
  shcert_t pkg_cert;
  /** The originating peer which generated the package. */
  shpeer_t pkg_peer;
  /** The time-stamp of when the package was updated. */
  shtime_t pkg_stamp;
  /** The architecture the package is intended for. */
  uint32_t pkg_arch;
} shpkg_info_t;

typedef struct shpkg_t
{
  shfs_t *pkg_fs;
  shfs_ino_t *pkg_file;
  shpkg_info_t pkg;
  shbuf_t *pkg_buff;
} shpkg_t;

typedef int (*shpkg_op_t)(shpkg_t *, char *, shfs_ino_t *);


int shpkg_extract_op(shpkg_t *pkg, char *path, SHFL *file);
int shpkg_sign_op(shpkg_t *pkg, char *path, SHFL *file);
int shpkg_unsign_op(shpkg_t *pkg, char *path, SHFL *file);
int shpkg_remove_op(shpkg_t *pkg, char *path, SHFL *file);
int shpkg_list_op(shpkg_t *pkg, char *path, SHFL *file);
int shpkg_op_dir(shpkg_t *pkg, char *dir_name, char *fspec, shpkg_op_t op);


int shpkg_init(char *pkg_name, shpkg_t **pkg_p);

shkey_t *shpkg_sig(shpkg_t *pkg);

void shpkg_free(shpkg_t **pkg_p);

char *shpkg_version(shpkg_t *pkg);

void shpkg_version_set(shpkg_t *pkg, char *ver_text);

char *shpkg_name_filter(char *in_name);

char *shpkg_name(shpkg_t *pkg);

shpkg_t *shpkg_load(char *pkg_name, shkey_t *cert_sig);

int shpkg_sign_remove(shpkg_t *pkg);

int shpkg_extract(shpkg_t *pkg);

int shpkg_owner(shpkg_t *pkg);

int shpkg_cert_clear(shpkg_t *pkg);

int shpkg_remove(shpkg_t *pkg);

SHFL *shpkg_spec_file(shpkg_t *pkg);

int shpkg_sign(shpkg_t *pkg, shcert_t *cert);

int shpkg_sign_name(shpkg_t *pkg, char *cert_alias);

int shpkg_extract_files(shpkg_t *pkg, char *fspec);

int shpkg_file_license(shpkg_t *pkg, SHFL *file);


/**
 * @}
 */












/**
 * Manage auxillary context information.
 *
 * @ingroup libshare_sys
 * @defgroup libshare_sysctx Context Database
 * @{
 */

#define SHCTX_DEFAULT_EXPIRE_TIME 63072000 /* two years */ 

#define SHCTX_MAX_VALUE_SIZE 4096

#define SHCTX_TABLE_COMMON "common"

#define SHCTX_DATABASE_NAME "ctx"


typedef struct shctx_t
{

  /** An ident or coin-addr hash referencing the creator. */
  char ctx_iss[MAX_SHARE_HASH_LENGTH];

  /** A key referencing the context name. */
  shkey_t ctx_key;

  /** The creation date. */
  time_t ctx_stamp;

  /** The expiration date (2 years from creation). */
  time_t ctx_expire;

  uint64_t ctx_data_len;

  /** An allocated string of up to 4095 characters. */
  uint8_t *ctx_data;

} shctx_t;



/** Set an auxillary context. */
int shctx_set(char *name, unsigned char *data, size_t data_len);

/** Set an auxillary string context. */
int shctx_setstr(char *name, char *data);

/** Get an auxillary context. */
int shctx_get(char *name, shctx_t *ctx);

/** Store auxillary context via a share-key. */
int shctx_set_key(shkey_t *name_key, unsigned char *data, size_t data_len);

/** Retrieve auxillary context via a share-key. */
int shctx_get_key(shkey_t *name_key, shctx_t *ctx);

/** inform the shared daemon to relay a local context. */
int shctx_notify(shkey_t *name_key);

/**
 * A (shr160 / ripemd32) share key referencing the textual name.
 * @param name An unlimited length literal string.
 * @returns An un-allocated share-key.
 * @note The returned share-key does not need to be freed.
 */
shkey_t *shctx_key(char *name);





/**
 * @}
 */




/**
 * Utility functions to generate cryptographic signature with public/private key pairs.
 *
 * Cryptographic algorythms supported include a 128-bit, 224-bit, 256-bit, 384-bit, or 512-bit hash digest.
 *
 * The SHALG_SHA1, SHALG_SHA224, SHALG_SHA256, SHALG_SHA384, and SHALG_SHA512 algorythms provide the various implementations of the Secure Hash Algorythm. Supplemental HMAC and HKDF encoding is provided. 
 *
 * The SHALG_ECDSA128R, SHALG_ECDSA160R, SHALG_ECDSA160K, SHALG_ECDSA224R, SHALG_ECDSA224K, SHALG_ECDSA256R, SHALG_ECDSA256K, SHALG_ECDSA384R, and SHALG_ECDSA512R algorythms provide the various implementions of the Elliptic Curve DSA encryption method. 
 *
 * The SHALG_RIPEMD160 algorythm is synonymous with the SHALG_SHR160 algorythm.
 *
 * The SHALG_SHR160 algorythm provides a private key generation via the RIPEMD160 implementation and a ECDSA256K public-private key validation method. 
 *
 * The SHALG_SHR224 algorythm is a proprietary libshare runtime library. The computation speed to calculate regular or HMAC SHR224 digests is much faster than the SHA or ECDSA algorythms. The method used combines aspects of both checksum computation and bit operations. 
 *
 * @ingroup libshare_sys
 * @defgroup libshare_sysalg Cryptographic messages.
 * @{
 */

#define SHFMT_HEX 0
#define SHFMT_BASE32 1
#define SHFMT_BASE58 2
#define SHFMT_BASE64 3
#define SHFMT_SHR56 4
#define MAX_SHFMT 5


#define MAX_ALG_WORD_SIZE 36

#define MAX_ALG_SIZE ((MAX_ALG_WORD_SIZE - 1) * sizeof(uint32_t))

/** A key or signature. */
typedef uint32_t shalg_t[MAX_ALG_WORD_SIZE];

#define shalg_size(_a) \
  ((_a)[MAX_ALG_WORD_SIZE - 1])


char *shhex_str(unsigned char *data, size_t data_len);

void shhex_bin(char *hex_str, unsigned char *data, size_t data_max);


/** Print the algorythm parameters. */
const char *shalg_str(int alg);

int shalg_fmt(char *label);

char *shalg_fmt_str(int fmt);

/** Print a binary segment in the format specified. */
char *shalg_encode(int fmt, unsigned char *data, size_t data_len);

int shalg_decode(int fmt, char *in_data, unsigned char *data, size_t *data_len_p);

/** Generate a private signature from a 'secret' binary segment. */
int shalg_priv(int alg, shalg_t ret_key, unsigned char *data, size_t data_len);

/** Print a key or signature in the format specified. */
char *shalg_print(int fmt, shalg_t key);

int shalg_gen(int fmt, char *in_data, shalg_t ret_key);

/** Generate a public key from a private key. */
int shalg_pub(int alg, shalg_t priv_key, shalg_t ret_key);

int shalg_sign(int alg, shalg_t priv_key, shalg_t ret_sig, unsigned char *data, size_t data_len);

int shalg_ver(int alg, shalg_t pub_key, shalg_t sig_key, unsigned char *data, size_t data_len);

int shalg_mode_str(char *mode_str);


/**
 * @}
 */












/**
 * @}
 */

#endif /* ndef __MEM__SHSYS_H__ */


#ifdef __cplusplus
}
#endif

