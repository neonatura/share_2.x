
/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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

#ifndef __SHPEER_H__
#define __SHPEER_H__

/**
 * @addtogroup libshare
 * @{
 */

/**
 * The local machine.
 */
#define SHNET_PEER_LOCAL 0

/**
 * A remote IPv4 network destination.
 */
#define SHNET_PEER_IPV4 1

/**
 * A remote IPv6 network destination.
 */
#define SHNET_PEER_IPV6 2

/**
 * A IPv4 network destination on the sharenet VPN.
 */
#define SHNET_PEER_VPN_IPV4 3

/**
 * A IPv6 network destination on the sharenet VPN.
 */
#define SHNET_PEER_VPN_IPV6 4

/**
 * Global network destination.
 */
#define SHNET_BROADCAST 5

/**
 * A local or remote network address.
 */
typedef struct shpeer_t shpeer_t;

/**
 * The local or remote machine commonly associated with a sharefs partition.
 * @manonly
 * See the libshare_net.3 API man page for ESP protocol network operations.
 * @endmanonly
 * @note Addresses are stored in network byte order.
 */
struct shpeer_t {

  /**
   * A IP 4/6 network address
   */
  union {
    uint32_t ip;
    uint64_t ip6;
  } addr;

  /**
   * The network hardware address associated with the peer.
   */
  uint8_t hwaddr[6];

  /**
   * A label identifying a perspective view of the peer.
   */
  char label[16];

  /**
   * The client user ID that is associated with the peer.
   */
  uint32_t uid;

  /**
   * A SHNET_PEER_XX type
   */
  uint32_t type;

  shkey_t name;
};

/**
 * Generates a peer reference to the local user for IPv4.
 * @returns Information relevant to identifying a peer host.
 */
shpeer_t *shpeer(void);

/**
 * Generates a peer reference to the public user for IPv4.
 * @returns Information relevant to identifying a public peer host.
 */
shpeer_t *shpeer_pub(void);

/**
 * Generates a peer reference that is unique per app name.
 */
shpeer_t *shpeer_app(char *app);

/**
 * @}
 */

#endif /* ndef __SHPEER_H__ */

