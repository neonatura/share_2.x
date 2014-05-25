
#ifndef __SHCGBD_CLIENTVERSION_H__
#define __SHCGBD_CLIENTVERSION_H__

#include "shcgbd_clientversion.h"
#include <string.h>

/** Client Version */
static const int CLIENT_VERSION =
                           1000000 * CLIENT_VERSION_MAJOR
                         +   10000 * CLIENT_VERSION_MINOR
                         +     100 * CLIENT_VERSION_REVISION
                         +       1 * CLIENT_VERSION_BUILD;


/** Network Protocol Version */
static const int PROTOCOL_VERSION = 60006;

// earlier versions not supported as of Feb 2012, and are disconnected
static const int MIN_PROTO_VERSION = 209;

// nTime field added to CAddress, starting with this version;
// if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 31402;

// only request blocks from nodes outside this range of versions
static const int NOBLKS_VERSION_START = 60002;
static const int NOBLKS_VERSION_END = 60004;

// BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 60000;

// "mempool" command, enhanced "getdata" behavior starts with this version:
static const int MEMPOOL_GD_VERSION = 60002;

#define DISPLAY_VERSION_MAJOR       1
#define DISPLAY_VERSION_MINOR       1
#define DISPLAY_VERSION_REVISION    6
#define DISPLAY_VERSION_BUILD       7

#endif /* ndef __SHCGBD_CLIENTVERSION_H__ */
