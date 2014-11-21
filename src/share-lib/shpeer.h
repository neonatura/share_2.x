
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

#define PEERF_VERBOSE (1 << 0)
#define PEERF_PUBLIC (1 << 1)
#define PEERF_PRIVATE (1 << 2)
#define PEERF_LOCAL (1 << 3)

struct shpeer_ipv4_t {
  uint16_t sin_family;
  uint16_t sin_port;
  struct shpeer_ipv4_addr_t {
    uint32_t s_addr;
  } sin_addr;
};
struct shpeer_ipv6_t {
  uint16_t sin6_family;
  uint16_t sin6_port;
  uint32_t sin6_flowinfo;
  struct shpeer_ipv6_addr_t {
    uint32_t net6_addr[4];
  } sin6_addr;
  uint32_t sin6_scope_id;  /* scope id (new in RFC2553) */
};

/**
 * A local or remote network address.
 */
typedef struct shpeer_t shpeer_t;

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
  union {
    struct shpeer_ipv4_t ip4;
    struct shpeer_ipv6_t ip6;
  } addr;

  /**
   * The network hardware address associated with the peer.
   */
  uint8_t hwaddr[6];

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

