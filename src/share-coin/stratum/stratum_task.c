
#define __STRATUM__TASK_C__
#include "shcoind.h"


#define BLOCK_VERSION 1
#define MAX_SERVER_NONCE 128
#define MAX_ROUND_TIME 600

static task_t *task_list;

char last_payout_hash[512];

user_t *sys_user;


void free_tasks(void)
{
  task_t *task;
  task_t *task_next;

  for (task = task_list; task; task = task_next) {
    task_next = task->next;
    task_free(&task);
  }
  task_list = NULL;

}

#if 0
void free_task(task_t **task_p)
{
  task_t *task;
  int i;

  if (!task_p)
    return;
  task = *task_p;
  *task_p = NULL;

  if (task->merkle) {
    for (i = 0; task->merkle[i]; i++) {
      free(task->merkle[i]);
    }
    free(task->merkle);
  }

  free(task);

}
#endif

int task_work_t = 3;

void reset_task_work_time(void)
{
  task_work_t = 3;
}

#define MEDIAN_TIME_PER_BLOCK 28

task_t *task_init(void)
{
  static int work_idx = 0;
  static uint64_t last_block_height;
  static int xn_len = 8;
  static time_t last_block_change;
  shjson_t *block;
  unsigned char hash_swap[32];
  shjson_t *tree;
  task_t *task;
  const char *templ_json;
  char coinbase[512];
  char sig[256];
  char *ptr;
  char target[32];
  char path[PATH_MAX+1];
  uint64_t block_height;
  unsigned long cb1;
  unsigned long cb2;
  int work_reset;
  int i;

  work_reset = FALSE;
  block_height = getblockheight();
  if (block_height != last_block_height ||
      (time(NULL) - MEDIAN_TIME_PER_BLOCK) > last_block_change) {
    check_payout();

    reset_task_work_time();
    work_idx = -1;
    work_reset = TRUE;

    free_tasks();
    last_block_change = time(NULL);
    last_block_height = block_height;
  }

  work_idx++;
  if (0 != (work_idx % task_work_t)) {
    return (NULL);
  }

  /* gradually decrease task generation rate per block. */
  work_idx = 0;
  task_work_t++;

  templ_json = getblocktemplate();
  if (!templ_json)
    return (NULL); /* template not currently available. */

  tree = shjson_init(templ_json);
  if (!tree) {
fprintf(stderr, "DEBUG: task_init: cannot parse json\n");
    return (NULL);
  }

  block = shjson_obj(tree, "result");
  if (!block) {
    shjson_free(&tree);
fprintf(stderr, "DEBUG: task_init: cannot parse json result\n");
    return (NULL);
  }



  task = (task_t *)calloc(1, sizeof(task_t));
  if (!task) { 
    shjson_free(&tree);
    return (NULL);
  }

  task->work_reset = work_reset;

  memset(target, 0, sizeof(target));
  strncpy(target, shjson_astr(block, "target", "0000ffff"), 8);
  task->target = (double)0xffff / (double)strtol(target, NULL, 16);

  memset(coinbase, 0, sizeof(coinbase));
  strncpy(coinbase, shjson_astr(block, "coinbase", ""), sizeof(coinbase) - 1);
  //strncpy(coinbase, shjson_astr(block, "coinbase", "01000000c5c58853010000000000000000000000000000000000000000000000000000000000000000ffffffff1003a55a0704b4b0b000062f503253482fffffffff014b4c0000000000002321026a51c89c384db03cd9381c08f7a9a48eabd0971cf7d86c8ce1446546be38534fac00000000"), sizeof(coinbase) - 1);

  memset(sig, 0, sizeof(sig));
  strncpy(sig, shjson_astr(block, "coinbaseflags", ""), sizeof(sig) - 1);
  //strncpy(sig, shjson_astr(block, "sigScript", "03a55a0704b4b0b000062f503253482f"), sizeof(sig) - 1);

  ptr = strstr(coinbase, sig);
  if (!ptr) {
fprintf(stderr, "DEBUG: task_init: coinbase does not contain sigScript (coinbase:%s, sig:%s)\n", coinbase, sig);
    shjson_free(&tree);
    task_free(&task);
    return (NULL);
  }

  strncpy(task->cb1, coinbase, strlen(coinbase) - strlen(ptr) - 16 /* xnonce */);
  //xn_len = user->peer.n1_len + user->peer.n2_len;
//  sprintf(task->cb1 + strlen(task->cb1), "%-2.2x", xn_len);

//  sprintf(task->xnonce2, "%-8.8x", shjson_astr(block, "extraNonce", 0));
//  strncpy(task->xnonce2, ptr + 2, 8); /* template xnonce */

  strcpy(task->cb2, ptr);
  //strcpy(task->cb2, ptr + 10);

  task->merkle_len = shjson_array_count(block, "transactions");
  task->merkle = (char **)calloc(task->merkle_len + 1, sizeof(char *));
  for (i = 0; i < task->merkle_len; i++) {
    task->merkle[i] = shjson_array_str(block, "transactions", i); /* alloc'd */
  }



  /* store server generate block. */
//  strncpy(task->tmpl_merkle, shjson_astr(block, "merkleroot", "9f9731f960b976a07de138599ad8c8f1737aecb0f5365c583c4ffdb3a73808d4"), sizeof(task->tmpl_merkle));
 // strncpy(task->xnonce2, ptr + 2 + 8, 8);
  sprintf(task->xnonce2, "%-8.8x", 0);

  task->version = (int)shjson_num(block, "version", BLOCK_VERSION);

  /* previous block hash */
  strncpy(task->prev_hash, shjson_astr(block, "previousblockhash", "0000000000000000000000000000000000000000000000000000000000000000"), sizeof(task->prev_hash) - 1);
/*
  hex2bin(hash_swap, task->prev_hash, 32);
  swap256(task->work.prev_hash, hash_swap);
*/


  strncpy(task->nbits, shjson_astr(block, "bits", "00000000"), sizeof(task->nbits) - 1);
  task->curtime = (time_t)shjson_num(block, "curtime", time(NULL));
  task->height = block_height;

  /* generate unique job id from user and coinbase */
  task->task_id = (unsigned int)shjson_num(block, "task", shcrc(task, sizeof(task_t)));

  shjson_free(&tree);

  /* keep list of shares to check for dups */
  task->share_list = shmeta_init(); /* mem */

  task->next = task_list;
  task_list = task;

  return (task);
}

void task_free(task_t **task_p)
{
  task_t *task;
  int i;

  if (!task_p)
    return;

  task = *task_p;
  *task_p = NULL;

  shmeta_free(&task->share_list);

  if (task->merkle && task->merkle_len) {
    for (i = 0; i < task->merkle_len; i++) {
      free(task->merkle[i]);
    }
    free(task->merkle);
  }

  free(task);
}

task_t *stratum_task(unsigned int task_id)
{
  task_t *task;

int cnt;

cnt = 0;
  for (task = task_list; task; task = task->next) {
    if (task_id = task->task_id)
      break; 
cnt++;
  }

  return (task);
}

/**
 * Monitors when a new accepted block becomes confirmed.
 * @note format: ["height"=<block height>, "category"=<'generate'>, "amount"=<block reward>, "time":<block time>, "confirmations":<block confirmations>]
 */
void check_payout()
{
  shjson_t *tree;
  shjson_t *block;
  user_t *user;
  char block_hash[512];
  char category[64];
  char uname[256];
  const char *templ_json;
  double tot_shares;
  double weight;
  double reward;
  int i;


  templ_json = getblocktransactions();
  if (!templ_json) {
fprintf(stderr, "DEBUG: task_init: getblocktransactions NULL\n");
    return;
  }
//fprintf(stderr, "DEBUG: check_payout: %s\n", templ_json); 

  tree = shjson_init(templ_json);
  if (!tree) {
fprintf(stderr, "DEBUG: task_init: cannot parse json\n");
    return;
  }

  block = shjson_obj(tree, "result");
  if (!block) {
    fprintf(stderr, "DEBUG: task_init: cannot parse json result\n");
    shjson_free(&tree);
    return;
  }

  memset(block_hash, 0, sizeof(block_hash));
  strncpy(block_hash, shjson_astr(block, "blockhash", ""), sizeof(block_hash) - 1);
  if (0 == strcmp(block_hash, "")) {
    fprintf(stderr, "DEBUG: check_payout: no block hash: %s\n", templ_json);
    shjson_free(&tree);
    return;
  }

  if (0 == strcmp(last_payout_hash, ""))
    strcpy(last_payout_hash, block_hash);
  if (0 == strcmp(last_payout_hash, block_hash)) {
    shjson_free(&tree);
    return;
  }

  memset(category, 0, sizeof(category));
  strncpy(category, shjson_astr(block, "category", "none"), sizeof(category) - 1);


  if (0 == strcmp(category, "generate")) {
    double amount = shjson_num(block, "amount", 0);
    double fee;

    if (amount < 1) {
      shjson_free(&tree);
      return;
    }
    fee = amount * 0.001; /* 0.1% */
    amount -= fee;

    tot_shares = 0;
    for (user = client_list; user; user = user->next) {
      for (i = 0; i < MAX_ROUNDS_PER_HOUR; i++)
        tot_shares += user->block_avg[i];
    }

    if (tot_shares < 1)
      return; /* wait until users have registered round shares. */
    
    /* divvy up profit */
    weight = amount / tot_shares;
    for (user = client_list; user; user = user->next) {
      memset(uname, 0, sizeof(uname));
      strncpy(uname, user->worker, sizeof(uname) - 1);
      strtok(uname, "."); 
      if (!*uname)
        continue;

      reward = 0;
      for (i = 0; i < MAX_ROUNDS_PER_HOUR; i++)
        reward += weight * user->block_avg[i];
//      fprintf(stderr, "DEBUG: setblockreward(\"%s\", %f)\n", uname, reward);
      if (reward >= 1)
        setblockreward(uname, reward);  
    }

/*
 * Just leave in main account to avoid transaction charge. 
    if (fee >= 1.0) 
      setblockreward("bank", fee);
*/

  }

  shjson_free(&tree);

  strcpy(last_payout_hash, block_hash);

}

void stratum_round_reset(time_t stamp)
{
  user_t *user;
  int hour;

  hour = ((stamp / 3600) % MAX_ROUNDS_PER_HOUR);
  for (user = client_list; user; user = user->next) {
    user->block_avg[hour] = 
      (user->block_avg[hour] + (double)user->block_tot) / 2;
    user->round_stamp = stamp;
    user->block_tot = 0;
    user->block_cnt = 0;
    user->block_acc = 0;
  }

}

/**
 * Generate MAX_SERVER_NONCE scrypt hashes against a work task.
 * @note Submits a block 
 */
void stratum_task_work(task_t *task)
{
  static int luck = 1;
  static int idx;
  static time_t round_stamp;
  time_t now;
  unsigned int last_nonce;
  char ntime[16];
  int err;

  idx++;
  if (0 != (idx % luck)) {
    return;
  }

  if (!sys_user) {
    /* track server's mining stats. */
    sys_user = stratum_user_init(-1);
    strncpy(sys_user->worker, "anonymous.system", sizeof(sys_user->worker) - 1);
    sys_user->flags |= USER_SYSTEM;
    sys_user->next = client_list;
    client_list = sys_user;
  }

  now = time(NULL);
  if (round_stamp < (now - MAX_ROUND_TIME)) {
    stratum_round_reset(now);
    round_stamp = now;
  }
  
  /* generate block hash */
/*
  memset(&sys_user->peer, 0, sizeof(sys_user->peer));
  sprintf(sys_user->peer.nonce1, "%-8.8x", 0x00000000);
  sys_user->peer.n1_len = 4;
  sys_user->peer.n2_len = 4;
*/
  sys_user->peer.diff = 0.125;
  sprintf(task->work.xnonce2, "%-8.8x", 0x00000000);
sprintf(ntime, "%-8.8x", task->curtime);
  shscrypt_work(&sys_user->peer, &task->work, task->merkle, task->prev_hash, task->cb1, task->cb2, task->nbits, ntime);

  err = shscrypt(&task->work, MAX_SERVER_NONCE);
  if (!err && task->work.nonce != MAX_SERVER_NONCE) {
//    fprintf(stderr, "DEBUG: [SWORK] %d = shscrypt() [sdiff %f, diff %f]\n", err, task->work.sdiff, shscrypt_hash_diff(&task->work));
    luck = MAX(1, (luck / 2));

    err = shscrypt_verify(&task->work);

    if (!err) {
      /* update server's mining stats. */
      stratum_user_block(sys_user, task);

      if (task->work.pool_diff < task->target) {
 //       fprintf(stderr, "DEBUG: [SWORK] share too low for submission (diff %f, target %f).\n", task->work.pool_diff, task->target);
      } else {
        char xn_hex[256];
        uint32_t be_nonce =  htobe32(task->work.nonce);

        sprintf(xn_hex, "%s%s", sys_user->peer.nonce1, task->work.xnonce2);
#if 0
        err = submitblock(task->task_id, task->curtime, task->work.nonce, xn_hex);
fprintf(stderr, "DEBUG: stratum_task_work: %d = submitblock(task %u, time %u, nonce %u, xn %s)\n", err, task->task_id, task->curtime, task->work.nonce, xn_hex);
#endif

        err = submitblock(task->task_id, task->curtime, be_nonce, xn_hex);
fprintf(stderr, "DEBUG: stratum_task_work: %d = submitblock(task %u, time %u, nonce %u, xn %s)\n", err, task->task_id, task->curtime, be_nonce, xn_hex);

      }
    }
  } else {
    luck++;
  }

}

void stratum_task_gen(void)
{
  task_t *task;
  scrypt_peer peer;
  unsigned int last_nonce;
  int time;
  int err;


  task = task_init();
  if (!task)
    return;

  /* notify subscribed clients of new task. */
  stratum_user_broadcast_task(task);

  stratum_task_work(task);

}




