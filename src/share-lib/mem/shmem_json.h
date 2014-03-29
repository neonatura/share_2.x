

/*
 * @copyright
 *
 *  Copyright 2011 Neo Natura
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

#ifndef __MEM__SHMEM_JSON_H__
#define __MEM__SHMEM_JSON_H__

/**
 * JSON text parser and generator.
 * @ingroup libshare_mem
 * @defgroup libshare_memjson JSON text parser and generator.
 * @{
 */






/* shjson_ Types: */
#define shjson_False 0
#define shjson_True 1
#define shjson_NULL 2
#define shjson_Number 3
#define shjson_String 4
#define shjson_Array 5
#define shjson_Object 6
	
#define shjson_IsReference 256

/* The shjson structure: */
typedef struct shjson_t {
	struct shjson_t *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct shjson_t *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==shjson_String */
	int valueint;				/* The item's number, if type==shjson_Number */
	double valuedouble;			/* The item's number, if type==shjson_Number */

	char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} shjson_t;


/**
 * Coverts a JSON string into a object format.
 * @see shjson_print
 */
shjson_t *shjson(char *json_str);

/**
 * @returns An allocated string in JSON format.
 * @see shjson
 */
char *shjson_print(shjson_t *json);



/**
 * @}
 */

#endif /* ndef __MEM__SHMEM_JSON_H__ */


