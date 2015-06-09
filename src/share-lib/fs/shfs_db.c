


#include "share.h"
#include "shfs_db_sqlite.h"






static int shdb_col_num_cb(void *p, int arg_nr, char **args, char **cols)
{
  uint64_t *val = (uint64_t *)p;

  *val = 0;
  if (arg_nr > 0) {
    *val = atoll(*args);
}

  return (0);
}

static int shdb_col_value_cb(void *p, int arg_nr, char **args, char **cols)
{
  char **value_p = (char **)p;

  *value_p = NULL;
  if (arg_nr > 0)
    *value_p = strdup(*args);

  return (0);
}

static _shdb_open_index;

int shdb_init(char *path, shdb_t **db_p)
{
  int err;

  *db_p = NULL;
  err = sqlite3_open_v2(path, db_p, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI, 0);
  if (err)
    return (err);

  _shdb_open_index++;
  shdb_exec(*db_p, "PRAGMA journal_mode=OFF");

  return (0);
}

shdb_t *shdb_open_file(SHFL *file)
{
  shdb_t *db;
  char path[SHFS_PATH_MAX];
  int err;

  memset(path, 0, sizeof(path));
  strncpy(path, shpeer_print(shfs_inode_peer(file)), sizeof(path)-2);
  strcat(path, ":");
  strncat(path, shfs_inode_path(file), sizeof(path)-strlen(path)-1);
  err = shdb_init(path, &db);
  if (err)
    return (NULL);

  return (db);
}

shdb_t *shdb_open(char *db_name)
{
  shpeer_t *peer;
  shdb_t *db;
  char path[SHFS_PATH_MAX];
  int err;

  memset(path, 0, sizeof(path));
  sprintf(path, "%s/%s", shpeer_get_app(ashpeer()), db_name);
  err = shdb_init(shfs_sys_dir(SHFS_DIR_DATABASE, path), &db);
  if (err)
    return (NULL);

  return (db);
}

void shdb_close(shdb_t *db)
{
  int err;

  err = sqlite3_close(db);
  if (err) {
    sherr(SHERR_IO, "shdb_close");
    return;
  }

  if (_shdb_open_index > 0) {
    _shdb_open_index--;
    if (_shdb_open_index == 0)
      sqlite3_shutdown();
  }
}

int shdb_exec(shdb_t *db, char *sql)
{
  char *errmsg;
  int err;

  errmsg = NULL;
  err = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (err) {
    if (errmsg) {
      sherr(SHERR_INVAL, errmsg);
      sqlite3_free(errmsg); 
    }
    return (SHERR_INVAL);
  }

  return (0);
}

int shdb_exec_cb(shdb_t *db, char *sql, shdb_cb_t func, void *arg)
{
  char *errmsg;
  int err;

  errmsg = NULL;
  err = sqlite3_exec(db, sql, func, arg, &errmsg);
  if (err) {
    if (errmsg) {
      sherr(SHERR_IO, errmsg);
      sqlite3_free(errmsg);
    }
    return (SHERR_INVAL);
  }

  return (0);
}

int shdb_table_new(shdb_t *db, char *table)
{
  char sql[1024];
  int err;

  if (!table || strlen(table) > 256)
    return (SHERR_INVAL);

  sprintf(sql, "create table IF NOT EXISTS %s ( _rowid INTEGER PRIMARY KEY ASC )", table);
  err = shdb_exec(db, sql); 
  if (err)
    return (err);

  return (0);
}

int shdb_table_like(shdb_t *db, char *table, char *tmpl_table)
{
  char sql[1024];
  int err;

  if (!table || strlen(table) > 256)
    return (SHERR_INVAL);

  if (!tmpl_table || strlen(tmpl_table) > 256)
    return (SHERR_INVAL);

  sprintf(sql, "create table %s like %s", table, tmpl_table);
  err = shdb_exec(db, sql); 
  if (err)
    return (err);

  return (0);
}

int shdb_table_delete(shdb_t *db, char *table)
{
  char sql[512];

  if (!table || strlen(table) > 256)
    return (SHERR_INVAL);

  sprintf(sql, "drop table %s", table);
  return (shdb_exec(db, sql));
}

int shdb_table_copy(shdb_t *db, char *orig_table, char *new_table)
{
  char sql[1024];
  int err;

  err = shdb_table_like(db, new_table, orig_table);
  if (err)
    return (err);

  sprintf(sql, "insert into %s select * from %s", orig_table, new_table);
  err = shdb_exec(db, sql); 
  if (err) {
    shdb_table_delete(db, new_table);
    return (err);
  }

  return (0);
}

int shdb_col_new(shdb_t *db, char *table, char *col)
{
  char sql[1024];
  int err;

  if (!table || strlen(table) > 256)
    return (SHERR_INVAL);
  if (!col || strlen(col) > 256)
    return (SHERR_INVAL);

  sprintf(sql, "alter table %s add column %s text", table, col);
  err = shdb_exec(db, sql); 
  if (err)
    return (err);

  return (0);
}


int shdb_row_new(shdb_t *db, char *table, uint64_t *rowid_p)
{
  char sql[1024];
  int err;

  if (!table || strlen(table) > 256)
    return (SHERR_INVAL);

  sprintf(sql, "insert into %s (_rowid) values (null)", table);
  err = shdb_exec(db, sql);
  if (err)
    return (err);

  *rowid_p = (uint64_t)sqlite3_last_insert_rowid(db);

  return (0);
}

int shdb_row_set(shdb_t *db, char *table, uint64_t rowid, char *col, char *text)
{
  char *sql;
  int err;

  sql = sqlite3_mprintf(
      "update %s set %s = %Q where _rowid = %llu",
      table, col, text, rowid);
  err = shdb_exec(db, sql);
  sqlite3_free(sql);
  if (err)
    return (err);

  return (0);
}

int shdb_row_set_time(shdb_t *db, char *table, uint64_t rowid, char *col)
{
  char sql[1024];
  int err;

  if (!table || strlen(table) > 256)
    return (SHERR_INVAL);

  sprintf(sql, 
      "update %s set %s = CURRENT_TIMESTAMP where _rowid = %llu",
      table, col, rowid);
  err = shdb_exec(db, sql);
  if (err)
    return (err);

  return (0);
}

time_t shdb_row_time(shdb_t *db, char *table, uint64_t rowid, char *col)
{
  char sql[1024];
  uint64_t val;
  int err;

  if (!table || strlen(table) > 256)
    return (SHERR_INVAL);

  val = 0;
  sprintf(sql, 
      "select strftime('%%s', %s) from %s where _rowid = %llu",
      col, table, rowid);
  err = shdb_exec_cb(db, sql, shdb_col_num_cb, &val);
  if (err)
    return (0);

  return ((time_t)val);
}

char *shdb_row_value(shdb_t *db, char *table, uint64_t rowid, char *col)
{
  char sql[1024];
  char *value;
  int err;

  if (!table || strlen(table) > 256)
    return (NULL);
  if (!col || strlen(col) > 256)
    return (NULL);

  value = NULL;
  sprintf(sql, "select %s from %s where _rowid = %llu", col, table, rowid);
  err = shdb_exec_cb(db, sql, shdb_col_value_cb, &value);
  if (err)
    return (NULL);

  return (value);
}






_TEST(shfs_db)
{
  shdb_t *db;
  char *errmsg;
  char *str;
  char sql[256];
  uint64_t rowid;
  time_t t;
  time_t now;
  int err;

  db = shdb_open("test");
  _TRUEPTR(db);

  err = shdb_table_new(db, "test");
  _TRUE(0 == err);

  _TRUE(0 == shdb_col_new(db, "test", "fld1"));
  _TRUE(0 == shdb_col_new(db, "test", "fld2"));
  _TRUE(0 == shdb_col_new(db, "test", "fld3"));

  rowid = 0;
  err = shdb_row_new(db, "test", &rowid);
  _TRUE(0 == err);

  err = shdb_row_set(db, "test", rowid, "fld1", "text1");
  _TRUE(0 == err);
  err = shdb_row_set(db, "test", rowid, "fld2", "text2");
  _TRUE(0 == err);
  now = time(NULL);
  err = shdb_row_set_time(db, "test", rowid, "fld3");
  _TRUE(0 == err);

  str = shdb_row_value(db, "test", rowid, "fld1");
  _TRUEPTR(str);
  free(str);
  str = shdb_row_value(db, "test", rowid, "fld2");
  _TRUEPTR(str);
  free(str);
  t = shdb_row_time(db, "test", rowid, "fld3");
  _TRUE(t >= now);

  err = shdb_table_delete(db, "test");
  _TRUE(err == 0);


  shdb_close(db);

}



int shfs_db_read_of(shfs_ino_t *file, shbuf_t *buff, off_t of, size_t size)
{
  int err;
  shfs_ino_t *aux;

  if (file == NULL)
    return (SHERR_INVAL);

  if (shfs_format(file) != SHINODE_DATABASE)
    return (SHERR_INVAL);

  aux = shfs_inode(file, NULL, SHINODE_DATABASE);
  if (!aux)
    return (SHERR_IO);

  err = shfs_aux_pread(aux, buff, of, size);
  if (err)
    return (err);

  return (0);
}

/** Read raw database content from a file. */
int shfs_db_read(shfs_ino_t *file, shbuf_t *buff)
{
  return (shfs_db_read_of(file, buff, 0, 0));
}

int shfs_db_write(shfs_ino_t *file, shbuf_t *buff)
{
  shfs_ino_t *aux;
  int err;

  if (file == NULL)
    return (SHERR_INVAL);

  aux = shfs_inode(file, NULL, SHINODE_DATABASE);
  if (!aux)
    return (SHERR_IO);

    err = shfs_aux_write(aux, buff);
    if (err)
      return (err);

  /* copy aux stats to file inode. */
  file->blk.hdr.mtime = aux->blk.hdr.mtime;
  file->blk.hdr.size = aux->blk.hdr.size;
  file->blk.hdr.crc = aux->blk.hdr.crc;
  file->blk.hdr.format = SHINODE_DATABASE;

  return (0);
}


