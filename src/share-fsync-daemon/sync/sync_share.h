

/*
 *  Copyright 2014 Neo Natura 
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

#ifndef __SYNC__SHARE_H__
#define __SYNC__SHARE_H__


int share_sync_init(sync_t *scan, void *unused);

int share_sync_term(sync_t *sync, void *unused);

int share_sync_watch(sync_t *sync, const char *path);

int share_sync_poll(sync_t *sync, double *to_p);

int share_sync_remove(sync_t *sync, sync_ent_t *ent);

int share_sync_read(sync_t *sync, void *unused);

int share_sync_write(sync_t *sync, void *unused);


#endif /* ndef __SYNC__SHARE_H__ */


