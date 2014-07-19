
/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */

#include "db.h"
#include "walletdb.h"
#include "bitcoinrpc.h"
#include "net.h"
#include "init.h"
#include "util.h"
#include "ui_interface.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/algorithm/string/predicate.hpp>

#ifndef WIN32
#include <signal.h>
#endif

using namespace std;
using namespace boost;
using namespace json_spirit;

extern CWallet* pwalletMain;
extern CClientUIInterface uiInterface;

extern int64 GetAccountBalance(CWalletDB& walletdb, const string& strAccount, int nMinDepth);

extern void AcentryToJSON(const CAccountingEntry& acentry, const string& strAccount, Array& ret);

extern string JSONRPCReply(const Value& result, const Value& error, const Value& id);

extern void ListTransactions(const CWalletTx& wtx, const string& strAccount, int nMinDepth, bool fLong, Array& ret);

class DescribeAddressVisitor : public boost::static_visitor<Object>
{
public:
    Object operator()(const CNoDestination &dest) const { return Object(); }

    Object operator()(const CKeyID &keyID) const {
        Object obj;
        CPubKey vchPubKey;
        pwalletMain->GetPubKey(keyID, vchPubKey);
        obj.push_back(Pair("isscript", false));
        obj.push_back(Pair("pubkey", HexStr(vchPubKey.Raw())));
        obj.push_back(Pair("iscompressed", vchPubKey.IsCompressed()));
        return obj;
    }

    Object operator()(const CScriptID &scriptID) const {
        Object obj;
        obj.push_back(Pair("isscript", true));
        CScript subscript;
        pwalletMain->GetCScript(scriptID, subscript);
        std::vector<CTxDestination> addresses;
        txnouttype whichType;
        int nRequired;
        ExtractDestinations(subscript, whichType, addresses, nRequired);
        obj.push_back(Pair("script", GetTxnOutputType(whichType)));
        Array a;
        BOOST_FOREACH(const CTxDestination& addr, addresses)
            a.push_back(CBitcoinAddress(addr).ToString());
        obj.push_back(Pair("addresses", a));
        if (whichType == TX_MULTISIG)
            obj.push_back(Pair("sigsrequired", nRequired));
        return obj;
    }
};


string address;


int c_LoadWallet(void)
{
    int64 nStart;
    std::ostringstream strErrors;

    const char* pszP2SH = "/P2SH/";
    COINBASE_FLAGS << std::vector<unsigned char>(pszP2SH, pszP2SH+strlen(pszP2SH));

    if (!bitdb.Open(GetDataDir()))
    {
fprintf(stderr, "error: unable to open data directory.\n");
        return (-1);
    }

    if (!LoadBlockIndex()) {
fprintf(stderr, "error: unable to open load block index.\n");
      return (-1);
    }

    nStart = GetTimeMillis();
    bool fFirstRun = true;
    pwalletMain = new CWallet("wallet.dat");
    pwalletMain->LoadWallet(fFirstRun);

    if (fFirstRun)
    {

        // Create new keyUser and set as default key
        RandAddSeedPerfmon();

        CPubKey newDefaultKey;
        if (!pwalletMain->GetKeyFromPool(newDefaultKey, false))
            strErrors << _("Cannot initialize keypool") << "\n";
        pwalletMain->SetDefaultKey(newDefaultKey);
        if (!pwalletMain->SetAddressBookName(pwalletMain->vchDefaultKey.GetID(), ""))
            strErrors << _("Cannot write default address") << "\n";
    }

    printf("%s", strErrors.str().c_str());

    RegisterWallet(pwalletMain);

    CBlockIndex *pindexRescan = pindexBest;
    if (GetBoolArg("-rescan"))
        pindexRescan = pindexGenesisBlock;
    else
    {
        CWalletDB walletdb("wallet.dat");
        CBlockLocator locator;
        if (walletdb.ReadBestBlock(locator))
            pindexRescan = locator.GetBlockIndex();
    }
    if (pindexBest != pindexRescan && pindexBest && pindexRescan && pindexBest->nHeight > pindexRescan->nHeight)
    {
        printf("Rescanning last %i blocks (from block %i)...\n", pindexBest->nHeight - pindexRescan->nHeight, pindexRescan->nHeight);
        nStart = GetTimeMillis();
        pwalletMain->ScanForWalletTransactions(pindexRescan, true);
        printf(" rescan      %15"PRI64d"ms\n", GetTimeMillis() - nStart);
    }


    /** load peers */
    printf("Loading addresses...\n");
    nStart = GetTimeMillis();

    {
        CAddrDB adb;
        if (!adb.Read(addrman))
            printf("Invalid or missing peers.dat; recreating\n");
    }

    printf("Loaded %i addresses from peers.dat  %"PRI64d"ms\n",
           addrman.size(), GetTimeMillis() - nStart);

    RandAddSeedPerfmon();
    pwalletMain->ReacceptWalletTransactions();
}

CBitcoinAddress GetNewAddress(string strAccount)
{
  if (!pwalletMain->IsLocked())
    pwalletMain->TopUpKeyPool();

  // Generate a new key that is added to wallet
  CPubKey newKey;
  if (!pwalletMain->GetKeyFromPool(newKey, false)) {
    throw JSONRPCError(-12, "Error: Keypool ran out, please call keypoolrefill first");
  }
  CKeyID keyID = newKey.GetID();

  pwalletMain->SetAddressBookName(keyID, strAccount);

  return CBitcoinAddress(keyID);
}
string c_getnewaddress(string strAccount)
{
  return (GetNewAddress(strAccount).ToString());
/*
  if (!pwalletMain->IsLocked())
    pwalletMain->TopUpKeyPool();

  // Generate a new key that is added to wallet
  CPubKey newKey;
  if (!pwalletMain->GetKeyFromPool(newKey, false))
    throw JSONRPCError(-12, "Error: Keypool ran out, please call keypoolrefill first");
  CKeyID keyID = newKey.GetID();

  pwalletMain->SetAddressBookName(keyID, strAccount);

  return CBitcoinAddress(keyID).ToString();
*/
}

CBitcoinAddress GetAddressByAccount(const char *accountName)
{
  CBitcoinAddress address;
  string strAccount(accountName);
  Array ret;
  bool found = false;

  // Find all addresses that have the given account
  BOOST_FOREACH(const PAIRTYPE(CBitcoinAddress, string)& item, pwalletMain->mapAddressBook)
  {
    const CBitcoinAddress& acc_address = item.first;
    const string& strName = item.second;
    if (strName == strAccount) {
      address = acc_address;
      found = true;
    }
  }

  /* create one */
  if (!found)
    address = GetNewAddress(strAccount);

  return (address);
}

const char *c_getaddressbyaccount(const char *accountName)
{
  address = GetAddressByAccount(accountName).ToString();
  return (address.c_str());
/*
  string strAccount(accountName);
  Array ret;
  bool found = false;

  // Find all addresses that have the given account
  BOOST_FOREACH(const PAIRTYPE(CBitcoinAddress, string)& item, pwalletMain->mapAddressBook)
  {
    const CBitcoinAddress& acc_address = item.first;
    const string& strName = item.second;
    if (strName == strAccount) {
      address = acc_address.ToString();
      found = true;
    }
  }

  if (!found)
    address = c_getnewaddress(strAccount);

  return (address.c_str());
*/
}

/**
 * Sends a reward to a particular address.
 */
int c_setblockreward(const char *accountName, double dAmount)
{
  CWalletDB walletdb(pwalletMain->strWalletFile);
  string strMainAccount("");
  string strAccount(accountName);
  string strComment("sharenet");
  int64 nAmount;
  Array ret;
  int nMinDepth = 1; /* single confirmation requirement */
  int nMinConfirmDepth = 1; /* single confirmation requirement */
  bool found = false;
  int64 nBalance;

  if (pwalletMain->IsLocked()) {
fprintf(stderr, "DEBUG: wallet is locked.\n");
    return (-13);
}

  const CBitcoinAddress address = GetAddressByAccount(accountName);



  if (dAmount <= 0.0 || dAmount > 84000000.0) {
fprintf(stderr, "DEBUG: invalid amount (%f)\n", dAmount);
    //throw JSONRPCError(-3, "Invalid amount");
    return (-3);
  }

  nAmount = roundint64(dAmount * COIN);
  if (!MoneyRange(nAmount)) {
fprintf(stderr, "DEBUG: invalid amount: !MoneyRange(%d)\n", (int)nAmount);
    //throw JSONRPCError(-3, "Invalid amount");
    return (-3);
  }


  nBalance  = GetAccountBalance(walletdb, strMainAccount, nMinConfirmDepth);
  if (nAmount > nBalance) {
fprintf(stderr, "DEBUG: account has insufficient funds\n");
    //throw JSONRPCError(-6, "Account has insufficient funds");
    return (-6);
  }

  //address = GetAddressByAccount(accountName);
  if (!address.IsValid()) {
fprintf(stderr, "DEBUG: invalid usde address destination\n");
    //throw JSONRPCError(-5, "Invalid usde address");
    return (-5);
  }

/*
  BOOST_FOREACH(const PAIRTYPE(CBitcoinAddress, string)& item, pwalletMain->mapAddressBook)
  {
    const CBitcoinAddress& acc_address = item.first;
    const string& strName = item.second;
    if (strName == strAccount) {
      address = acc_address;
      found = true;
    }
  }

  if (!found) {
    if (!pwalletMain->IsLocked())
      pwalletMain->TopUpKeyPool();

    // Generate a new key that is added to wallet
    CPubKey newKey;
    if (!pwalletMain->GetKeyFromPool(newKey, false)) {
      //throw JSONRPCError(-12, "Error: Keypool ran out, please call keypoolrefill first");
      return (-12);
    }
    CKeyID keyID = newKey.GetID();
    pwalletMain->SetAddressBookName(keyID, strAccount);
    address = CBitcoinAddress(keyID);
  }
  
*/

  CWalletTx wtx;
  wtx.strFromAccount = strMainAccount;
  wtx.mapValue["comment"] = strComment;
  string strError = pwalletMain->SendMoneyToDestination(address.Get(), nAmount, wtx);
  if (strError != "") {
fprintf(stderr, "DEBUG: '%s' = SendMoneyTo: amount %d\n", strError.c_str(), (int)nAmount);
    //throw JSONRPCError(-4, strError);
    return (-4);
  }

fprintf(stderr, "DEBUG: c_set_block_reward: reward (%s -> %f).\n", accountName, dAmount);


  return (0);
}

double c_getaccountbalance(const char *accountName)
{
  CWalletDB walletdb(pwalletMain->strWalletFile);
  string strAccount(accountName);

  int nMinDepth = 1;
  int64 nBalance = GetAccountBalance(walletdb, strAccount, nMinDepth);

  return ((double)nBalance / (double)COIN);
}


/**
 * local known transactions associated with account name.
 * @returns json string format 
 */
string addresstransactioninfo_json;
const char *c_getaddresstransactioninfo(const char *tx_account)
{

  if (!tx_account)
    return (NULL);

  string strAccount(tx_account);

  Array result;
  CWalletDB walletdb(pwalletMain->strWalletFile);

  for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it) {
    CWalletTx* wtx = &((*it).second);
    ListTransactions(*wtx, strAccount, 0, true, result);
  }

/*
  list<CAccountingEntry> acentries;
  walletdb.ListAccountCreditDebit(strAccount, acentries);
  BOOST_FOREACH(CAccountingEntry& entry, acentries) {
    AcentryToJSON(entry, strAccount, result);
  }
*/

  addresstransactioninfo_json = JSONRPCReply(result, Value::null, Value::null);
  return (addresstransactioninfo_json.c_str());
}

string addressinfo_json;
const char *c_getaddressinfo(const char *addr_hash)
{
  string strAddr(addr_hash);
  CBitcoinAddress address(strAddr);

  bool isValid = address.IsValid();
  if (!isValid)
    return (NULL);

  Object result;

  CTxDestination dest = address.Get();
  string currentAddress = address.ToString();
  result.push_back(Pair("address", currentAddress));
  bool fMine = IsMine(*pwalletMain, dest);

  Object detail = boost::apply_visitor(DescribeAddressVisitor(), dest);
  result.insert(result.end(), detail.begin(), detail.end());

  if (pwalletMain->mapAddressBook.count(dest))
    result.push_back(Pair("account", pwalletMain->mapAddressBook[dest]));

  addressinfo_json = JSONRPCReply(result, Value::null, Value::null);
  return (addressinfo_json.c_str());
}


#ifdef __cplusplus
extern "C" {
#endif

int load_wallet(void)
{
  return (c_LoadWallet());
}

const char *getaddressbyaccount(const char *accountName)
{
  if (!*accountName)
    return ("");
  return (c_getaddressbyaccount(accountName));
}

double getaccountbalance(const char *accountName)
{
  return (c_getaccountbalance(accountName));
}

int setblockreward(const char *accountName, double amount)
{
  if (!*accountName)
    return (-5); /* invalid usde address */
  return (c_setblockreward(accountName, amount));
}

const char *getaddresstransactioninfo(const char *hash)
{
  return (c_getaddresstransactioninfo(hash));
}

const char *getaddressinfo(const char *addr_hash)
{
  return (c_getaddressinfo(addr_hash));
}

#ifdef __cplusplus
}
#endif


