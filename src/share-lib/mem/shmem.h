
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

#ifndef __MEM__SHMEM_H__
#define __MEM__SHMEM_H__


/**
 * Memory manipulation routines.
 * @ingroup libshare
 * @defgroup libshare_mem Encryption, memory pools, and hashmaps.
 * @{
 */


/**
 * A hard-coded value stored in memory segments in order to validate a current state or integrity.
 * @note Specifies a hard-coded value that identifies a @c shmeta_value_t data segment.
 * @see shmeta_value_t shencode()
 */
#define SHMEM_MAGIC 0x87654321

/**
 * The byte padding size when allocating a stored value.
 * @see shencode() shmeta_set()
 */
#define SHMEM_PAD_SIZE 32 

#include "shmem_key.h"
#include "shmem_buf.h"
#include "shmem_pool.h"
#include "shmem_meta.h"
#include "shmem_crypt.h"
#include "shmem_lock.h"

/**
 * @}
 */

#endif /* ndef __MEM__SHMEM_H__ */

