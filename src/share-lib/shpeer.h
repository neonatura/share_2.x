
/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/briburrell/share)
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
#define SHSK_PEER_LOCAL 0

/**
 * A remote IPv4 network destination.
 */
#define SHSK_PEER_IPV4 1

/**
 * A remote IPv6 network destination.
 */
#define SHSK_PEER_IPV6 2

/**
 * A IPv4 network destination on the sharenet VPN.
 */
#define SHSK_PEER_VPN_IPV4 3

/**
 * A IPv6 network destination on the sharenet VPN.
 */
#define SHSK_PEER_VPN_IPV6 4

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
   * A SHSK_PEER_XX type
   */
  int type;

  /**
   * A IP 4/6 network address
   */
  union {
    uint32_t ip;
    uint64_t ip6;
  } addr;
};

/**
 * @}
 */

#endif /* ndef __SHPEER_H__ */

