
/*
 * INET	
 *    An protocol implementation for the IPv4 protocol suite for the 
 *    LINUX operating system. INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level. ESP
 *    was implemented by Brian Burrell.
 *
 *    The Encoded STREAM Protocol (ESP).
 *
 * Version:	$Id: esp_csum.c,v 2302.2 2002/03/29 17:20:42 root cut root $
 *
 * Author: Brian Burrell, <bburrell@el-i.net>
 *
 * Fixes:
 *
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

#define VER_MAJOR 1
#define VER_MINOR 1
#define VER_PATCH 2
#include <linux/config.h>
#if defined (CONFIG_ES_PROTOCOL) || defined (CONFIG_ES_PROTOCOL_MODULE) 

#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/mm.h>
#include <linux/config.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <net/inet_common.h>
#include <net/snmp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <net/enc.h>
#include <net/encver.h>
#include <net/icmp.h>
#include <net/route.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/enc.h>
#include <net/checksum.h>
#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
#include <net/flow.h>
#include <net/ipv6.h>
#include <net/transp_v6.h>
#include <net/ip6_route.h>
#endif

/* ESPOPT_FAST_CHECKSUM */
inline u_short esp_fast_csum(__u32 seed, char *data, int len)
{
  u_short csum;
  int i, l;

  /* Seed value is base for checksum. */
  csum = seed;
  l = len - 8;

  /* compute base checksum from data argument. */
  for (i = 0; i < l; i += 8) {
    csum += *((__u64 *)(data+i));
  }
  for (; i < len; i++) {
    csum += data[i];
  }

  return (htons(csum));
}

#if ESP_VERSION >= 2
/* ESPOPT_LONG_CHECKSUM */
__u64 esp_long_csum(__u32 seed, char *data, int len) /* ?EXPERIMENTAL? */
{
	__u64 ret_csum;
	__u32 csum_min, csum2_maj;
	char *ptr = (char *)&ret_csum;
	int i, l;

	/* seed value is base for checksum. */
	csum_min = seed;
	for (i = 0; i < len; i++) {
		csum1_min += data[i];
	}
	csum_min = htonl(csum_min);

	/* compute base checksum from data argument. */
	l = max(0, len - 16);
	csum_maj = seed;
	for (i = 0; i < l; i += 16) {
    csum_maj += *((__u64 *) (data + i)); /* skips every 8 bytes */
  }
	csum_maj = htonl(csum_maj);

	memcpy(ptr, &csum_min, 4);
	memcpy(ptr+4, &csum_maj, 4);

	return (ret_csum);
}
#endif

#ifdef MODULE

int init_module (void)
{
  printk (KERN_INFO "Registered ESP-%d Encode Module.\n", ESP_VERSION);

  return (0);
}

void cleanup_module (void)
{
  printk (KERN_INFO "Unregistered ESP-%d Encode Module.\n", ESP_VERSION);
}

#endif /* def MODULE */

#endif

