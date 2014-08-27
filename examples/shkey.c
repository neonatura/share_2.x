
/*
 * @copyright
 *
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
 *
 *  @endcopyright
*/  

#include "share.h"
#include "shkey.h"

/**
 * The following can also be accomplished by running:
 *  shnet file mkdir -f/new_dir
 */


static int run_mode;
static char prog_name[PATH_MAX+1];

/**
 * Displays the program's version information to the command console.
 */
void program_version(void)
{
  printf ("%s version %s (%s)\n"
      "\n"
      "Copyright 2014 Neo Natura\n"
      "Licensed under the GNU GENERAL PUBLIC LICENSE Version 3\n"
      "Visit 'https://github.com/neonatura/share' for more information.\n",
      prog_name, PACKAGE_VERSION, PACKAGE_NAME);
}

/**
 * Displays the program's usage information to the command console.
 */
void program_usage(void)
{
  printf (
    "%s version %s (%s)\n"
    "usage: %s [--gen] [--verify] [--alias <alias>] [--data <context>]\n"
    "\n"
    "Command-line arguments:\n"
    "  --gen\t\tGenerate a new key for the alias.\n"
    "  --verify\t\tVerify data context against existing key-store.\n"
//    "  --import\t\tImport a raw keystore for an alias.\n"
//    "  --export\t\tExport a raw keystore for an alias.\n"
    "  --alias\t\tName of the key-store to utilize.\n"
    "  --data @<filename>\tUse file as context for key generation.\n"
    "  --data <string>\tUse text string as context for key generation.\n"
    "\n"
    "Example of storing, verifying, and retrieiving arbitrary keys.\n"
    "\n"
    "Visit 'https://github.com/neonatura/share' for more information.\n",
    prog_name, PACKAGE_VERSION, PACKAGE_NAME);
}

/**
 * Performs key-store List, Generate, and Verify operations.
 */
int main(int argc, char **argv)
{
  shfs_t *tree;
  shfs_ino_t *file;
  unsigned char *k_data;
  size_t k_len;
  keystore_t **k_list;
  keystore_t *sv_list;
  keystore_t key_data;
  shpeer_t *peer;
  shkey_t *key;
  char path[PATH_MAX+1];
  char alias[1024];
  char *ref_data;
  size_t ref_data_len;
  shkey_t *ref_key;
  int ret_code;
  int rec_nr;
  int err;
  int i;

  peer = shpeer_app(PROGRAM_NAME);

  memset(prog_name, 0, sizeof(prog_name));
  strncpy(prog_name, argv[0], sizeof(prog_name) - 1);

  run_mode = RUN_LIST;
  memset(alias, 0, sizeof(alias));

  ref_data = NULL;
  ref_data_len = 0;

  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "-h") ||
        0 == strcmp(argv[i], "--help")) {
      program_usage();
      return (0);
    }
    if (0 == strcmp(argv[i], "-v") ||
       0 == strcmp(argv[i], "--version")) {
      program_version();
      return (0);
    }
    if (0 == strcmp(argv[i], "--gen")) {
      run_mode = RUN_GENERATE;
      continue;
    }
    if (0 == strcmp(argv[i], "--verify")) {
      run_mode = RUN_VERIFY;
      continue;
    }
    if (0 == strcmp(argv[i], "--alias")) {
      if ((i+1) < argc && argv[i+1][0] != '-') {
        strncpy(alias, argv[i+1], sizeof(alias) - 1);
        i++;
      }
      continue;
    }
    if (0 == strcmp(argv[i], "--data")) {
      if ((i+1) < argc && argv[i+1][0] != '-') {
        if (argv[i+1][0] == '@') {
          err = shfs_read_mem(argv[i+1] + 1, &ref_data, &ref_data_len);
          if (err) {
            fprintf(stderr, "%s: %s\n", argv[i+1] + 1, str_sherr(err));
            return (1);
          }
        } else {
          ref_data = strdup(argv[i+1]);
          ref_data_len = strlen(argv[i+1]);
        }
        i++;
      }
    }
  }

  if (!ref_data)
    ref_data = strdup("");

  tree = shfs_init(peer);

  key = shkey_str(alias);
  sprintf(path, "/data/%s", shkey_print(key));
  file = shfs_file_find(tree, path);

  rec_nr = 0;
  k_list = NULL;
  err = shfs_file_read(file, &k_data, &k_len);
  if (!err) {
    rec_nr = (k_len / sizeof(keystore_t));
    k_list = (keystore_t **)calloc(rec_nr + 1, sizeof(keystore_t *));
    for (i = 0; i < rec_nr; i++) {
      k_list[i] = (keystore_t *)(k_data + (sizeof(keystore_t) * i));
    }
  }

  printf ("Alias: %s (\"%s\")\n", shkey_print(key), alias);

  ret_code = 0;
  switch (run_mode) {
    case RUN_LIST:
      if (k_list) {
        for (i = 0; k_list[i]; i++) {
          printf ("\n");
          printf ("Timestamp: %-19.19s\n", ctime(&k_list[i]->stamp)); 
          printf ("Context: %s\n", shkey_print(&k_list[i]->context));
        }
      }
      break;

    case RUN_GENERATE:
      /* initialize keystore record */
      memset(&key_data, 0, sizeof(key_data));
      key_data.stamp = shtime64();

      /* generate key referencing context data. */
      ref_key = shkey_bin(ref_data, ref_data_len);
      memcpy(&key_data.context, ref_key, sizeof(shkey_t)); 
      shkey_free(&ref_key);

      /* create new keystore list */
      rec_nr++;
      sv_list = (keystore_t *)calloc(rec_nr, sizeof(keystore_t));
      if (k_list) {
        for (i = 0; k_list[i]; i++)
          memcpy(&sv_list[i], k_list[i], sizeof(keystore_t));
      }
      memcpy(&sv_list[i], &key_data, sizeof(keystore_t));

      /* write keystore list to share file */
      err = shfs_file_write(file, sv_list, sizeof(keystore_t) * rec_nr);
      if (err) {
        fprintf(stderr, "%s: %s\n", path, str_sherr(err));
        ret_code = 1;
      }
      break;

    case RUN_VERIFY:
      if (k_list) {
        ref_key = shkey_bin(ref_data, ref_data_len);
        for (i = 0; k_list[i]; i++) {
          if (0 == memcmp(ref_key, &k_list[i]->context, sizeof(shkey_t)))
            break; /* found matching key */
        }
        shkey_free(&ref_key);
      }

      if (!k_list || !k_list[i]) {
        /* context is not referenced in key-store. */
        printf ("No matching context was found.\n");
        ret_code = 1;
      } else {
        printf ("The context was verified successfully.\n");
      }
      break;

    case RUN_IMPORT:
      break;

    case RUN_EXPORT:
      break;
  }

  free(ref_data);
  shkey_free(&key);
  shfs_free(&tree);

  return (ret_code);
}


