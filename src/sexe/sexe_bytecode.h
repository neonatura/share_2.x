

#ifndef __SEXE__SEXE_BYTECODE_H__
#define __SEXE__SEXE_BYTECODE_H__

#include "lobject.h"
#include "lzio.h"
#include "lparser.h"


/* data to catch conversion errors */
#define LUAC_TAIL		"\x19\x93\r\n\x1a\n"

/* size in bytes of header of binary files */
#define LUAC_HEADERSIZE		(sizeof(LUA_SIGNATURE)-sizeof(char)+2+6+sizeof(LUAC_TAIL)-sizeof(char))


typedef struct DumpState {
 lua_State* L;
 lua_Writer writer;
 void* data;
 int strip;
 int status;
} DumpState;

struct SParser {  /* data to `f_parser' */
  ZIO *z;
  Mbuffer buff;  /* dynamic structure used by the scanner */
  Dyndata dyd;  /* dynamic structures used by the parser */
  const char *mode;
  const char *name;
};

/** Execute a protected parser.  */
static inline void checkmode (lua_State *L, const char *mode, const char *x) {
  if (mode && strchr(mode, x[0]) == NULL) {
    luaO_pushfstring(L, 
        "attempt to load a %s chunk (mode is " LUA_QS ")", x, mode);
    luaD_throw(L, LUA_ERRSYNTAX);
  }
}


/* load one chunk; from lundump.c */
LUAI_FUNC Proto* luaU_undump (lua_State* L, ZIO* Z, Mbuffer* buff, const char* name);

/* make header; from lundump.c */
LUAI_FUNC void luaU_header (lu_byte* h);

/* dump one chunk; from ldump.c */
LUAI_FUNC int luaU_dump (lua_State* L, const Proto* f, lua_Writer w, void* data, int strip);


void DumpBlock(const void* b, size_t size, DumpState* D);
void DumpString(const TString* s, DumpState* D);

Proto *sexe_load(lua_State* L, ZIO* Z, Mbuffer* buff, const char* name);

#endif /* ndef __SEXE__SEXE_BYTECODE_H__ */
