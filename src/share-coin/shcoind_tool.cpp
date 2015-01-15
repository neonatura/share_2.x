

#include "server/main.h"
#include "server/net.h"
#include "server/bitcoinrpc.h"
#include "shcoind_rpc.h"

#ifndef WIN32
#include <signal.h>
#endif


using namespace std;
using namespace boost;


int main(int argc, char *argv[])
{
  char username[256];
  char password[256];
  int ret;

  /* load rpc credentials */
  get_rpc_cred(username, password);
  string strUser(username);
  string strPass(username);
  mapArgs["-rpcuser"] = strUser;
  mapArgs["-rpcpassword"] = strPass; 

  /* perform rpc operation */
  ret = CommandLineRPC(argc, argv);

  return (ret);
}




