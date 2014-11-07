
/*
 *  Copyright 2014 Neo Natura
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
*/  

#include <sys/msg.h>
#include <sys/mman.h>
#include "share.h"





static shkey_t _message_peer_key;

static unsigned char *_message_queue[MAX_MESSAGE_QUEUES];
static FILE *_message_queue_fl[MAX_MESSAGE_QUEUES];

unsigned char *shmsg_queue_init(int q_idx)
{
  struct stat st;
  unsigned char *ret_data;
  char path[PATH_MAX+1];
  int err;

  sprintf(path, "%s/msg", get_libshare_path());
  mkdir(path, 0777);
  sprintf(path+strlen(path), "/%x", (unsigned int)q_idx);

  err = stat(path, &st);
  if (err) {
    FILE *fl;
    char *ptr;

    ptr = (char *)calloc(1, MESSAGE_QUEUE_SIZE);
    if (!ptr)
      return (NULL);

    fl = fopen(path, "wb");
    if (!fl)
      return (NULL);

    fwrite(ptr, MESSAGE_QUEUE_SIZE, 1, fl);
    fclose(fl);
  }

  _message_queue_fl[q_idx] = fopen(path, "rb+");
  if (!_message_queue_fl[q_idx])
    return (NULL);

  ret_data = mmap(NULL, MESSAGE_QUEUE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fileno(_message_queue_fl[q_idx]), 0);
  if (ret_data == MAP_FAILED) { 
    fclose(_message_queue_fl[q_idx]);
    _message_queue_fl[q_idx] = NULL;
    return (NULL);
  }

  return (ret_data);
}

void shmsg_queue_free(int q_idx)
{

  if (q_idx < 0 || q_idx >= MAX_MESSAGE_QUEUES)
    return;

  if (_message_queue[q_idx]) {
    munmap(_message_queue[q_idx], MESSAGE_QUEUE_SIZE);
    _message_queue[q_idx] = NULL;
  }

  if (_message_queue_fl[q_idx]) {
    fclose(_message_queue_fl[q_idx]);
    _message_queue_fl[q_idx] = 0;
  }

}

static shmsgq_t *shmsg_queue_map(int msg_qid)
{
  int q_idx;

  q_idx = (msg_qid % MAX_MESSAGE_QUEUES);
  if (!_message_queue[q_idx])
    _message_queue[q_idx] = shmsg_queue_init(q_idx);

  return ((shmsgq_t *)_message_queue[q_idx]);
}

#if 0
unsigned char *shmsg_queue_map(int msg_qid)
{
  int q_idx;

  q_idx = (msg_qid % MAX_MESSAGE_QUEUES);
  if (!_message_queue[q_idx])
    _message_queue[q_idx] = shmsg_queue_init(q_idx);

  return (_message_queue[q_idx]);
}
#endif

int shmsg_lock(shmsgq_t *hdr)
{
  shtime_t t;

  t = shtime64();
  if (hdr->lock_t) {
    if (hdr->lock_t > t)
      return (SHERR_AGAIN);
  }

  hdr->lock_t = t + SHTIME64_ONE_SECOND;
  return (0);
}

void shmsg_unlock(shmsgq_t *hdr)
{
  hdr->lock_t = 0;
}

#if 0
shmsg_t *shmsg_write_map(shmsgq_t *map, int msg_type, shkey_t msg_src)
{
  int start_of;
  int end_of;
  int idx;

  if (!map)
    return (NULL);

  start_of = map->read_idx;
  end_of = map->write_idx;

  if (end_of <= start_of) {
    for (idx = start_of; idx < MAX_MESSAGES_PER_QUEUE; idx++) {
      if (shmsg_read_valid(&map->msg[idx], msg_type, msg_src)) {
        map->read_idx = (idx+1) % MAX_MESSAGES_PER_QUEUE;
        return (&map->msg[idx]);
      }
    }
    for (idx = 0; idx < end_of; idx++) {
      if (shmsg_read_valid(&map->msg[idx], msg_type, msg_src)) {
        map->read_idx = (idx+1) % MAX_MESSAGES_PER_QUEUE;
        return (&map->msg[idx]);
      }
    }
  } else {
    for (idx = start_of; idx < end_of; idx++) {
      if (shmsg_read_valid(&map->msg[idx], msg_type, msg_src)) {
        map->read_idx = (idx+1) % MAX_MESSAGES_PER_QUEUE;
        return (&map->msg[idx]);
      }
    }
  }

  return (NULL);
}
#endif


shmsg_t *shmsg_write_map(shmsgq_t *map)
{
  shmsg_t *msg;
  size_t start_idx;
  size_t end_idx;
  size_t idx;

  start_idx = map->write_idx;
  end_idx = map->read_idx;

  msg = NULL;
  if (map->read_idx <= map->write_idx) {
    for (idx = map->write_idx; idx < MAX_MESSAGES_PER_QUEUE; idx++) {
      msg = &map->msg[idx];
      if (msg->msg_type == 0)
        goto done;
    }
    for (idx = 0; idx < map->read_idx; idx++) {
      msg = &map->msg[idx];
      if (msg->msg_type == 0)
        goto done;
    }
  } else {
    for (idx = map->write_idx; idx <= map->read_idx; idx++) {
      msg = &map->msg[idx];
      if (msg->msg_type == 0)
        goto done;
    }
  }

  if (!(map->flags & SHMSGF_OVERFLOW))
    return (NULL); /* full */

  /* 'overflow' onto next slot */
  idx = end_idx;

done:
  map->write_idx = (idx + 1) % MAX_MESSAGES_PER_QUEUE;
  memset(msg, 0, sizeof(shmsg_t));

  return (msg);
}

int shmsg_write_map_data(shmsgq_t *map, shmsg_t *msg, unsigned char *msg_data, size_t msg_size)
{
  size_t start_of;
  size_t end_of;
  size_t max_len;
  int reset;

  max_len = MESSAGE_QUEUE_SIZE - sizeof(shmsgq_t);

  start_of = map->write_of;
  if (map->write_of >= map->read_of) {
    end_of = MESSAGE_QUEUE_SIZE - sizeof(shmsgq_t);
    reset = TRUE;
  } else {
    end_of = map->read_of;
    reset = FALSE;
  }

  if (reset && (end_of - start_of) < msg_size) {
    start_of = 0;
    end_of = map->read_of;
  }

#if 0
  if ((map->flags & SHMSGF_OVERFLOW) && (end_of - start_of) < msg_size) {
    end_of = MESSAGE_QUEUE_SIZE - sizeof(shmsgq_t);
    if (end_of - map->write_of >= msg_size) {
      start_of = map->write_of;
    } else {
      start_of = 0;
      end_of = map->read_of;
    }
  }
#endif

  if ((end_of - start_of) < msg_size) {
    /* message too big */
    return (SHERR_AGAIN);
  }
  
  /* append message content */
  map->write_of = start_of + msg_size; 
  memcpy((char *)map->data + start_of, msg_data, msg_size);
  msg->msg_size = msg_size;
  msg->msg_of = start_of;

  return (0);
}

int shmsgsnd(int msg_qid, void *msg_data, size_t msg_size, char *msg_type)
{
  shmsg_t *msg;
  shmsg_t *msg_n;
  shmsgq_t *map;
  size_t of;
  int msg_typenum;
  int ret_err;
  int err;

  if (!msg_type)
    return (SHERR_INVAL); /* psuedo-posix */

  /* can only do so much */
  if (msg_size >= (MESSAGE_QUEUE_SIZE - sizeof(shmsgq_t)))
    return (SHERR_AGAIN);

  msg_typenum = 0;
  if (msg_type)
    msg_typenum = shcrc(msg_type, strlen(msg_type));

  map = shmsg_queue_map(msg_qid);
  if (!map) {
    return (SHERR_INVAL);
}

  err = shmsg_lock(map);
  if (err) {
    return (err);
}

  ret_err = 0;

  /* obtain a message slot. note: updates 'map->write_idx'. */
  msg = shmsg_write_map(map);
  if (!msg) {
    ret_err = SHERR_AGAIN; /* no space avail */
    goto done;
  }

  /* write definition contents of message */
  memcpy(&msg->msg_src, &_message_peer_key, sizeof(shkey_t));
  msg->msg_type = msg_typenum;
  msg->msg_qid = msg_qid; 

  /* write data contents of message */
  err = shmsg_write_map_data(map, msg, msg_data, msg_size);
  if (err) {
    msg->msg_type = 0; /* nullify */
    ret_err = err;
    goto done;
  }

done:
  shmsg_unlock(map);


  return (ret_err);
}

_TEST(shmsgsnd)
{
  char cmp_buf[1024];
  char buf[1024];
  int err;
  int id;
  int i;

  id = shmsgget(NULL);
 
i = 1;
  for (i = 0; i < 64; i++) {
    memset(buf, 'a' + (i % 8), sizeof(buf)); 
    _TRUE(0 == shmsgsnd(id, buf, sizeof(buf), "shmsgsnd")); 
  }

  for (i = 0; i < 64; i++) {
    memset(buf, 0, sizeof(buf));
    memset(cmp_buf, 'a' + (i % 8), sizeof(cmp_buf)); 
  
    err = shmsgrcv(id, buf, sizeof(buf), "shmsgsnd", NULL, 0); 
    _TRUE(sizeof(buf) == err);
    _TRUE(0 == memcmp(cmp_buf, buf, 1024));
  }

  err = shmsgrcv(id, buf, sizeof(buf), "shmsgsnd", NULL, 0); 
  _TRUE(SHERR_NOMSG == err);
 
  shmsg_queue_free(id);
}

int shmsg_read_valid(shmsg_t *msg, int msg_qid, int msg_type, shkey_t *msg_src)
{

  if (msg->msg_type == 0) {
    return (FALSE);
  }

  if (msg->msg_size == 0) {
    return (FALSE);
  }

  if (msg_type && msg_type != msg->msg_type) {
    return (FALSE);
  }

  /* verify message source */
  if (msg_src && !shkey_cmp(msg_src, &msg->msg_src))
    return (FALSE);

  return (TRUE);
}

/** Scan a range of messages for readable content. */
shmsg_t *shmsg_read_map(shmsgq_t *map, int msg_qid, int msg_type, shkey_t *msg_src)
{
  int start_of;
  int end_of;
  int idx;

  if (!map)
    return (NULL);

  start_of = map->read_idx;
  end_of = map->write_idx;

  if (end_of <= start_of) {
    for (idx = start_of; idx < MAX_MESSAGES_PER_QUEUE; idx++) {
      if (shmsg_read_valid(&map->msg[idx], msg_qid, msg_type, msg_src)) {
        map->read_idx = (idx+1) % MAX_MESSAGES_PER_QUEUE;
        return (&map->msg[idx]);
      }
    }
    for (idx = 0; idx < end_of; idx++) {
      if (shmsg_read_valid(&map->msg[idx], msg_qid, msg_type, msg_src)) {
        map->read_idx = (idx+1) % MAX_MESSAGES_PER_QUEUE;
        return (&map->msg[idx]);
      }
    }
  } else {
    for (idx = start_of; idx < end_of; idx++) {
      if (shmsg_read_valid(&map->msg[idx], msg_qid, msg_type, msg_src)) {
        map->read_idx = (idx+1) % MAX_MESSAGES_PER_QUEUE;
        return (&map->msg[idx]);
      }
    }
  }

  return (NULL);
}

int shmsgrcv(int msg_qid, void *msg_data, size_t msg_size, char *msg_type, shkey_t *msg_src, int msg_flags)
{
  shmsg_t *msg;
  shmsgq_t *map;
  size_t len;
  size_t of;
  size_t max_len;
  int msg_typenum;
  int ret_len;
  int idx;
  int err;

  map = shmsg_queue_map(msg_qid);
  if (!map) {
    return (SHERR_INVAL);
  }

  msg_typenum = 0;
  if (msg_type)
    msg_typenum = shcrc(msg_type, strlen(msg_type));

  err = shmsg_lock(map);
  if (err)
    return (err);

  msg =  shmsg_read_map(map, msg_qid, msg_typenum, msg_src);
  if (!msg) {
    ret_len = SHERR_NOMSG;
    goto done;
  }

  if (msg_size < msg->msg_size && msg_flags != MSG_NOERROR) {
    ret_len = SHERR_2BIG;
    goto done;
  }

  ret_len = MIN(msg_size, msg->msg_size); 
  memcpy(msg_data, (char *)map->data + msg->msg_of, ret_len);
  msg->msg_type = 0; /* nullify */

done:
  shmsg_unlock(map);
  return (ret_len);
}

int shmsgget(shpeer_t *peer)
{
  shpeer_t *src_peer;
  shmsgq_t *map;
  int q_id;

  src_peer = shpeer();
  memcpy(&_message_peer_key, &src_peer->name, sizeof(shkey_t));
  shpeer_free(&src_peer);

  if (!peer)
    peer = ashpeer();

  q_id = ((int)shcrc(&peer->name, sizeof(peer->name)) % MAX_MESSAGE_QUEUES);

#if 0
  /* append flags to message queue header. */ 
  map = shmsg_queue_map(msg_qid);
  if (!map)
    return (SHERR_IO);

  map->flags |= flags;
#endif

  return (q_id);
}

int shmsgctl(int msg_qid, int cmd, int value)
{

  switch (cmd) {
    case SHMSGF_RMID:
      shmsg_queue_free(msg_qid);
      break;
  }

}


