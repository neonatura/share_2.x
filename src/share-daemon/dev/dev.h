
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura
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

#ifndef __CARD__DEV_H__
#define __CARD__DEV_H__

#include <libusb-1.0/libusb.h>


#define SHDEV_NONE 0
#define SHDEV_CARD 1


#define BASE_CARD_SIZE (sizeof(uint16_t) + sizeof(shtime_t) + sizeof(shkey_t))

#define CARDTYPE_VISA "VIS"
#define CARDTYPE_MASTER "MAS"
#define CARDTYPE_NEONATURA "NAT"
#define CARDTYPE_AUX "AUX"

/* card service code - first digit */
#define SHCARD_INTERNATIONAL (1 << 0) /* '1' / '2' */ 
#define SHCARD_NATIONAL (1 << 1) /* '5' / '6' */
#define SHCARD_CHIP (1 << 2) /* '2' / '6' */
/* card service code - second digit */
#define SHCARD_ONLINE (1 << 6) /* '2' */
#define SHCARD_ONLINE_BILATERAL (1 << 7) /* '4' */
/* card service code - third digit */
#define SHCARD_PIN (1 << 10) /* '0' */
#define SHCARD_SERVICE (1 << 11) /* '2' no cash / '5' pin */
#define SHCARD_ATM (1 << 12) /* '3' ATM */
#define SHCARD_CASH (1 << 13) /* '4' CASH */
#define SHCARD_PIN_PREF (1 << 14) /* '6' pin preferred / '7' +SERVICE */

/** Determines whether card flags have no service restrictions. */
#define IS_SHCARD_NO_RESTRICTION(_flags) \
  (!(_flags & SHCARD_PIN) && \
   !(_flags & SHCARD_SERVICE) && \
   !(_flags & SHCARD_ATM) && \
   !(_flags & SHCARD_CASH) && \
   !(_flags & SHCARD_PIN_PREF))


struct shcard_t
{
  /** A three character gateway identifier */
  char card_type[4]; 

  /** The card's 'service code' as a bitvector. */
  uint32_t card_flags;

  /** The card owner's account id. */
  uint64_t card_acc;

  /** The card's expiration date. */
  shtime_t card_expire;

  /** A key reference to the card's 12-19 digit ID number. */
  uint64_t card_id;

  /** Peer information for a 'NAT' type card's issuer. */
  shpeer_t card_issuer;
};
typedef struct shcard_t shcard_t;



struct shdev_t {
  /* driver */
  libusb_device_handle *handle;
  int iface;
  int index;

  /* content */
  int type;
  union {
    shcard_t card;
  } data;

  /* io stream */
  shbuf_t *buff;

  struct shdev_t *next;
};
typedef struct shdev_t shdev_t;



#include "card_kmap.h"
#include "card_csc.h"


#endif /* ndef __CARD__DEV_H__ */
