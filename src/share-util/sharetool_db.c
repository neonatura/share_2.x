

/*
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
 */  

#include "share.h"
#include "sharetool.h"

static shdb_t *_sharetool_db;
static shbuf_t *_inbuff;
static int _inbuff_index;



static int _favail(FILE *pFile) 
{
  int             fd;
  fd_set          set;
  struct timeval  tv;
  int             result;

  fd = fileno(pFile);

  FD_ZERO(&set);
  FD_SET(fd, &set);

  /* don't wait */

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  return select(fd + 1, &set, NULL, NULL, &tv); 
}

int sharetool_db_interp(shdb_t *db)
{
  unsigned char *data;
  char stmt[10240];
  int idx;
  int err;

  data = shbuf_data(_inbuff);
  if (!data)
    return (1);

  if (0 == strcmp(data, "quit") ||
      0 == strcmp(data, "exit"))
    return (0); /* exit program */

  while (-1 != (idx = stridx(data, ';'))) {
    memset(stmt, 0, sizeof(stmt));
    strncpy(stmt, data, idx);

    shbuf_trim(_inbuff, idx);
    data = shbuf_data(_inbuff);

    err = shdb_exec(db, stmt);
    if (err) {
      fprintf(stderr, "%s: %s: %s\n", process_path, stmt, sherrstr(err)); 
      if (!(run_flags & PFLAG_IGNORE)) {
        /* return error to caller */
        return (err);
      }
    } else {
      fprintf(sharetool_fout, "SQL: %s\n", stmt);
    }
  }

  return (0);
}


int sharetool_db_stream(shdb_t *db)
{
  static size_t of;
  char line[10240];
  int err;
  
  memset(line, 0, sizeof(line));
  fgets(line, sizeof(line) - 1, stdin);

  if (-1 != stridx(line, '\n'))
    _inbuff_index++;

  if (strlen(line) >= sizeof(line) - 1) {
    of = 0;
    printf ("%s: line %d: line too long (> %d bytes)\n", process_path, _inbuff_index, sizeof(line)-1);
    return (SHERR_INVAL);
  }

  /* append line to input buffer */
  strtok(line, "\r\n");
  shbuf_catstr(_inbuff, line);

  /* process pending input */
  err = sharetool_db_interp(db);
  if (err)
    return (err);

  return (1);
}

int sharetool_db_console(shdb_t *db)
{
  
  printf ("> ");
  fflush(stdout);

  return (sharetool_db_stream(db));
}

void sharetool_db_sig(int sig_nr)
{
  signal(sig_nr, SIG_DFL);

  if (_sharetool_db) {
    shdb_close(_sharetool_db);
    _sharetool_db = NULL;
  }

  shbuf_free(&_inbuff);

  raise(sig_nr);
}

int sharetool_database(char **args, int arg_cnt, int pflags)
{
  shdb_t *db;
  char db_name[MAX_SHARE_NAME_LENGTH];
  int pend_in;
  int err;
  int i;

  if (arg_cnt <= 1)
    return (SHERR_INVAL);

  /* [OPTIONS] [NAME] */  
  memset(db_name, 0, sizeof(db_name));
  for (i = 1; i < arg_cnt; i++) {
    if (args[i][0] == '-') {
      continue;
    }

    if (!*db_name) {
      strncpy(db_name, args[i], sizeof(db_name) - 1);
    }
  }

  /* check whether pending input is available. */
  pend_in = _favail(stdin);
  if (pend_in < 0)
    return (-errno);

  db = shdb_open(db_name);
  if (!db) {
    fprintf(stderr, "%s: %s: error opening database.\n", process_path, db_name);
    return (SHERR_NOENT);
  }

  fprintf(sharetool_fout, "%s: info: opened database '%s'.\n", process_path, db_name);

  _sharetool_db = db;
  signal(SIGINT, sharetool_db_sig);
  signal(SIGQUIT, sharetool_db_sig);

  _inbuff = shbuf_init();
  if (pend_in) {
    do { 
      err = sharetool_db_stream(db);
      if (err) {
        fprintf(stderr, "%s: %s: transaction interrupted by SQL syntax error.\n", process_path, db_name); 
        break;
      }
    } while (_favail(stdin));
  } else {
    /* switch to console mode */
    sharetool_fout = stdout;
    while (1 == sharetool_db_console(db));
  }
  shbuf_free(&_inbuff);

  shdb_close(db);

  return (0);
}

