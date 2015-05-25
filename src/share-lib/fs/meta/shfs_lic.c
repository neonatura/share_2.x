
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


/**
 * Generate a license for a file using the given certificate.
 * @param lic A reference to the license to generate.
 */
int shlic_sign(SHFL *file, shcert_t *cert)
{
  SHFL *lic_fl;
  shbuf_t *buff;
  shlic_t lic;
  shfs_t *tree;
  char path[SHFS_PATH_MAX];
  int err;

  /* apply the certificate to the file. */
  err = shcert_sign(file, cert);
  if (err)
    return (err);

  tree = shfs_inode_tree(file);
  memset(&lic, 0, sizeof(lic));
  memcpy(&lic.lic_peer, shpeer_kpriv(&tree->peer), sizeof(lic.lic_peer));
#if 0
  memcpy(&lic.lic_cert, &cert->cert_key, sizeof(lic.lic_cert));
#endif

  /* save the certificate using certificate 'subject signature' key */
  sprintf(path, "/sys/lic/%s", shkey_hex(&cert->cert_sub.ent_sig.sig_key));
  lic_fl = shfs_file_find(shfs_inode_tree(file), path);
  buff = shbuf_map(&lic, sizeof(shlic_t));
  err = shfs_write(lic_fl, buff);
  free(buff);
  if (err)
    return (err);
  
  /* generate a local-specific signature for the licence file */ 
  err = shfs_sig_gen(lic_fl, NULL);
  if (err)
    return (err);

  return (0);
}

