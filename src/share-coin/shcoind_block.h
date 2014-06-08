
#define CURRENT_TX_VERSION 1
#define CURRENT_BLOCK_VERSION 4

typedef struct tx_in_t
{
  unsigned char prevout[32];
  unsigned char sig[16];
  unsigned int nSequence;
} tx_in_t;

typedef struct tx_out_t
{
  uint64_t nValue;
  /** @note only 33 bytes are used */
  unsigned char pubkey[64];
}

 
typedef struct tx_t
{
  int nVersion; /* CURRENT_TX_VERSION */
  unsigned int nTime;
  unsigned int nLockTime;
  unsigned int nTxIn;
  unsigned int nTxOut;
/*
  std::vector<CTxIn> vin;
  std::vector<CTxOut> vout;
*/
} tx_t;

typedef struct block_t
{
  /** CURRENT_BLOCK_VERSION */
  int nVersion;

  uint256 hashPrevBlock;
  uint256 hashMerkleRoot;
  unsigned int nTime;
  unsigned int nBits;
  unsigned int nNonce;
  char vchBlockSig[128];
  unsigned int nTx;

}


