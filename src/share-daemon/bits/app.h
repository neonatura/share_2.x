

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
 *
 *  @file app.h
 */

#ifndef __BITS__APP_H__
#define __BITS__APP_H__

int generate_app(tx_app_t *app, tx_t *tx, tx_id_t *id);

int generate_app_tx(tx_app_t *app, shpeer_t *peer);

int confirm_app(tx_app_t *app);

tx_app_t *init_app(shpeer_t *peer);


int txop_app_confirm(shpeer_t *cli_peer, tx_app_t *app);

int txop_app_init(shpeer_t *cli_peer, tx_app_t *app);


#endif /* ndef __BITS__APP_H__ */

