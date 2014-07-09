
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

#include "main.h"
#include "wallet.h"
#include "db.h"
#include "walletdb.h"
#include "net.h"
#include "init.h"
#include "ui_interface.h"
#include "base58.h"
#include "bitcoinrpc.h"
#include "../server_iface.h" /* BLKERR_XXX */

#undef printf
#include <boost/asio.hpp>
#include <boost/asio/ip/v6_only.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/shared_ptr.hpp>
#include <list>

#define printf OutputDebugStringF

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace json_spirit;

map<int, CBlock*>mapWork;
string blocktemplate_json; 
string mininginfo_json; 

extern std::string HexBits(unsigned int nBits);
extern string JSONRPCReply(const Value& result, const Value& error, const Value& id);
extern Value ValueFromAmount(int64 amount);
extern void WalletTxToJSON(const CWalletTx& wtx, Object& entry);

extern double GetDifficulty(const CBlockIndex* blockindex = NULL);

extern void ListTransactions(const CWalletTx& wtx, const string& strAccount, int nMinDepth, bool fLong, Array& ret);

/**
 * Generate a block to work on.
 * @returns JSON encoded block state information
 */
const char *c_getblocktemplate(void)
{
  static CReserveKey reservekey(pwalletMain);
  static unsigned int nTransactionsUpdatedLast;
  static CBlockIndex* pindexPrev;
  static unsigned int work_id;
  CBlock* pblock;

  /* DEBUG: required for release
     if (vNodes.empty())
     return (NULL);
     */

  /* DEBUG: required for release
     if (IsInitialBlockDownload())
     return (NULL);
     */

  if (!pwalletMain) {
    fprintf(stderr, "DEBUG: CreateNewBlock: Wallet not initialized.");
    return (NULL);
  }

  // Update block

  pblock = NULL;
  pindexPrev = NULL;

  if (pindexPrev != NULL && pindexPrev->nHeight != pindexBest->nHeight) {
    /* delete all worker blocks. */
    for (map<int, CBlock*>::const_iterator mi = mapWork.begin(); mi != mapWork.end(); ++mi)
    {
      CBlock *tblock = mi->second;
      delete tblock;
    }
    mapWork.clear();
  }

  // Store the pindexBest used before CreateNewBlock, to avoid races
  nTransactionsUpdatedLast = nTransactionsUpdated;
  CBlockIndex* pindexPrevNew = pindexBest;

  pblock = CreateNewBlock(reservekey);
  if (!pblock)
    return (NULL);

  // Need to update only after we know CreateNewBlock succeeded
  pindexPrev = pindexPrevNew;

  /* store "worker" block for until height increment. */
  work_id++;
  mapWork[work_id] = pblock; 

  // Update nTime
  pblock->UpdateTime(pindexPrev);
  pblock->nNonce = 0;

  Array transactions;
  //map<uint256, int64_t> setTxIndex;
  int i = 0;
  CTxDB txdb("r");
  BOOST_FOREACH (CTransaction& tx, pblock->vtx)
  {
    uint256 txHash = tx.GetHash();

    if (tx.IsCoinBase())
      continue;
    transactions.push_back(txHash.GetHex());
  }

  uint256 hashTarget = CBigNum().SetCompact(pblock->nBits).getuint256();

  Object result;

  /* all pool mining is defunc when "connections=0". */
  result.push_back(Pair("connections",   (int)vNodes.size()));

  result.push_back(Pair("version", pblock->nVersion));
  result.push_back(Pair("task", (int64_t)work_id));
  result.push_back(Pair("previousblockhash", pblock->hashPrevBlock.GetHex()));
  result.push_back(Pair("transactions", transactions));
  result.push_back(Pair("coinbasevalue", (int64_t)pblock->vtx[0].vout[0].nValue));
  result.push_back(Pair("target", hashTarget.GetHex()));
  result.push_back(Pair("sizelimit", (int64_t)MAX_BLOCK_SIZE));
  result.push_back(Pair("curtime", (int64_t)pblock->nTime));
  result.push_back(Pair("bits", HexBits(pblock->nBits)));

  if (!pindexPrev) {
    /* mining is defunct when "height < 2" */
    result.push_back(Pair("height", (int64_t)0));
  } else {
    result.push_back(Pair("height", (int64_t)(pindexPrev->nHeight+1)));
  }

  /* dummy nExtraNonce */
  SetExtraNonce(pblock, "f0000000f0000000");

  /* coinbase */
  CTransaction coinbaseTx = pblock->vtx[0];
  CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
  ssTx << coinbaseTx;
  result.push_back(Pair("coinbase", HexStr(ssTx.begin(), ssTx.end())));
  //  result.push_back(Pair("sigScript", HexStr(pblock->vtx[0].vin[0].scriptSig.begin(), pblock->vtx[0].vin[0].scriptSig.end())));
  result.push_back(Pair("coinbaseflags", HexStr(COINBASE_FLAGS.begin(), COINBASE_FLAGS.end())));

  blocktemplate_json = JSONRPCReply(result, Value::null, Value::null);
  return (blocktemplate_json.c_str());
}

int c_processblock(CBlock* pblock)
{
  CNode *pfrom = NULL;

  // Check for duplicate
  uint256 hash = pblock->GetHash();
  if (mapBlockIndex.count(hash))// || mapOrphanBlocks.count(hash))
    return (BLKERR_DUPLICATE_BLOCK);

  // Preliminary checks
  if (!pblock->CheckBlock()) {
fprintf(stderr, "DEBUG: c_processblock: !CheckBlock()\n");
    return (BLKERR_CHECKPOINT);
  }

  // Store to disk
  if (!pblock->AcceptBlock()) {
fprintf(stderr, "DEBUG: c_processblock: !AcceptBlock()\n");
    return (BLKERR_INVALID_BLOCK);
  }

  printf("ProcessBlock: ACCEPTED\n");

  return (0);
}

bool QuickCheckWork(CBlock* pblock)
{
  uint256 hash = pblock->GetPoWHash();
  uint256 hashTarget = CBigNum().SetCompact(pblock->nBits).getuint256();

        uint256 bhash = Hash(BEGIN(pblock->nVersion), END(pblock->nNonce));
fprintf(stderr, "DEBUG: QuickCheckWork: block hash \"%s\"\n", bhash.GetHex().c_str());
fprintf(stderr, "DEBUG: QuickCheckWork: block data \"%s\"\n", HexStr(BEGIN(pblock->nVersion), END(pblock->nNonce)).c_str());

  //// debug print
  fprintf(stderr, "BitcoinMiner:\n");
  fprintf(stderr, "proof-of-work found  \n  hash: %s (%s) \ntarget: %s\n", hash.GetHex().c_str(), HexStr(hash.begin(), hash.end()).c_str(), hashTarget.GetHex().c_str());
  pblock->print();
  fprintf(stderr, "generated %s\n", FormatMoney(pblock->vtx[0].vout[0].nValue).c_str());

  if (hash > hashTarget)
    return false;

  return true;
}

int c_submitblock(unsigned int workId, unsigned int nTime, unsigned int nNonce, char *xn_hex)
{
  CBlock *pblock;
int err;
bool ok;

  pblock = mapWork[workId];
  if (pblock == NULL)
    return (BLKERR_INVALID_JOB);

  pblock->nTime = nTime;
  pblock->nNonce = nNonce;

  /* set coinbase. */
  SetExtraNonce(pblock, xn_hex);

  /* generate merkle root */
  pblock->hashMerkleRoot = pblock->BuildMerkleTree();

#if 0
  fprintf(stderr, "DEBUG: submitblock: previousblockhash %s\n", pblock->hashPrevBlock.GetHex().c_str());
  fprintf(stderr, "DEBUG: submitblock: previousblockhash %s\n", HexStr(pblock->hashPrevBlock.begin(), pblock->hashPrevBlock.end()).c_str());
  CTransaction coinbaseTx = pblock->vtx[0];
  CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
  ssTx << coinbaseTx;
  fprintf(stderr, "DEBUG: submitblock: coinbase %s\n", HexStr(ssTx.begin(), ssTx.end()).c_str());
  fprintf(stderr, "DEBUG: sigScript: %s\n", HexStr(pblock->vtx[0].vin[0].scriptSig.begin(), pblock->vtx[0].vin[0].scriptSig.end()).c_str());
  fprintf(stderr, "DEBUG: submitblock: merkleroot %s\n", pblock->hashMerkleRoot.GetHex().c_str());
  fprintf(stderr, "DEBUG: submitblock: merkleroot %s\n", HexStr(pblock->hashMerkleRoot.begin(), pblock->hashMerkleRoot.end()).c_str());
  fprintf(stderr, "DEBUG: submitblock: hash %s\n", pblock->GetHash().GetHex().c_str());
  fprintf(stderr, "DEBUG: submitblock: target %s\n",  CBigNum().SetCompact(pblock->nBits).GetHex().c_str());
  fprintf(stderr, "DEBUG: submitblock: target diff %f\n", 
      (double)0x0000ffff / (double)(pblock->nBits & 0x00ffffff));
#endif

  ok = QuickCheckWork(pblock);
  if (!ok)
    return (BLKERR_LOW_DIFFICULTY);

  err = c_processblock(pblock);
  return (err);
}

Object c_AcentryToJSON(const CAccountingEntry& acentry, const string& strAccount, Object entry)
{
  bool fAllAccounts = (strAccount == string("*"));

  if (fAllAccounts || acentry.strAccount == strAccount)
  {
    entry.push_back(Pair("account", acentry.strAccount));
    entry.push_back(Pair("category", "move"));
    entry.push_back(Pair("time", (boost::int64_t)acentry.nTime));
    entry.push_back(Pair("amount", ValueFromAmount(acentry.nCreditDebit)));
    entry.push_back(Pair("otheraccount", acentry.strOtherAccount));
    entry.push_back(Pair("comment", acentry.strComment));
  }

  return (entry);
}

bool c_ListGenerateTransactions(const CWalletTx& wtx, Object entry)
{
  string strAccount = "*";
  int64 nGeneratedImmature, nGeneratedMature, nFee;
  string strSentAccount;
  list<pair<CTxDestination, int64> > listReceived;
  list<pair<CTxDestination, int64> > listSent;

  wtx.GetAmounts(nGeneratedImmature, nGeneratedMature, listReceived, listSent, nFee, strSentAccount);

  bool fAllAccounts = (strAccount == string("*"));

  // Generated blocks assigned to account ""
  //if ((nGeneratedMature+nGeneratedImmature) != 0) {
  if (nGeneratedMature) {
    entry.push_back(Pair("account", string("")));
    entry.push_back(Pair("category", "generate"));
    entry.push_back(Pair("amount", ValueFromAmount(nGeneratedMature)));
    WalletTxToJSON(wtx, entry);
    return (true);
  }

  return (false);
}

const char *c_getblocktransactions(void)
{

  string strAccount = "*";
  int nCount = 10;
  int nFrom = 0;


//  Array ret;
  CWalletDB walletdb(pwalletMain->strWalletFile);

  // First: get all CWalletTx and CAccountingEntry into a sorted-by-time multimap.
  typedef pair<CWalletTx*, CAccountingEntry*> TxPair;
  typedef multimap<int64, TxPair > TxItems;
  TxItems txByTime;

  // Note: maintaining indices in the database of (account,time) --> txid and (account, time) --> acentry
  // would make this much faster for applications that do this a lot.
  for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it)
  {
    CWalletTx* wtx = &((*it).second);
    txByTime.insert(make_pair(wtx->GetTxTime(), TxPair(wtx, (CAccountingEntry*)0)));
  }
  list<CAccountingEntry> acentries;
  walletdb.ListAccountCreditDebit(strAccount, acentries);
  BOOST_FOREACH(CAccountingEntry& entry, acentries)
  {
    txByTime.insert(make_pair(entry.nTime, TxPair((CWalletTx*)0, &entry)));
  }

  Object result;

  // iterate backwards until we have nCount items to return:
  for (TxItems::reverse_iterator it = txByTime.rbegin(); it != txByTime.rend(); ++it)
  {
    CWalletTx *const pwtx = (*it).second.first;
    if (pwtx != 0) {
      if (c_ListGenerateTransactions(*pwtx, result))
        break; /* found mature generation. */
    }
  }
  // ret is newest to oldest

  blocktemplate_json = JSONRPCReply(result, Value::null, Value::null);
  return (blocktemplate_json.c_str());
}

double c_GetNetworkHashRate(void)
{
  int lookup = 120;

  if (pindexBest == NULL)
    return 0;

  // If lookup is -1, then use blocks since last difficulty change.
  if (lookup <= 0)
    lookup = pindexBest->nHeight % 2016 + 1;

  // If lookup is larger than chain, then set it to chain length.
  if (lookup > pindexBest->nHeight)
    lookup = pindexBest->nHeight;

  CBlockIndex* pindexPrev = pindexBest;
  for (int i = 0; i < lookup; i++)
    pindexPrev = pindexPrev->pprev;

  double timeDiff = pindexBest->GetBlockTime() - pindexPrev->GetBlockTime();
  double timePerBlock = timeDiff / lookup;

  return ((double)GetDifficulty() * pow(2.0, 32)) / (double)timePerBlock;
}


const char *c_getmininginfo(void)
{
  Array result;

  result.push_back((int)nBestHeight);
  result.push_back((double)GetDifficulty());
  result.push_back((double)c_GetNetworkHashRate());

  mininginfo_json = JSONRPCReply(result, Value::null, Value::null);
  return (mininginfo_json.c_str());
}

string blockinfo_json;
const char *c_getblockinfo(const char *hash_addr)
{

  if (!hash_addr)
    return (NULL);

  std::string strHash(hash_addr);
  uint256 hash(strHash);

  if (mapBlockIndex.count(hash) == 0) {
//    throw JSONRPCError(-5, "Block not found");
    return (NULL);
  }

  CBlock block;
  CBlockIndex* pblockindex = mapBlockIndex[hash];
  block.ReadFromDisk(pblockindex, true);

  Object result;
  result.push_back(Pair("hash", block.GetHash().GetHex()));
  CMerkleTx txGen(block.vtx[0]);
  txGen.SetMerkleBranch(&block);
  result.push_back(Pair("confirmations", (int)txGen.GetDepthInMainChain()));
  result.push_back(Pair("size", (int)::GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION)));
  result.push_back(Pair("height", pblockindex->nHeight));
  result.push_back(Pair("version", block.nVersion));
  result.push_back(Pair("merkleroot", block.hashMerkleRoot.GetHex()));
  Array txs;
  BOOST_FOREACH(const CTransaction&tx, block.vtx)
    txs.push_back(tx.GetHash().GetHex());
  result.push_back(Pair("tx", txs));
  result.push_back(Pair("time", (boost::int64_t)block.GetBlockTime()));
  result.push_back(Pair("nonce", (boost::uint64_t)block.nNonce));
  result.push_back(Pair("bits", HexBits(block.nBits)));
  result.push_back(Pair("difficulty", GetDifficulty(pblockindex)));

  if (pblockindex->pprev)
    result.push_back(Pair("previousblockhash", pblockindex->pprev->GetBlockHash().GetHex()));
  if (pblockindex->pnext)
    result.push_back(Pair("nextblockhash", pblockindex->pnext->GetBlockHash().GetHex()));

  blockinfo_json = JSONRPCReply(result, Value::null, Value::null);
  return (blockinfo_json.c_str());
}

string transactioninfo_json;
const char *c_gettransactioninfo(const char *tx_id)
{

  if (!tx_id)
    return (NULL);

  std::string txStr(tx_id);
  uint256 hash;
  hash.SetHex(txStr);

  Object result;
  if (!pwalletMain->mapWallet.count(hash)) {
    //  throw JSONRPCError(-5, "Invalid or non-wallet transaction id");
    return (NULL);
  }
  const CWalletTx& wtx = pwalletMain->mapWallet[hash];

  int64 nCredit = wtx.GetCredit();
  int64 nDebit = wtx.GetDebit();
  int64 nNet = nCredit - nDebit;
  int64 nFee = (wtx.IsFromMe() ? wtx.GetValueOut() - nDebit : 0);

  result.push_back(Pair("amount", ValueFromAmount(nNet - nFee)));
  if (wtx.IsFromMe())
    result.push_back(Pair("fee", ValueFromAmount(nFee)));

  int confirms = wtx.GetDepthInMainChain();
  result.push_back(Pair("confirmations", confirms));
  if (confirms)
  {
    result.push_back(Pair("blockhash", wtx.hashBlock.GetHex()));
    result.push_back(Pair("blockindex", wtx.nIndex));
  }
  result.push_back(Pair("txid", wtx.GetHash().GetHex()));
  result.push_back(Pair("time", (boost::int64_t)wtx.GetTxTime()));
  BOOST_FOREACH(const PAIRTYPE(string,string)& item, wtx.mapValue)
    result.push_back(Pair(item.first, item.second));

  Array details;
  ListTransactions(wtx, "*", 0, false, details);
  result.push_back(Pair("details", details));

  transactioninfo_json = JSONRPCReply(result, Value::null, Value::null);
  return (transactioninfo_json.c_str());
}

string lastblockinfo_json;
const char *c_getlastblockinfo(const char *hash)
{
  CBlockIndex *pindex = NULL;
  int target_confirms = 1;

  if (hash) {
    std::string hashStr(hash);
    uint256 blockId = 0;

    blockId.SetHex(hashStr);
    pindex = CBlockLocator(blockId).GetBlockIndex();
  }

  int depth = pindex ? (1 + nBestHeight - pindex->nHeight) : -1;

  Array transactions;

  for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); it++)
  {
    CWalletTx tx = (*it).second;

    if (depth == -1 || tx.GetDepthInMainChain() < depth)
      ListTransactions(tx, "*", 0, true, transactions);
  }

  uint256 lastblock;

  if (target_confirms == 1)
  {
    lastblock = hashBestChain;
  }
  else
  {
    int target_height = pindexBest->nHeight + 1 - target_confirms;

    CBlockIndex *block;
    for (block = pindexBest;
        block && block->nHeight > target_height;
        block = block->pprev)  { }

    lastblock = block ? block->GetBlockHash() : 0;
  }

  lastblockinfo_json = JSONRPCReply(transactions, Value::null, Value::null);
  return (lastblockinfo_json.c_str());
}

string block_save_json;
bool WriteToShareNet(CBlock* pBlock, int nHeight)
{
  Object result;
  Array transactions;
  int err;

  result.push_back(Pair("version", pBlock->nVersion));
  result.push_back(Pair("height", (int64_t)nHeight));
  result.push_back(Pair("hash", pBlock->GetHash().ToString()));
  result.push_back(Pair("prevblock", pBlock->hashPrevBlock.GetHex()));
  result.push_back(Pair("merkleroot", pBlock->hashMerkleRoot.GetHex()));
  result.push_back(Pair("time", (int64_t)pBlock->nTime));
  result.push_back(Pair("bits", (int64_t)pBlock->nBits));
  result.push_back(Pair("nonce", (int64_t)pBlock->nNonce));

  //CTxDB txdb("r");
  BOOST_FOREACH (CTransaction& tx, pBlock->vtx)
  {
    uint256 txHash = tx.GetHash();

    Object entry;

    CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
    ssTx << tx;
    transactions.push_back(HexStr(ssTx.begin(), ssTx.end()));
  }
  result.push_back(Pair("transactions", transactions));

  block_save_json = JSONRPCReply(result, Value::null, Value::null);
  err = block_save(nHeight, block_save_json.c_str());
  if (err)
    return false;

  return true;
}



#ifdef __cplusplus
extern "C" {
#endif

const char *getblocktemplate(void)
{
  return (c_getblocktemplate());
}

int submitblock(unsigned int workId, unsigned int nTime, unsigned int nNonce, char *xn_hex)
{
  return (c_submitblock(workId, nTime, nNonce, xn_hex));
}

const char *getblocktransactions(void)
{
  return (c_getblocktransactions());
}

const char *getmininginfo(void)
{
  return (c_getmininginfo());
}

const char *getblockinfo(const char *hash)
{
  return (c_getblockinfo(hash));
}

const char *gettransactioninfo(const char *hash)
{
  return (c_gettransactioninfo(hash));
}

const char *getlastblockinfo(const char *hash)
{
  return (c_getlastblockinfo(hash));
}


#ifdef __cplusplus
}
#endif

