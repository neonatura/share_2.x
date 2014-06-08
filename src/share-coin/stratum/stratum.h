
typedef struct user_t
{
  scrypt_peer peer;

  char worker[128];
  char cli_ver[128];

  int fd;
  int flags;

  /* request id for 'client.get_version'. */
  int cli_id;

  /* last height notified to user */
  int height;

  /* last submitted block timestamp. */
  shtime_t block_tm;

  /* total shares from blocks */ 
  double block_tot;

  /* cntal accepted blocks submitted. */ 
  size_t block_cnt;

  struct user_t *next;
} user_t;

typedef struct task_t
{

  /* unique reference number for task */
  unsigned int task_id;

  /* block attributes */
  int version;
  char cb1[256];
  char cb2[256];
  char prev_hash[256];
  char xnonce2[16];
  char nbits[32];
  time_t curtime;
  long height;

  /* transactions */
  char **merkle;
  size_t merkle_len;

  /* block template parameters */
  char tmpl_merkle[256];
  char tmpl_xnonce1[16];

  scrypt_work work;

  struct task_t *next;
} task_t;

#include "stratum_user.h"
#include "stratum_protocol.h"
#include "stratum_message.h"
#include "stratum_task.h"

