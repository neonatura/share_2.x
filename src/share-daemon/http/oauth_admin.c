
/*
 *  Copyright 2016 Neo Natura 
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

#include "sharedaemon.h"
#include <stdio.h>

void oauth_admin_redir_login(shd_t *cli, char *client_id)
{
  char *enc_cid = http_token_encode(client_id);
  char *enc_uri = http_token_encode("/admin");
  char buf[1024];

  sprintf(buf,
      "HTTP/1.1 302 Found\r\n"
      "Location: /auth?response_type=token&client_id=%s&redirect_uri=%s\r\n"
      "Content-Length: 0\r\n"
      "\r\n",
      enc_cid, enc_uri);
  shbuf_catstr(cli->buff_out, buf);

  free(enc_cid);
  free(enc_uri);
}

int oauth_admin_client(shd_t *cli, char *client_id, char *password, char *fullname, char *address, char *zipcode, char *phone, char *title, char *logo_url)
{
  shmap_t *sess;
  char buf[1024];
  int err;

  if (!client_id)
    client_id = "";

  sess = oauth_sess_load(cli, client_id);
  if (!sess)
    return (SHERR_INVAL);

  if (!oauth_sess_login(sess)) {
    oauth_admin_redir_login(cli, client_id);
    return (0);
  }

  if (title && logo_url && (*title || *logo_url)) {
    oauth_sess_client_set(sess, client_id, title, logo_url);
  }

  sprintf(buf, "<html><form action=\"/admin\"><input name=\"client_id\" value=\"%s\" disabled></input><input name=\"title\" value=\"%s\"></input><input name=\"logo_url\" value=\"%s\"><input type=\"submit\"></input></form></html>\r\n", client_id, title?title:"", logo_url?logo_url:"");
  oauth_html_template(cli->buff_out, buf);

  return (0);
}

