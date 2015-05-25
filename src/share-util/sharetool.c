
/*
 *  Copyright 2013 Neo Natura
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

char process_path[PATH_MAX + 1];
char process_file_path[PATH_MAX + 1];
char process_outfile_path[PATH_MAX + 1];
char process_socket_host[PATH_MAX + 1];
unsigned int process_socket_port;
int process_run_mode;
int run_flags;

FILE *sharetool_fout;

void print_process_version(void)
{
  char *app_name = shfs_app_name(process_path);
  printf (
      "%s version %s\n"
      "\n"
      "Copyright 2013 Neo Natura\n"
      "Licensed under the GNU GENERAL PUBLIC LICENSE Version 3\n",
      app_name, VERSION); 
}

void print_process_usage(void)
{

  printf (
      "share-util: Command-line tools for the Share Library.\n"
      "\n"
      );
  switch (process_run_mode) {
    case SHM_PACKAGE:
      printf("Usage: %s [COMMAND] [NAME]\n", process_path);
      printf("Managage file distribution packages.\n");
      break;
    case SHM_CERTIFICATE:
      printf("Usage: %s [COMMAND] [NAME]\n", process_path);
      printf("Managage digital certificates.\n");
      break;
    case SHM_FILE_LIST:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("List entries in a shfs partition.\n");
      break;
    case SHM_FILE_MKDIR:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Create a directory in a shfs partition.\n");
      break;
    case SHM_FILE_REMOVE:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Remove a entry in a shfs partition.\n");
      break;
    case SHM_FILE_CAT:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Print a file in a shfs partition.\n");
      break;
    case SHM_FILE_COPY:
      printf("Usage: %s [OPTION] [PATH] ..\n", process_path);
      printf ("Copy a file to another location.\n");
      break;
    case SHM_FILE_INFO:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf ("Print verbose information on a file path.\n");
      break;
    case SHM_FILE_LINK:
      printf("Usage: %s [OPTION] [PATH] ..\n", process_path);
      printf ("Reference a file from another location.\n");
      break;
    case SHM_FILE_DIFF:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Generate a binary patch file from two files.\n");
      break;
    case SHM_FILE_PATCH:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Apply a binary patch file to a file.\n");
      break;
    case SHM_PREF:
      printf("Usage: %s [OPTION] [PREFERENCE] [<value>]\n", process_path);
      printf("Define or view global preferences.\n");
      break;
    case SHM_FILE_REV:
      printf("Usage: %s [OPTION] [COMMAND] [PATH] [@<hash>]\n", process_path);
      printf("Track file revisions.\n");
      break;
    case SHM_FILE_ATTR:
      printf("Usage: %s [OPTION] [+|-][ATTRIBUTE] [PATH]\n", process_path);
      printf("Set file attributes.\n");
      break;
    case SHM_INFO:
      printf("Usage: %s [OPTION] [PEER]\n", process_path);
      printf("Show network information.\n");
      break;
    case SHM_PAM:
      printf("Usage: %s [OPTION] [PEER]\n", process_path);
      printf("Account permission access management.\n");
      break;
    default:
      printf ("Usage: %s [OPTION]\n", process_path);
      break;
  }
  printf
    (
     "\n"
     "Options:\n"
     "\t-h | --help\t\tShows program usage instructions.\n"
     "\t-v | --version\t\tShows program version.\n"
     "\t-l | --list\t\tList additional verbose information.\n"
     "\t-o | --out <path>\tPrint output to a file.\n"
     "\t-r | --recursive\tProcess sub-directories recursively.\n"
    );

  if (process_run_mode == SHM_PAM) {
    printf(
        "\t-s\t\t\tShow status information for an account.\n"
#if 0
        "\t-d\t\t\tDelete an account.\n"
        "\t-e\t\t\tExpire a session token for an account.\n"
        "\t-k\t\t\tLock an account from login.\n"
        "\t-t\t\t\tGenerate a session token for an accout.\n"
#endif
        );
  } else if (process_run_mode == SHM_CERTIFICATE) {
    printf("\t-c | --cert [PATH]\tSpecify a x509 file in sharefs path notation.\n");
  }

  printf("\n");

  if (process_run_mode == SHM_CERTIFICATE) {
    printf (
        "Commands:\n"
        "\tlist\t\t\tList the certificates available in the system.\n"
        "\tcreate <name> [<ca-name>]\tCreate a new system certificate.\n"
        "\tremove <name>\tRemove a system share certificate.\n"
        "\tprint [<name>]\tPrint a certificate's specifications.\n"
        "\n"
        "Managing x509 certificates:\n"
        "\tImport a x509 certificate:\n"
        "\t\tshcert -c x509.crt create <name>\n" 
        "\tPrint a x509 certificate:\n"
        "\t\tshcert -c x509.crt print\n"
        );
  } else if (process_run_mode == SHM_FILE_REV) {
    printf (
        "Commands:\n"
        "\tadd\t\t\tAdd current directory to repository.\n"
        "\tadd <path>\t\tAdd supplemental file(s) to the repository.\n"
        "\tbranch\t\t\tShow all branch revision references.\n"
        "\tbranch <name> [<path>]\tCreate a new repository branch.\n"
        "\tcat [<path>]\t\tPrint last committed revision.\n"
        "\tcat @<hash> [<path>]\tPrint contents of file revision.\n"
        "\tcheckout [<path>]\tSwitch to \"master\" branch.\n"
        "\tcheckout @<hash> [<path>]\n\t\t\t\tSwitch to commit revision.\n"
        "\tcommit\t\t\tCommit current directory's modifications.\n"
        "\tcommit <path>\t\tCommit revision for file(s).\n"
        "\tdiff\t\t\tDisplay the difference between file versions.\n"
        "\tdiff [@hash] [<path>]\tShow modifications since revision.\n"
        "\tlog\t\t\tRevision history of current directory.\n"
        "\tlog [@hash] [<path>]\tDisplay file revision history.\n"
        "\trevert\t\t\tRevert to last commit revision.\n"
        "\trevert [<path>]\t\tRevert working-area to revision.\n"
        "\tstatus\t\t\tDisplay status of modified files.\n"
        "\tstatus [<path>]\t\tDisplay status of file(s).\n"
        "\tswitch\t\t\tSet the current working revision.\n"
        "\tswitch [<path>]\t\tSwitch to \"master\" branch.\n"
        "\tswitch <branch> [<path>]\n\t\t\t\tSwitch working area to branch.\n"
        "\tswitch <tag> [<path>]\tSwitch working area to tag.\n"
        "\tswitch master\t\tSet working area to initial branch.\n"
        "\tswitch PREV\t\tSet working area to prior committed revision.\n"
        "\ttag\t\t\tShow all named tag revision references.\n"
        "\ttag <name> [@hash] [<path>]\n\t\t\t\tTag revision by name.\n"
        "\n"
        "\tNote: Working area defaults to current directory when no path is specified.\n"
//        "Note: Use option '-r' in order to include sub-directory hierarchies in revision operations.\n"
        "\n"
        );
  } else if (process_run_mode == SHM_FILE_ATTR) {
    char *label;
    int i;

    printf(
        "Attributes:\n"
        "\t+a (Arch)\tCopy as tar archive to native file-system.\n"
        "\t-a \t\tCopy directory without archive conversion.\n"
        "\t+c (Compress)\tData content is stored in a compressed format.\n"
        "\t-c \t\tData content is not stored in a compressed format.\n"
        "\t+e (Encrypt)\tData content is stored in a encrypted format.\n"
        "\t-e \t\tData content is not stored in a encrypted format.\n"
        "\t+f (FLock)\tThe underlying data content is locked.\n"
        "\t-f \t\tThe underlying data content is not locked.\n"
        "\t+r (Read)\tThe file has public read access.\n"
        "\t-r \t\tThe file's read access is limited to owner.\n"
        "\t+s (Sync)\tThe underlying data is synchronized with remote peers.\n"
        "\t-s \t\tThe underlying data is limited to local storage.\n"
        "\t+t (Temp)\tThe directory can only be modified by it's owner.\n"
        "\t-t \t\tThe directory can be modified by normal permissions.\n"
        "\t+v (Version)\tCreate a repository to store file revisions.\n"
        "\t-v \t\tRemove a file revision repository.\n"
        "\t+w (Write)\tThe file has public write access.\n"
        "\t-w \t\tThe file's write access is limited to owner.\n"
        "\t+x (Exe)\tThe file has public execute access.\n"
        "\t-x \t\tThe file's execute access is limited to owner.\n");
  }

  if (process_run_mode == SHM_PREF) { 
    printf(
        "Preferences:\n"
        "\tuser.name\tThe login user's real name.\n"
        "\tuser.email\tThe login user's email address.\n"
        "\n"
        );
  } else if (process_run_mode == SHM_INFO ||
      process_run_mode == SHM_PAM) { 
    printf(
        "Peer:\n"
        "\t<app>[:<group>][@<host>[:<port>]]\n"
        "\t\t<app>\tThe application name.\n"
        "\t\t<group>\tA specific group of the application.\n"
        "\t\t<host>\tThe application's network host-name.\n"
        "\t\t<port>\tThe application's network port number.\n"
        "\n"
        );
  } else {
    printf(
        "Paths:\n"
        "\t<filename>\n"
        "\t\tA local hard-drive path in the current directory.\n"
        "\t/<path>/[<filename>]\n"
        "\t\tA path in the default share-fs partition.\n"
        "\thome:/[<path>/][<filename>]\n"
        "\t\tThe user's home share-fs partition.\n"
        "\tfile://<path>/[<filename>]\n"
        "\t\tAn absolute local hard-drive path.\n"
        "\t<app>[:<group>][@<host>[:<port>]]:/<path>/[<filename>]\n"
        "\t\tAn absolute path in a share-fs partition.\n"
        "\n"
        );
  }

  printf(
      "Visit 'http://docs.sharelib.net/' for libshare API documentation.\n"
      "Report bugs to <%s>.\n",
      get_libshare_email()
      );

}

int main(int argc, char **argv)
{
  char out_path[PATH_MAX+1];
  char peer_name[4096];
  char subcmd[256];
  char *app_name;
  char **args;
  int arg_cnt;
  int err_code;
  int pflags;
  int err;
  int i;

  sharetool_fout = stdout;
  process_run_mode = SHM_NONE;

  app_name = shfs_app_name(argv[0]);
  strncpy(process_path, app_name, PATH_MAX);

  if (0 == strcmp(app_name, "shls")) {
    process_run_mode = SHM_FILE_LIST;
  } else if (0 == strcmp(app_name, "shinfo")) {
    process_run_mode = SHM_INFO;
  } else if (0 == strcmp(app_name, "shln")) {
    process_run_mode = SHM_FILE_LINK;
  } else if (0 == strcmp(app_name, "shcp")) {
    process_run_mode = SHM_FILE_COPY;
  } else if (0 == strcmp(app_name, "shstat")) {
    process_run_mode = SHM_FILE_INFO;
  } else if (0 == strcmp(app_name, "shmkdir")) {
    process_run_mode = SHM_FILE_MKDIR;
  } else if (0 == strcmp(app_name, "shrm")) {
    process_run_mode = SHM_FILE_REMOVE;
  } else if (0 == strcmp(app_name, "shar")) {
    process_run_mode = SHM_ARCH_CREATE;
  } else if (0 == strcmp(app_name, "unshar")) {
    process_run_mode = SHM_ARCH_EXTRACT;
  } else if (0 == strcmp(app_name, "shcat")) {
    process_run_mode = SHM_FILE_CAT;
  } else if (0 == strcmp(app_name, "shpref")) {
    process_run_mode = SHM_PREF;
  } else if (0 == strcmp(app_name, "shdiff")) {
    process_run_mode = SHM_FILE_DIFF;
  } else if (0 == strcmp(app_name, "shdelta")) {
    process_run_mode = SHM_FILE_DELTA;
  } else if (0 == strcmp(app_name, "shpatch")) {
    process_run_mode = SHM_FILE_PATCH;
  } else if (0 == strcmp(app_name, "shattr")) {
    process_run_mode = SHM_FILE_ATTR;
  } else if (0 == strcmp(app_name, "shrev")) {
    process_run_mode = SHM_FILE_REV;
  } else if (0 == strcmp(app_name, "shpasswd")) {
    process_run_mode = SHM_PAM;
  } else if (0 == strcmp(app_name, "shpkg")) {
    process_run_mode = SHM_PACKAGE;
  } else if (0 == strcmp(app_name, "shcert")) {
    process_run_mode = SHM_CERTIFICATE;
  }

  args = (char **)calloc(argc+1, sizeof(char *));
  args[0] = strdup(process_path);
  arg_cnt = 1;

  pflags = 0;
  memset(out_path, 0, sizeof(out_path));
  memset(peer_name, 0, sizeof(peer_name));
  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "-l") ||
        0 == strcmp(argv[i], "--list")) {
      pflags |= PFLAG_VERBOSE;
    } else if (0 == strcmp(argv[i], "-r") ||
        0 == strcmp(argv[i], "--recursive")) {
      pflags |= PFLAG_RECURSIVE;
#if 0
    } else if (0 == strcmp(argv[i], "-l") ||
        0 == strcmp(argv[i], "--local")) {
      pflags |= PFLAG_LOCAL;
#endif
    } else if (0 == strcmp(argv[i], "-h") ||
        0 == strcmp(argv[i], "--help")) {
      pflags |= PFLAG_SYNTAX;
    } else if (0 == strcmp(argv[i], "-v") ||
        0 == strcmp(argv[i], "--version")) {
      pflags |= PFLAG_VERSION;
    } else if (0 == strcmp(argv[i], "-o") ||
        0 == strcmp(argv[i], "--out")) {
      if (i + 1 < argc) {
        i++;
        strncpy(out_path, argv[i], sizeof(out_path) - 1);
      } else {
        printf ("%s: warning: no output path specified.", process_path); 
      } 
#if 0
    } else if (0 == strcmp(argv[i], "-c")) {
      if ( (i + 1) < argc ) {
        i++;
        strncpy(peer_name, argv[i], sizeof(peer_name) - 1);
      }
#endif
    } else {
      args[arg_cnt] = strdup(argv[i]);
      arg_cnt++;
    } 
  }

  if (pflags & PFLAG_VERSION) {
    print_process_version();
    exit(0);
  }
  if (pflags & PFLAG_SYNTAX) {
    print_process_usage();
    exit(0);
  }

  if (*out_path) {
    sharetool_fout = fopen(out_path, "wb");
  }

  memset(subcmd, 0, sizeof(subcmd));
  for (i = 1; i < arg_cnt; i++) {
    if (*subcmd)
      strcat(subcmd, " ");
    strcat(subcmd, args[i]);
  }

  err_code = 0;
  run_flags = pflags;
  switch (process_run_mode) {
    case SHM_FILE_LIST:
      share_file_list(subcmd, pflags);
      break;
#if 0
    case SHM_FILE_IMPORT:
      share_file_import(subcmd, pflags);
      break;
#endif
    case SHM_FILE_CAT:
      for (i = 1; i < arg_cnt; i++) {
        err_code = share_file_cat(args[i], pflags);
        if (err_code) {
          fprintf(stderr, "%s: cannot access %s: %s.\n", process_path, subcmd, sherrstr(err_code)); 
        }
      }
      break;
#if 0
    case SHM_FILE_MKDIR:
      share_file_mkdir(subcmd, pflags);
      break;
#endif

    case SHM_FILE_REMOVE:
      share_file_remove(args, arg_cnt, pflags);
      break;

    case SHM_FILE_ATTR:
      share_file_attr(subcmd, pflags);
      break;

    case SHM_FILE_DIFF:
      err = share_file_diff(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s.\n", process_path, sherrstr(err));
      }
      break;

    case SHM_FILE_DELTA:
      err = share_file_delta(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s.\n", process_path, sherrstr(err));
      }
      break;

    case SHM_FILE_PATCH:
      err = share_file_patch(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s.\n", process_path, sherrstr(err));
      }
      break;

    case SHM_PREF:
      err = sharetool_pref(subcmd);
      if (err) {
        if (err == SHERR_INVAL)
          fprintf(stderr, "%s: error: no preference name specified.\n", process_path);
        else if (err == SHERR_NOENT)
          fprintf(stderr, "%s: warning: preference has no value set.\n", process_path);
        return (1);
      }
      break;

    case SHM_FILE_REV:
      err = share_file_revision(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;

    case SHM_FILE_COPY:
      err = share_file_copy(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;

    case SHM_FILE_LINK:
      err = share_file_link(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;

    case SHM_FILE_INFO:
      err = share_file_info(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;

#if 0
    case SHM_ARCH_CREATE:
      err = share_arch_create(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;

    case SHM_ARCH_EXTRACT:
      err = share_arch_extract(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;
#endif

    case SHM_INFO:
      err = share_info(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;

    case SHM_PAM:
      err = sharetool_passwd(args, arg_cnt);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;

    case SHM_PACKAGE:
      err = sharetool_package(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;

    case SHM_CERTIFICATE:
      err = sharetool_certificate(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherrstr(err));
        return (1);
      }
      break;

    default:
      print_process_usage();
      break;
  }

  if (sharetool_fout) {
    fclose(sharetool_fout);
  }

	return (err_code);
}


