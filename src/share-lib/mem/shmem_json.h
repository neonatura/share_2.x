

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


#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131E-16
#endif
#ifndef INT_MAX
#define INT_MAX 0x7FFF/0x7FFFFFFF
#endif
#ifndef INT_MIN
#define INT_MIN ((int) 0x8000/0x80000000)
#endif





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
/** next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct shjson_t *next,*prev;	

		/** An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
	struct shjson_t *child;

			/** The type of the item, as above. */
	int type;		

		/** The item's string, if type==shjson_String */
	char *valuestring;	
			/** The item's number, if type==shjson_Number */
	int valueint;	
		/** The item's number, if type==shjson_Number */
	double valuedouble;	

	/** The item's name string, if this item is the child of, or is in the list of subitems of an object. */
	char *string;			
} shjson_t;



/**
 * Obtain a text representation of the @c json hiearchy in JSON format.
 * @returns An allocated string in JSON format.
 * @see shjson_init()
 */
char *shjson_print(shjson_t *json);

/**
 * Obtain an allocated string value from a JSON object.
 * @param tree The JSON object containing the string value.
 * @param name The name of the string JSON node.
 * @param def_str The default string value if the JSON node does not exist.
 * @returns The string value contained in the JSON node.
 */
char *shjson_str(shjson_t *json, char *name, char *def_str);

/**
 * Obtain an un-allocated string value from a JSON object.
 * @param tree The JSON object containing the string value.
 * @param name The name of the string JSON node.
 * @param def_str The default string value if the JSON node does not exist.
 * @returns The string value contained in the JSON node.
 */
char *shjson_astr(shjson_t *json, char *name, char *def_str);

/**
 * Add a string value to a JSON object or array.
 * @param tree The JSON object containing the string value.
 * @param name The name of the string JSON node.
 * @param val The string value to store in the new JSON node.
 * @returns The new JSON node containing the string value.
 */
shjson_t *shjson_str_add(shjson_t *tree, char *name, char *val);

/**
 * De-allocates memory associated with a JSON hiearchy.
 * @param tree_p A reference to the JSON hierarchy.
 * @see shjson_init()
 */
void shjson_free(shjson_t **tree_p);

/**
 * Obtain a number value from a JSON object.
 * @param tree The JSON object containing the number value.
 * @param name The name of the number JSON node.
 * @param def_d The default number value if the JSON node does not exist.
 * @returns The number value contained in the JSON node.
 */
double shjson_num(shjson_t *json, char *name, double def_d);

/**
 * Add a number value to a JSON object or array.
 * @param tree The JSON object or array to add the number value.
 * @param name The name of the number JSON node or NULL if @c tree is an array.
 * @param idx The number value to store in the new JSON node.
 * @returns The new JSON node containing the number value.
 */
shjson_t *shjson_num_add(shjson_t *tree, char *name, double num);

/**
 * Create a new JSON tree hierarchy.
 * @param json_str A JSON formatted text string or NULL.
 * @returns A new JSON object if @c json_str is null or a full JSON node hierarchy otherwise.
 * @see shjson_print
 * @see shjson_free
 */
shjson_t *shjson_init(char *json_str);

/**
 * Create a new JSON node at the end of an array.
 * @param json The JSON object containing the array.
 * @param name The name of the array in the JSON object.
 * @returns A new JSON node attachd to the array.
 */
shjson_t *shjson_array_add(shjson_t *tree, char *name);

/**
 * Obtain an allocated string value from an array.
 * @param json The JSON object containing the array.
 * @param name The name of the array in the JSON object.
 * @param idx The index number of the JSON node in the array.
 * @returns The string value contained in the array's node index.
 * @note The string pointer returned must be de-allocated.
 */
char *shjson_array_str(shjson_t *json, char *name, int idx);

/**
 * Obtain an un-allocated string value from an array.
 * @param json The JSON object containing the array.
 * @param name The name of the array in the JSON object.
 * @param idx The index number of the JSON node in the array.
 * @returns The string value contained in the array's node index.
 * @note Do not free the string pointer returned.
 */
char *shjson_array_astr(shjson_t *json, char *name, int idx);

/**
 * Obtain a number value from an array.
 * @param json The JSON object containing the array.
 * @param name The name of the array in the JSON object.
 * @param idx The index number of the JSON node in the array.
 * @returns The numeric value contained in the array's node index.
 */
double shjson_array_num(shjson_t *json, char *name, int idx);

/**
 * @returns A JSON object contained inside another object.
 */
shjson_t *shjson_obj(shjson_t *json, char *name);

/**
 * @returns The string length of a JSON object node.
 */
size_t shjson_strlen(shjson_t *json, char *name);


/**
 * @}
 */

#endif /* ndef __MEM__SHMEM_JSON_H__ */


