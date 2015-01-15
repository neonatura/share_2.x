
#include "sexe.h"

static void sexe_table_field_set(lua_State *L, shjson_t *json)
{
  shjson_t *node;

  for (node = json; node; node = node->next) {
    switch (node->type) {
      case shjson_False:
      case shjson_True:
        lua_pushstring(L, node->string); /* token */
        lua_pushboolean(L, node->type); /* value */
        lua_settable(L, -3);
        break;
      case shjson_Number:
        lua_pushstring(L, node->string); /* token */
        lua_pushnumber(L, node->valuedouble); /* value */
        lua_settable(L, -3);
        break;
      case shjson_String:
        lua_pushstring(L, node->string); /* token */
        lua_pushstring(L, node->valuestring); /* value */
        lua_settable(L, -3);
        break;
#if 0
      case shjson_Array:
        cnt = 0;
        for (node = json->child; node; node = node->next) {
          if (node->type == shjson_False || node->type == shjson_True ||
              node->type == shjson_Number || node->type == shjson_String)
            cnt++;
        }
        if (cnt == 0)
          return; /* all done */

        lua_createtable(L, cnt, 0);
        cnt = 0;
        for (node = json->child; node; node = node->next) {
          switch (node->type) {
            case shjson_False:
            case shjson_True:
              luah_pushboolean(L, node->type);
              lua_rawseti(L, -2, cnt++); 
              break;
            case shjson_Number:
              lua_pushnumber(L, node->valuedouble);
              lua_rawseti(L, -2, cnt++); 
              break;
            case shjson_String:
              lua_pushstring(L, node->valuestring);
              lua_rawseti(L, -2, cnt++);
              break;
          }
        }
        break;
#endif
      case shjson_Object:
        lua_pushstring(L, node->string); /* token */
        lua_newtable(L);
        sexe_table_field_set(L, node->child);
        lua_settable(L, -3);
        break;
    }
  }

}

/**
 * Set a json hierarchy as a table on the lua stack.
 */
void sexe_table_set(lua_State *L, shjson_t *json)
{
  
  lua_newtable(L);
  sexe_table_field_set(L, json->child);

}

static void sexe_table_field_get(lua_State *L, shjson_t *json)
{
  shjson_t *obj;
  const char *k, *v;
  char *e_ptr;
  double d;

  lua_pushnil(L);
  while (lua_next(L, -2)) {
    if (lua_istable(L, -1)) {
      obj = shjson_CreateObject();
      sexe_table_field_get(L, obj);
      lua_pop(L, 1);
      k = lua_tostring(L, -1);
      shjson_AddItemToObject(json, k, obj);
      continue;
    }

    v = NULL;
    d = 0;
    if (lua_isnumber(L, -1)) {
      d = lua_tonumber(L, -1);
    } else if (lua_isstring(L, -1)) {
      v = lua_tostring(L, -1);
    }

    lua_pop(L, 1);
    k = lua_tostring(L, -1);

    if (v) {
      shjson_str_add(json, k, v); /* string */  
    } else if (d) {
      shjson_num_add(json, k, d); /* number */  
    }
  }

}

/**
 * Retrieve a lua table on the lua stack as a json hierarchy.
 */
shjson_t *sexe_table_get(lua_State *L)
{
  shjson_t *json;
  const char *k, *v;
  char *e_ptr;
  double d;

  json = shjson_init(NULL);
  sexe_table_field_get(L, json);

  return (json);
}

