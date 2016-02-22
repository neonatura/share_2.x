
/*
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
 */  

#ifndef __BITS__SESSION_H__
#define __BITS__SESSION_H__


int local_session_generate(tx_id_t *id, shtime_t sess_stamp, tx_session_t **sess_p);


int txop_session_init(shpeer_t *cli_peer, tx_session_t *sess);
int txop_session_confirm(shpeer_t *cli_peer, tx_session_t *sess);
int txop_session_send(shpeer_t *cli_peer, tx_session_t *sess);
int txop_session_recv(shpeer_t *cli_peer, tx_session_t *sess);


#endif /* ndef __BITS__SESSION_H__ */
