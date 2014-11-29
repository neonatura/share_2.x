
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
 * Application group.
 */
#define SHNET_GROUP 6

#define PEERF_PRIVATE (1 << 2)
#define PEERF_LOCAL (1 << 3)
#define PEERF_GROUP (1 << 4)

#define SHARCH_32BIT (1 << 0)
#define SHARCH_LINUX (1 << 1)
#define SHARCH_WIN (1 << 2)
#define SHARCH_MAC (1 << 3)
#define SHARCH_BSD (1 << 4)
#define SHARCH_SUN (1 << 5)


struct shpeer_addr {
  /** The definition AF_INET or AF_INET6. */
  uint16_t sin_family;
  /** The network byte order socket port. */
  uint16_t sin_port;
  /** The ipv4/ipv6 socket address. */
  uint32_t sin_addr[4];
  /** The ethernet hardware address associated with the socket peer.  */
  uint8_t hwaddr[6];
};
typedef struct shpeer_addr shpeer_addr_t;

/**
 * A local or remote reference to a libshare runtime enabled application.
 * @manonly
 * See the libshare_net.3 API man page for ESP protocol network operations.
 * @endmanonly
 * @note Addresses are stored in network byte order.
 */
struct shpeer_t {

  /**
   * A IP 4/6 network address
   */
  shpeer_addr_t addr;

  /**
   * The local process id.
   */
  uint16_t pid;

  /**
   * A label identifying a perspective view of the peer.
   */
  char label[16];

  /**
   * The client user ID that is associated with the peer.
   */
  uint32_t uid;

  /**
   * Architecture of local machine for private key generation.
   */
  uint32_t arch;

  /**
   * A SHNET_PEER_XX type
   */
  uint32_t type;

  /**
   * behaviour control
   */
  uint32_t flags;

  /**
   * A key reference to the peer identify.
   */
  shkey_t name;
};

/**
 * A local or remote network address.
 */
typedef struct shpeer_t shpeer_t;



void shpeer_set_default(shpeer_t *peer);

/**
 * Generates a peer reference to the local user for IPv4.
 * @returns Information relevant to identifying a peer host.
 */
shpeer_t *shpeer(void);


shpeer_t *ashpeer(void);

/**
 * Generates a peer reference to the public user for IPv4.
 * @returns Information relevant to identifying a public peer host.
 */
//shpeer_t *shpeer_pub(void);

/**
 * Generates a peer reference that is unique per app name.
 */
//shpeer_t *shpeer_app(char *app);

/**
 * Generate a peer-to-peer connection to a specific host.
 * @note The information accessed by this peer is unique per user id.
 */ 
//shpeer_t *shpeer_host(char *hostname);

void shpeer_free(shpeer_t **peer_p);

shpeer_t *shpeer_init(char *appname, char *hostname, int flags);

/**
 * @}
 */

#endif /* ndef __SHPEER_H__ */

