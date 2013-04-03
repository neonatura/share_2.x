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
 *
 *  @file share.h 
 *  @brief The Share Library
 *  Provides: Hashmap suitable for high-volume memory sharing, disk storage and network transfer.
 *  Used By: Client programs.
 *
 */  

#ifndef __META__SHMETA_VALUE_H__
#define __META__SHMETA_VALUE_H__


/**
 * @defgroup sh_meta_value A meta definition value.
 * @brief A meta definition value is comprised of a data segment using a @c shmeta_value_t as a header. 
 * The @c shmeta_value_t specifies the meta definition's data segment. 
 * No @c shmeta_value variables need to be set if the hashmap is only going to reside in memory.
 * Providing the @c shmeta_value_t.sz is set appropriately then no other variables need to be filled to generate raw hashmap entries.
 * @{
 */
 
/**
 * The base of a version 1 shmeta hashmap entry value.
 */
struct shmeta_value_v1_t 
{

  /**
   * The hard-coded value @c SHMETA_VALUE_MAGIC set to ensure validity.
   * @note Used for recycling pools of objects and to ensure network integrity.
   */ 
  uint32_t magic;

  /**
   * An adler32 checksum of this @c shmeta_value_t hashmap value header and the data segment following.
   */
  uint32_t crc;

  /**
   * A unix-time timestamp referencing when this hashmap value was generated.
   */
  uint32_t stamp;

  /**
   * A unique index number directly related to this @c shmeta_value_t hashmap value.
   */
  uint32_t gl;

  /**
   * A adler32 reference to a function pertaining to this hashmap value.
   */ 
  uint32_t func;

  /**
   * A paramater format which describes the data segment following the @c shmeta_value_t header.
   */
  uint32_t pf;  

  /**
   * A sequence number in an order of operations.
   */
  uint32_t seq;

  /**
   * adler32 reference to the name of this value.
   */
  uint32_t name;

  /**
   * The total size of data segment with a @c shmeta_value_t header 
   */
  uint64_t sz;  

  /**
   * blind reference to additional an data segment. 
   * */
  char raw[0];
};

/**
 * Specifies a reference to the current version of a shmeta hashmap entry value.
 */
typedef struct shmeta_value_v1_t shmeta_value_t;

/**
 * Creates a @c shmeta_value_t hashmap value from a string.
 * @param str The string to generated into a hashmap value.
 * @seealso shmeta_set
 */
char *shmeta_str(char *str);

/**
 * @}
 */


#endif /* ndef __META__SHMETA_VALUE_H__ */


