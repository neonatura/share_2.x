
/*
 *  Copyright 2011 Neo Natura
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

void share_server(char *subcmd)
{
  unsigned int port = (unsigned int)process_socket_port;
  char buff[TEST_BUFFER_SIZE];
  ssize_t b_read, b_write;
  int cli_fd;
  int err;
  int fd;

  if (!*subcmd)
    subcmd = "test";

  err = shfs_proc_lock(process_path, subcmd);
  if (err) {
    printf ("Terminating.. '%s' server '%s' is already running.\n", subcmd, process_path);
    return;
  }

  if (0 == strcmp(subcmd, "ping")) {
    shnet_server_ping();
    return;
  }

  printf ("Initializing '%s' server..\n", subcmd);

  fd = shnet_sk();
  if (fd == -1) {
    perror("shsk");
    return;
  }
  
  err = shnet_bindsk(fd, NULL, port);
  if (err) {
    perror("shbindport");
    shnet_close(fd);
    return;
  }

  printf ("Accepting connections on port %d.\n", port);

  cli_fd = shnet_accept(fd);
  if (cli_fd == -1) {
    perror("shnet_accept");
    shnet_close(fd);
    return;
  }

  printf ("Received new connection on port %d.\n", port);

  memset(buff, 0, sizeof(buff));
  memset(buff, 'a', sizeof(buff) - 1);
  b_write = shnet_write(cli_fd, buff, sizeof(buff));
  if (b_write <= 0) {
    shnet_close(cli_fd);
    shnet_close(fd);
    perror("shnet_write");
return;
  }
  printf ("%d of %d bytes written to port %d on fd %d..\n", b_write, sizeof(buff), port, cli_fd); 

  memset(buff, 0, sizeof(buff));
  b_read = shnet_read(cli_fd, buff, sizeof(buff));
  if (b_read <= 0) {
    perror("shread");
    shnet_close(cli_fd);
    shnet_close(fd);
    return;
  }

  printf ("MESSAGE: %-*.*s\n", b_read, b_read, buff);
  printf ("%d of %d bytes read from port %d on fd %d..\n", b_read, sizeof(buff), port, cli_fd); 
  
  err = shnet_close(fd);
  if (err) {
    perror("shnet_close");
    shnet_close(cli_fd);
    shnet_close(fd);
    return;
  }

    shnet_close(cli_fd);
    shnet_close(fd);

  return (0);
}



shserver_t *netcoin_server_ops(void)
{
  shserver_t *serv;

  serv = default_server_opts();

  SERV_SET_PROXY(serv);
  SERV_SET_TYPE(SERV_NETCOIN);

  serv->read_op = netcoin_read;
  serv->write_op = netcoin_write;
  serv->work_op = netcoin_work;

  return (serv);
}

int netcoin_work(shtime_t *timer)
{

  /* wait 10 seconds */
  *timer += 10;
/*
@spec https://en.bitcoin.it/wiki/BIP_0022

{\"method\": \"getblocktemplate\", \"params\": [{\"data\":".
     "\"020000003c48a294584f90e58325c60ca82896d071826b45680a661cec4d424d00000000".
"de6433d46c0c7f50d84a05aec77be0199176cdd47f77e344b6f50c84380fddba66dc47501d00ff".
"ff0000010001010000000100000000000000000000000000000000000000000000000000000000".
"00000000ffffffff1302955d0f00456c6967697573005047dc66085fffffffff02fff1052a0100".
"00001976a9144ebeb1cd26d6227635828d60d3e0ed7d0da248fb88ac01000000000000001976a9".
"147c866aee1fa2f3b3d5effad576df3dbf1f07475588ac00000000\"}]}

HTTP/1.1 200 OK
Date: Fri, 28 Feb 2014 06:10:01 +0000
Connection: keep-alive
Content-Length: 45
Content-Type: application/json
Server: netcoin-json-rpc/v1.2.0.0

{"result":"rejected","error":null,"id":null}

*/

}

int netcoin_read_json(shserver_t *serv, char json_text)
{
  shjson_t *json;
  shjson_t *resp;
  char *ptr;

  json = shjson(json_text);
  if (!json)
    return (SHERR_INVAL);

  req = shjson_str(json, "method");
  if (0 == strcasecmp(req, "getwork")) {
    resp = shjson(NULL);

    ptr = shmeta_get_str(serv->param, ashkey_str("midstate")); 
    if (ptr) 
      shjson_set_str(resp, "midstate", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("data")); 
    if (ptr) 
      shjson_set_str(resp, "data", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("hash1")); 
    if (ptr) 
      shjson_set_str(resp, "hash1", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("target")); 
    if (ptr) 
      shjson_set_str(resp, "target", ptr);

    netcoin_write_json(serv, resp);
    shjson_free(&resp);
  } else if (0 == strcasecmp(req, "getblocktemplate")) {
    resp = shjson(NULL);

    ptr = shmeta_get_str(serv->param, ashkey_str("previousblockhash")); 
    if (ptr) 
      shjson_set_str(resp, "previousblockhash", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("coinbasevalue")); 
    if (ptr) 
      shjson_set_str(resp, "coinbasevalue", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("target")); 
    if (ptr) 
      shjson_set_str(resp, "target", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("sizelimit")); 
    if (ptr) 
      shjson_set_str(resp, "sizelimit", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("noncerange")); 
    if (ptr) 
      shjson_set_str(resp, "noncerange", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("curtime")); 
    if (ptr) 
      shjson_set_str(resp, "curtime", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("bits")); 
    if (ptr) 
      shjson_set_str(resp, "bits", ptr);
    ptr = shmeta_get_str(serv->param, ashkey_str("height")); 
    if (ptr) 
      shjson_set_str(resp, "height", ptr);

/*
{
            "data" : "0200000001395088cc3cc1779ca70a3ebcedbcfc7fa9e89cde00c5382061428a22c12caf3b000000006b483045022007107f13a230c7e4e7acfbc220e8df82972d32fbc594e5e7a2c75d55d6996a1c0221008b2fc0337bb5a6dfbdcc847b440cf3f70d4e103b1f0f077aefdf299b9b10b2d40121021b0db18cf253fcf40e285b36a15f33f066e327f218cc9a9036fc105b06fc7f48ffffffff021673f606000000001976a9140a73a8037a5ae382be90cb104a19a2b816cb065c88ac922c8406000000001976a91472eb6df471215e1475571f63b234b7790d71bcf088ac0000000000",
            "hash" : "117dc0c7706d39e9b86adb413ffb6ae73c4b1b5aff00312a961692dfb941c69e",
            "fee" : 1000000,
            "depends" : [
            ],
            "sigops" : 2
        }

 */

    netcoin_write_json(serv, resp);
    shjson_free(&resp);
  }

  shjson_free(&json);

  return (0);
}

int netcoin_read_line(serv, char *buf)
{
  char *ptr;
  int err;
  
  ptr = strchr(buf, '\n');
  if (!ptr) {
    /* penalize for taking long. */
    serv->timer++;
    return (0);
  }
  *ptr = '\0';
  ret_len = strlen(buf);
  if (ret_len && buf[ret_len-1] == '\r')
    buf[ret_len-1] = '\0'; /* trim '\r' */

  switch (serv->proto) {
    case PROTO_HTTP_JSON:
    case PROTO_POST:
      if (!*buf && serv->proto == PROTO_HTTP_JSON) {
        serv->state = PROTO_JSON;
        break;
      }
      if (0 == strcasecmp(buf, "content-type: application/json")) {
        serv->proto = PROTO_HTTP_JSON; /* wait for newline */
      } else if (0 == strncmp(buf, "Authorization: Basic ", strlen("Authorization: Basic "))) {
        strncpy(serv->userpass_b64, 
            buf + strlen("Authorization: Basic "), sizeof(serv->userpass_b64) - 1); 
      } else if (0 == strncmp(buf, "Content-Length: ", strlen("Content-Length: "))) {
        serv->data_len = atoi(buf + strlen("Content-Length: "));
      }
      break;

    case PROTO_JSON:
      err = netcoin_read_json(serv, buf);
      break;
  } 

  return (ret_len);
}

int netcoin_read(server_t *serv, char *buf, size_t len)
{
  int err;

  switch (serv->proto) {
    case PROTO_NONE:
      /* reading header */
      if (0 == strncmp(buf, "POST")) {
        serv->proto = SS_POST;
        return (0);
      }
      break;

    case PROTO_POST:
      return (netcoin_read_line(serv, buf));

    case PROTO_JSON:
      return (netcoin_read_line(serv, buf));

  }


  /* junk it */
  return (len);
}




