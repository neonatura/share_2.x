
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
 */

#ifndef __SOCKET__SHSK_H__ 
#define __SOCKET__SHSK_H__

/**
 * The libshare Socket Handling provides access to regular socket operations with posix or convienence functions in addition to access to the ESP network protocol.
 * The ESP protocol performs streaming compression and provides more security than TCP.
 * @brief The libshare network socket operations. 
 * @ingroup libshare
 * @defgroup libshare_socket Network socket operations.
 * @{
 */

#include "socket.h"
#include "connect.h"
#include "gethost.h"
#include "shfcntl.h"
#include "shsk_write.h"

/**
 * @}
 */

#endif /* ndef __SOCKET__SHSK_H__ */


