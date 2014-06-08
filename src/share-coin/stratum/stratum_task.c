
#define __STRATUM__TASK_C__
#include "shcoind.h"


#define BLOCK_VERSION 4

static task_t *task_list;

task_t *task_init(int xn_len)
{
  shjson_t *block;
  task_t *task;
  char *templ_json;
  char coinbase[512];
  char sig[256];
  char *ptr;
  unsigned long cb1;
  unsigned long cb2;
  int i;

  templ_json = getblocktemplate();
  if (!templ_json)
    return (NULL); /* template not currently available. */

  block = shjson_init(templ_json);
  if (!block) {
    return (NULL);
  }

  task = (task_t *)calloc(1, sizeof(task_t));
  if (!task) { 
    shjson_free(&block);
    return (NULL);
  }

  memset(coinbase, 0, sizeof(coinbase));
  strncpy(coinbase, shjson_astr(block, "coinbase", "01000000c5c58853010000000000000000000000000000000000000000000000000000000000000000ffffffff1003a55a0704b4b0b000062f503253482fffffffff014b4c0000000000002321026a51c89c384db03cd9381c08f7a9a48eabd0971cf7d86c8ce1446546be38534fac00000000"), sizeof(coinbase) - 1);

  memset(sig, 0, sizeof(sig));
  strncpy(sig, shjson_astr(block, "sigScript", "03a55a0704b4b0b000062f503253482f"), sizeof(sig) - 1);

  ptr = strstr(coinbase, sig);
  if (!ptr) {
    task_free(&task);
    return (NULL);
  }
  ptr += 8; /* include sig prefix */

  strncpy(task->cb1, coinbase, strlen(coinbase) - strlen(ptr));
  //xn_len = user->peer.n1_len + user->peer.n2_len;
  sprintf(task->cb1 + strlen(task->cb1), "%-2.2x", xn_len);

  sprintf(task->xnonce2, "%-8.8x", shjson_astr(block, "extraNonce", 0));
//  strncpy(task->xnonce2, ptr + 2, 8); /* template xnonce */

  strcpy(task->cb2, ptr + 10);

  task->merkle_len = shjson_array_count(block, "transactions");
  task->merkle = (char **)calloc(task->merkle_len + 1, sizeof(char *));
  for (i = 0; i < task->merkle_len; i++) {
    task->merkle[i] = shjson_array_str(block, "transactions", i); /* alloc'd */
  } 

  /* store server generate block. */
  strncpy(task->tmpl_merkle, shjson_astr(block, "merkleroot", "9f9731f960b976a07de138599ad8c8f1737aecb0f5365c583c4ffdb3a73808d4"), sizeof(task->tmpl_merkle));
  strncpy(task->xnonce2, ptr + 2 + 8, 8);

  task->version = (int)shjson_num(block, "version", BLOCK_VERSION);
  strncpy(task->prev_hash, shjson_str(block, "previousblockhash", "0000000000000000000000000000000000000000000000000000000000000000"), sizeof(task->prev_hash) - 1);
  strncpy(task->nbits, shjson_str(block, "bits", "00000000"), sizeof(task->nbits) - 1);
  task->curtime = (time_t)shjson_num(block, "curtime", time(NULL));
  task->height = (long)shjson_num(block, "height", 0);

  /* generate unique job id from user and coinbase */
  task->task_id = (unsigned int)shcrc(task, sizeof(task_t));

  shjson_free(&block);

  return (task);
}

void task_free(task_t **task_p)
{
  task_t *task;
  if (!task_p)
    return;
  task = *task_p;
  *task_p = NULL;
  free(task);
}

task_t *stratum_task(unsigned int task_id)
{
  task_t *task;

  for (task = task_list; task; task = task->next) {
    if (task_id = task->task_id)
      break; 
  }

  return (task);
}

void stratum_task_gen(void)
{
  task_t *task;
  
  task = task_init(8);

  /* notify subscribed clients of new task. */
  stratum_user_broadcast_task(task);

}


