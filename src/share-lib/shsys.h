
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

#define SHAPP_LOCAL (1 << 0)


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

typedef struct shcert_ent_t
{

  /** encrypted data content being signed */
  char ent_data[512];

  /** The name of the entity or a pseudonym. */
  char ent_name[MAX_SHARE_NAME_LENGTH];

  /** A network peer reference of the entity. */
  struct shpeer_t ent_peer;

  /** A reference to a signature or key. */
  struct shsig_t ent_sig;

  /** The byte-size of the context that was used to generate the signature. */
  uint32_t ent_len;

} shcert_ent_t;

typedef struct shcert_t
{

  /* The signatory of the certificate. */
  shcert_ent_t cert_sub;

  /* The CA entity which authorizes the certificate. */
  shcert_ent_t cert_iss;

  /** certificate serial number (128-bit number) */
  uint8_t cert_ser[16];

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

/** Obtain the data content used to create the signature. */
#define shcert_sub_key(_cert) \
  ((_cert)->cert_sub.ent_data)

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




int shcert_sign_verify(shcert_t *cert, shcert_t *parent);

int shcert_sign(shcert_t *cert, shcert_t *parent);

int shcert_verify(shcert_t *cert, shcert_t *parent);

int shfs_cert_apply(SHFL *file, shcert_t *cert);

void shcert_free(shcert_t **cert_p);

shcert_t *shfs_cert_load_ref(char *ref_path);


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

shpkg_t *shpkg_load(char *pkg_name, shkey_t *cert_sig);

int shpkg_sign_remove(shpkg_t *pkg);

int shpkg_extract(shpkg_t *pkg);

int shpkg_owner(shpkg_t *pkg);

int shpkg_cert_clear(shpkg_t *pkg);

int shpkg_remove(shpkg_t *pkg);

SHFL *shpkg_spec_file(shpkg_t *pkg);

int shpkg_sign(shpkg_t *pkg, shcert_t *cert);

int shpkg_extract_files(shpkg_t *pkg, char *fspec);

int shpkg_file_license(shpkg_t *pkg, SHFL *file);


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

