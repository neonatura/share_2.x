

/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */

#include <stddef.h>

#include "sexe.h"
#include "lua.h"
#include "lobject.h"
#include "lstate.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstring.h"
#include "lapi.h"
#include "sexe_compile.h"

int run_flags;


void error(LoadState* S, const char* why)
{
 luaO_pushfstring(S->L,"%s: %s precompiled chunk",S->name,why);
 luaD_throw(S->L,LUA_ERRSYNTAX);
}

#define LoadMem(S,b,n,size)	LoadBlock(S,b,(n)*(size))
#define LoadByte(S)		(lu_byte)LoadChar(S)
#define LoadVar(S,x)		LoadMem(S,&x,1,sizeof(x))
#define LoadVector(S,b,n,size)	LoadMem(S,b,n,size)

#if !defined(luai_verifycode)
#define luai_verifycode(L,b,f)	(f)
#endif


#define DumpMem(b,n,size,D)	DumpBlock(b,(n)*(size),D)
#define DumpVar(x,D)		DumpMem(&x,1,sizeof(x),D)

void DumpBlock(const void* b, size_t size, DumpState* D)
{
 if (D->status==0)
 {
  lua_unlock(D->L);
  D->status=(*D->writer)(D->L,b,size,D->data);
  lua_lock(D->L);
 }
}

void DumpChar(int y, DumpState* D)
{
 char x=(char)y;
 DumpVar(x,D);
}

void DumpInt(int x, DumpState* D)
{
 DumpVar(x,D);
}

void DumpNumber(lua_Number x, DumpState* D)
{
 DumpVar(x,D);
}

void DumpVector(const void* b, int n, size_t size, DumpState* D)
{
 DumpInt(n,D);
 DumpMem(b,n,size,D);
}

void DumpString(const TString* s, DumpState* D)
{
 if (s==NULL)
 {
  size_t size=0;
  DumpVar(size,D);
 }
 else
 {
  size_t size=s->tsv.len+1;		/* include trailing '\0' */
  DumpVar(size,D);
  DumpBlock(getstr(s),size*sizeof(char),D);
 }
}

#define DumpCode(f,D)	 DumpVector(f->code,f->sizecode,sizeof(Instruction),D)

void DumpFunction(const Proto* f, DumpState* D);

void DumpConstants(const Proto* f, DumpState* D)
{
 int i,n=f->sizek;
 DumpInt(n,D);
 for (i=0; i<n; i++)
 {
  const TValue* o=&f->k[i];
  DumpChar(ttype(o),D);
  switch (ttype(o))
  {
   case LUA_TNIL:
	break;
   case LUA_TBOOLEAN:
	DumpChar(bvalue(o),D);
	break;
   case LUA_TNUMBER:
	DumpNumber(nvalue(o),D);
	break;
   case LUA_TSTRING:
	DumpString(rawtsvalue(o),D);
	break;
  }
 }
 n=f->sizep;
 DumpInt(n,D);
 for (i=0; i<n; i++) DumpFunction(f->p[i],D);
}

void DumpUpvalues(const Proto* f, DumpState* D)
{
 int i,n=f->sizeupvalues;
 DumpInt(n,D);
 for (i=0; i<n; i++)
 {
  DumpChar(f->upvalues[i].instack,D);
  DumpChar(f->upvalues[i].idx,D);
 }
}

void DumpDebug(const Proto* f, DumpState* D)
{
 int i,n;
 DumpString((D->strip) ? NULL : f->source,D);
 n= (D->strip) ? 0 : f->sizelineinfo;
 DumpVector(f->lineinfo,n,sizeof(int),D);
 n= (D->strip) ? 0 : f->sizelocvars;
 DumpInt(n,D);
 for (i=0; i<n; i++)
 {
  DumpString(f->locvars[i].varname,D);
  DumpInt(f->locvars[i].startpc,D);
  DumpInt(f->locvars[i].endpc,D);
 }
 n= (D->strip) ? 0 : f->sizeupvalues;
 DumpInt(n,D);
 for (i=0; i<n; i++) DumpString(f->upvalues[i].name,D);
}

void DumpFunction(const Proto* f, DumpState* D)
{
 DumpInt(f->linedefined,D);
 DumpInt(f->lastlinedefined,D);
 DumpChar(f->numparams,D);
 DumpChar(f->is_vararg,D);
 DumpChar(f->maxstacksize,D);
 DumpCode(f,D);
 DumpConstants(f,D);
 DumpUpvalues(f,D);
 DumpDebug(f,D);
}


#if 0
/*
** dump Lua function as precompiled chunk
*/
int luaU_dump (lua_State* L, const Proto* f, lua_Writer w, void* data, int strip)
{
 DumpState D;
 D.L=L;
 D.writer=w;
 D.data=data;
 D.strip=strip;
 D.status=0;
 SexeDumpHeader(&D);
 DumpFunction(f,&D);
 return D.status;
}
#endif
/*
** $Id: lundump.c,v 1.71 2011/12/07 10:39:12 lhf Exp $
** load precompiled Lua chunks
** See Copyright Notice in lua.h
*/


void LoadBlock(LoadState* S, void* b, size_t size)
{
 if (luaZ_read(S->Z,b,size)!=0) error(S,"truncated");
}

int LoadChar(LoadState* S)
{
 char x;
 LoadVar(S,x);
 return x;
}

int LoadInt(LoadState* S)
{
 int x;
 LoadVar(S,x);
 if (x<0) error(S,"corrupted");
 return x;
}

lua_Number LoadNumber(LoadState* S)
{
 lua_Number x;
 LoadVar(S,x);
 return x;
}

TString* LoadString(LoadState* S)
{
 size_t size;
 LoadVar(S,size);
 if (size==0)
  return NULL;
 else
 {
  char* s=luaZ_openspace(S->L,S->b,size);
  LoadBlock(S,s,size*sizeof(char));
  return luaS_newlstr(S->L,s,size-1);		/* remove trailing '\0' */
 }
}

void SexeLoadCode(LoadState* S, Proto* f, sexe_stack_t *stack)
{
  size_t len;
  int n;

  f->code=luaM_newvector(S->L, stack->size, Instruction);
  f->sizecode = stack->size;
  LoadBlock(S, f->code, stack->size * sizeof(Instruction));

  VERBOSE("[CODE] x%d instrs @ %d bytes each <%d bytes>\n", stack->size, sizeof(Instruction), sizeof(sexe_stack_t) + (stack->size * sizeof(Instruction)));

}

Proto* SexeLoadFunction(LoadState* S);

void SexeLoadConstants(LoadState* S, Proto* f, sexe_stack_t *stack)
{
  char *lit;
  char **lits;
  char *s_ptr, *e_ptr;
  int tot;
  int idx;
  int i;

  lit = NULL;
  lits = NULL;
  if (stack->type == SESTACK_LITERAL) {
    /* literals */
    lit = (char *)calloc(stack->size + 1, sizeof(char));
    LoadBlock(S, lit, stack->size);

    tot = 0;
    for (i = 0; i < stack->size; i++) {
      if (lit[i] == '\0')
        tot++;
    }
    VERBOSE("[CONSTANT] loaded x%d literal strings <%d bytes>\n", tot, sizeof(stack) + stack->size);

    lits = (char **)calloc(tot + 1, sizeof(char *));

    idx = 0;
    s_ptr = lit;
    for (i = 0; i < stack->size; i++) {
      if (lit[i] == '\0') {
        lits[idx] = s_ptr;
        VERBOSE("[CONSTANT] lits[%d] = '%s'\n", idx, lits[idx]);

        idx++;
        s_ptr = &lit[i+1];
      }
    }
  }

  /* constants */
  memset(stack, 0, sizeof(sexe_stack_t));
  LoadBlock(S, stack, sizeof(sexe_stack_t));
  VERBOSE("[CONSTANT] loading x%d constants\n", stack->size);
  f->k=luaM_newvector(S->L, stack->size, TValue);
  f->sizek = stack->size;
  for (i=0; i < stack->size; i++) setnilvalue(&f->k[i]);
  for (i=0; i < stack->size; i++) {
    TValue* o=&f->k[i];
    sexe_const_t con;
    int t;

    memset(&con, 0, sizeof(con));
    LoadBlock(S, &con, sizeof(con));


    switch (con.con_type)
    {
      case LUA_TNIL:
        setnilvalue(o);
        break;
      case LUA_TBOOLEAN:
        setbvalue(o, (unsigned char)con.con_val);
        break;
      case LUA_TNUMBER:
        setnvalue(o, (lua_Number)con.con_val);
        break;
      case LUA_TSTRING:
        if (lits && con.con_val < tot) {
          char *raw = lits[con.con_val];
          size_t str_len = strlen(raw); 
          char *s = luaZ_openspace(S->L, S->b, str_len);
          memcpy(s, raw, str_len);
          setsvalue2n(S->, o, luaS_newlstr(S->L, s, str_len));
        }
        //setsvalue2n(S->L, o, lits[con.con_val]);
        break;
    }
  }

  if (lits) free(lits);
  if (lit) free(lit);


}

void SexeLoadUpvalues(LoadState* S, Proto* f, sexe_stack_t *stack)
{
  sexe_upval_t upv;
  int i;

  f->upvalues=luaM_newvector(S->L, stack->size, Upvaldesc);
  f->sizeupvalues = stack->size;
  for (i=0; i < stack->size; i++)
    f->upvalues[i].name=NULL;

  for (i=0; i < stack->size; i++) {
    memset(&upv, 0, sizeof(upv));
    LoadBlock(S, &upv, sizeof(upv));
    f->upvalues[i].instack = (unsigned char)upv.upv_instack;
    f->upvalues[i].idx = (unsigned char)upv.upv_index;
  }

}

void SexeLoadLocalVarDebug(LoadState* S, Proto* f, sexe_stack_t *stack)
{
  sexe_debug_lvar_t lvar;
  int i;

  f->sizelocvars = stack->size;
  f->locvars=luaM_newvector(S->L, stack->size, LocVar);
  for (i = 0; i < stack->size; i++) 
    f->locvars[i].varname=NULL;
  for (i = 0; i < stack->size; i++) {
    char *s;

    memset(&lvar, 0, sizeof(lvar));
    LoadBlock(S, &lvar, sizeof(lvar));

    s = luaZ_openspace(S->L, S->b, strlen(lvar.lvar_name) + 1);
    strcpy(s, lvar.lvar_name);
    f->locvars[i].varname = luaS_newlstr(S->L, s, strlen(s));		
    f->locvars[i].startpc = lvar.lvar_startpc;
    f->locvars[i].endpc = lvar.lvar_endpc;
  }

  VERBOSE("[DEBUG] loaded x%d local var debug entries <%d bytes>\n", stack->size, sizeof(stack) + (stack->size * sizeof(sexe_debug_lvar_t)));
}

void SexeLoadCodeDebug(LoadState* S, Proto* f, sexe_stack_t *stack)
{
  uint32_t linfo;
  int i;

  f->lineinfo = luaM_newvector(S->L, stack->size, int);
  f->sizelineinfo = stack->size;
  for (i = 0; i < stack->size; i++) {
    LoadBlock(S, &linfo, sizeof(linfo)); 
    f->lineinfo[i] = linfo;
  }

  VERBOSE("[DEBUG] loaded x%d local var debug <%d bytes>\n", stack->size, sizeof(sexe_stack_t) + (stack->size * sizeof(uint32_t)));

}

void SexeLoadUpvalDebug(LoadState* S, Proto* f, sexe_stack_t *stack)
{
}

static void SexeLoadFunctions(LoadState *S, Proto *f, sexe_stack_t *stack)
{ 
  int tot = stack->size; /* copy due to static context */
  int i;

  /* inner functions */
  f->p=luaM_newvector(S->L, tot, Proto*);
  f->sizep = tot;
  for (i=0; i < tot; i++) 
    f->p[i]=NULL;
  for (i=0; i < tot; i++) 
    f->p[i]=SexeLoadFunction(S);
}

Proto* SexeLoadFunction(LoadState* S)
{
  Proto* f=luaF_newproto(S->L);
  sexe_stack_t stack;
  sexe_func_t func;

  setptvalue2s(S->L,S->L->top,f); incr_top(S->L);
  strncpy(f->mod_name, S->name, sizeof(f->mod_name) - 1);

  LoadBlock(S, &func, sizeof(func));
  if (func.func_source) {
    char *s = luaZ_openspace(S->L,S->b, 13);
    sprintf(s, "#%x", (unsigned int)func.func_source); 
    f->source = luaS_newlstr(S->L, s, strlen(s));
  }
  f->numparams = func.param_max;
  if (func.param_flag & SEPARAMF_VARARG)
    f->is_vararg = TRUE; 
  f->maxstacksize = func.stack_max;
  VERBOSE("[FUNCTION] source #%x\n", func.func_source);

  memset(&stack, 0, sizeof(stack));
  stack.type = -1;
  while (stack.type != SESTACK_NONE) {
    memset(&stack, 0, sizeof(stack));
    LoadBlock(S, &stack, sizeof(stack));
    VERBOSE("[STACK] type:%d size:%d\n", stack.type, stack.size);
    switch (stack.type) {
      case SESTACK_INSTRUCTION:
        SexeLoadCode(S, f, &stack);
        break;
      case SESTACK_INSTRUCTION_DEBUG:
        SexeLoadCodeDebug(S, f, &stack);
        break;
      case SESTACK_LITERAL:
      case SESTACK_CONSTANT:
        SexeLoadConstants(S, f, &stack);
        break;
      case SESTACK_FUNCTION:
        SexeLoadFunctions(S, f, &stack);
        break;
      case SESTACK_UPVAL:
        SexeLoadUpvalues(S, f, &stack);
        break;
      case SESTACK_UPVAL_DEBUG:
        SexeLoadUpvalDebug(S, f, &stack);
        break;
      case SESTACK_LOCALVAR_DEBUG:
        SexeLoadLocalVarDebug(S, f, &stack); 
        break;
    }
  }

  S->L->top--;
  return f;
}


void SexeLoadBlock(LoadState* S, void* b, size_t size)
{
 if (luaZ_read(S->Z,b,size)!=0) error(S,"truncated");
}

void SexeLoadHeader(LoadState* S)
{
//  lu_byte h[SEXE_HEADERSIZE];
  static lu_byte s[SEXE_HEADERSIZE];
  sexe_mod_t *lhdr;

//  sexe_header(h, name);
  s[0] = SEXE_SIGNATURE[0];
  SexeLoadBlock(S,s+1,SEXE_HEADERSIZE-1);
  lhdr = (sexe_mod_t *)s;

  if (0 != memcmp(s, SEXE_SIGNATURE, strlen(SEXE_SIGNATURE)))
    return;

/*
  if (lhdr->ver != h.ver)
    error(S,"version mismatch in");
*/

  if (lhdr->fmt != 0)
    error(S,"incompatible"); 
//else error(S,"corrupted");

  if (0 != memcmp(s + SEXE_HEADERSIZE - strlen(SEXE_TAIL), SEXE_TAIL, strlen(SEXE_TAIL))) {
    error(S, "corrupt");
  }

  if (*lhdr->name)
    S->name = lhdr->name;
 
  VERBOSE("[HEADER] '%s' v%d <%d bytes>\n", lhdr->name, lhdr->ver, SEXE_HEADERSIZE);
}


#define MYINT(s)	(s[0]-'0')
#define VERSION		MYINT(LUA_VERSION_MAJOR)*16+MYINT(LUA_VERSION_MINOR)
#define FORMAT		0		/* this is the official format */




static const char *getS (lua_State *L, void *ud, size_t *size) {
  LoadS *ls = (LoadS *)ud;
  (void)L;  /* not used */
  if (ls->size == 0) return NULL;
  *size = ls->size;
  ls->size = 0;
  return ls->s;
}
#if 0
int sexe_load_file(lua_State *S, char *path)
{
  unsigned char *data;
  size_t data_len;
  int err;

  err = shfs_read_mem(path, &data, &data_len);
  if (err)
    return (err);

  err = sexe_loadbuffer(S, data, data_len, shfs_app_name(path));
  if (data) free(data);
  if (err)
    return (err);

  return (err);
}
#endif

static const char *getF (lua_State *L, void *ud, size_t *size) {
  LoadF *lf = (LoadF *)ud;
  (void)L;  /* not used */
  if (lf->n > 0) {  /* are there pre-read characters to be read? */
    *size = lf->n;  /* return them (chars already in buffer) */
    lf->n = 0;  /* no more pre-read characters */
  }
  else {  /* read a block from file */
    /* 'fread' can return > 0 *and* set the EOF flag. If next call to
       'getF' called 'fread', it might still wait for user input.
       The next check avoids this problem. */
    if (feof(lf->f)) return NULL;
    *size = fread(lf->buff, 1, sizeof(lf->buff), lf->f);  /* read block */
  }
  return lf->buff;
}

static int errfile (lua_State *L, const char *what, int fnameindex) {
  const char *serr = strerror(errno);
  const char *filename = lua_tostring(L, fnameindex) + 1;
  lua_pushfstring(L, "cannot %s %s: %s", what, filename, serr);
  lua_remove(L, fnameindex);
  return LUA_ERRFILE;
}

static int skipBOM (LoadF *lf) {
  const char *p = "\xEF\xBB\xBF";  /* Utf8 BOM mark */
  int c;
  lf->n = 0;
  do {
    c = getc(lf->f);
    if (c == EOF || c != *(unsigned char *)p++) return c;
    lf->buff[lf->n++] = c;  /* to be read by the parser */
  } while (*p != '\0');
  lf->n = 0;  /* prefix matched; discard it */
  return getc(lf->f);  /* return next character */
}
static int skipcomment (LoadF *lf, int *cp) {
  int c = *cp = skipBOM(lf);
  if (c == '#') {  /* first line is a comment (Unix exec. file)? */
    while ((c = getc(lf->f)) != EOF && c != '\n') ;  /* skip first line */
    *cp = getc(lf->f);  /* skip end-of-line */
    return 1;  /* there was a comment */
  }
  else return 0;  /* no comment */
}

int sexe_loadfile(lua_State *L, const char *filename, const char *mode) 
{
  LoadF lf;
  int status, readstatus;          
  int c;
  int fnameindex = lua_gettop(L) + 1;  /* index of filename on the stack */
  if (filename == NULL) {
    lua_pushliteral(L, "=stdin");
    lf.f = stdin;
  }
  else {
    lua_pushfstring(L, "@%s", filename);          
    lf.f = fopen(filename, "r");
    if (lf.f == NULL) return errfile(L, "open", fnameindex);
  }
  if (skipcomment(&lf, &c))  /* read initial portion */
    lf.buff[lf.n++] = '\n';  /* add line to correct line numbers */
  if (c == SEXE_SIGNATURE[0] && filename) {  /* binary file? */
    lf.f = freopen(filename, "rb", lf.f);  /* reopen in binary mode */
    if (lf.f == NULL) return errfile(L, "reopen", fnameindex);
    skipcomment(&lf, &c);  /* re-read initial portion */
  }
  if (c != EOF)
    lf.buff[lf.n++] = c;  /* 'c' is the first character of the stream */
  status = sexe_load(L, getF, &lf, lua_tostring(L, -1), mode);
  readstatus = ferror(lf.f);
  if (filename) fclose(lf.f);  /* close file (even in case of errors) */
  if (readstatus) {
    lua_settop(L, fnameindex);  /* ignore results from `lua_load' */
    return errfile(L, "read", fnameindex);
  }
  lua_remove(L, fnameindex);
  return status;
}


static void f_parser (lua_State *L, void *ud) 
{
  int i;
  Proto *tf;
  Closure *cl;
  struct SParser *p = cast(struct SParser *, ud);
  int c = zgetc(p->z);  /* read first character */
  if (c == SEXE_SIGNATURE[0]) {
    checkmode(L, p->mode, "binary");
    tf = sexe_undump(L, p->z, &p->buff, p->name);
  }
  else {
    checkmode(L, p->mode, "text");
    tf = luaY_parser(L, p->z, &p->buff, &p->dyd, p->name, c);
  }
  setptvalue2s(L, L->top, tf);
  incr_top(L);
  cl = luaF_newLclosure(L, tf);
  setclLvalue(L, L->top - 1, cl);
  for (i = 0; i < tf->sizeupvalues; i++)  /* initialize upvalues */
    cl->l.upvals[i] = luaF_newupval(L);
}

int sexe_parser(lua_State *L, ZIO *z, const char *name, const char *mode) 
{
  struct SParser p;
  int status;

  memset(&p, 0, sizeof(p));

  L->nny++;  /* cannot yield during parsing */
  p.z = z; p.name = name; p.mode = mode;
  p.dyd.actvar.arr = NULL; p.dyd.actvar.size = 0;
  p.dyd.gt.arr = NULL; p.dyd.gt.size = 0;
  p.dyd.label.arr = NULL; p.dyd.label.size = 0;
  luaZ_initbuffer(L, &p.buff);
  status = luaD_pcall(L, f_parser, &p, savestack(L, L->top), L->errfunc);
  luaZ_freebuffer(L, &p.buff);
  luaM_freearray(L, p.dyd.actvar.arr, p.dyd.actvar.size);
  luaM_freearray(L, p.dyd.gt.arr, p.dyd.gt.size);
  luaM_freearray(L, p.dyd.label.arr, p.dyd.label.size);
  L->nny--;
  return status;
}

int sexe_load(lua_State *L, lua_Reader reader, void *data, const char *chunkname, const char *mode) 
{
  ZIO z; 
  int status;
  lua_lock(L);
  if (!chunkname) chunkname = "?";
  luaZ_init(L, &z, reader, data);
  status = sexe_parser(L, &z, chunkname, mode);
  if (status == LUA_OK) {  /* no errors? */
    LClosure *f = clLvalue(L->top - 1);  /* get newly created function */
    if (f->nupvalues == 1) {  /* does it have one upvalue? */
      /* get global table from registry */
      Table *reg = hvalue(&G(L)->l_registry);
      const TValue *gt = luaH_getint(reg, LUA_RIDX_GLOBALS);
      /* set global table as 1st upvalue of 'f' (may be LUA_ENV) */
      setobj(L, f->upvals[0]->v, gt);
      luaC_barrier(L, f->upvals[0], gt);
    }
  }
  lua_unlock(L);
  return status;
}

Proto *sexe_undump (lua_State* L, ZIO* Z, Mbuffer* buff, const char* name)
{
  LoadState S;

  if (*name=='@' || *name=='=')
    S.name=name+1;
  else if (*name==SEXE_SIGNATURE[0])
    S.name="binary string";
  else
    S.name=name;
  S.L=L;
  S.Z=Z;
  S.b=buff;
  SexeLoadHeader(&S);
  return luai_verifycode(L,buff,SexeLoadFunction(&S));
}



#define UPVALNAME(x) ((f->upvalues[x].name) ? getstr(f->upvalues[x].name) : "-")
#define MYK(x)		(-1-(x))

void SexeDumpHeader(const Proto *f, DumpState *D)
{
  lu_byte h[SEXE_HEADERSIZE];
  sexe_header(h, f->mod_name);
  DumpBlock(h,SEXE_HEADERSIZE,D);
}

static void SexeDumpConstants(const Proto* f, DumpState* D)
{
  sexe_stack_t stack;
  sexe_const_t con;
  shbuf_t *buff;
  char *str;
  int str_idx;
  int i,n=f->sizek;

  /* literal strings */
  memset(&stack, 0, sizeof(stack));
  stack.type = SESTACK_LITERAL;
  buff = shbuf_init();
  for (i=0; i<n; i++) {
    const TValue* o=&f->k[i];
    if (ttype(o) != LUA_TSTRING)
      continue;
//    DumpString(rawtsvalue(o),D);
    str = getstr(rawtsvalue(o)); 
    shbuf_cat(buff, str, strlen(str) + 1);
    VERBOSE("[CONSTANT] literal #%d +%d bytes: '%s'\n", i, strlen(str)+1, str);
  }
  stack.size = shbuf_size(buff);
  DumpBlock(&stack, sizeof(stack), D);
  DumpBlock(shbuf_data(buff), shbuf_size(buff), D);
  VERBOSE("[CONSTANT] saved literal strings <%d bytes>\n", sizeof(stack) + shbuf_size(buff));
  shbuf_free(&buff);

  /* constants */
  str_idx = 0;
  memset(&stack, 0, sizeof(stack));
  stack.type = SESTACK_CONSTANT;
  stack.size = f->sizek;
  DumpBlock(&stack, sizeof(stack), D);
  VERBOSE("[CONSTANT] saved constants def: x%d\n", stack.size);
  for (i=0; i < f->sizek; i++) {
    const TValue* o=&f->k[i];

    memset(&con, 0, sizeof(con));
    con.con_type = ttype(o);
    switch (con.con_type) {
      case LUA_TNIL:
        break;
      case LUA_TBOOLEAN:
        con.con_val = (uint64_t)bvalue(o); 
        break;
      case LUA_TNUMBER:
        con.con_val = (uint64_t)nvalue(o);
        break;
      case LUA_TSTRING:
        con.con_val = (uint64_t)str_idx;
        str_idx++;
        break;
    }
    DumpBlock(&con, sizeof(con), D);
    VERBOSE("[CONSTANT] saved constants #%d\n", i);
  }

}

static int SexeDumpUpvalues(const Proto* f, DumpState* D)
{
  sexe_stack_t *stack;
  sexe_upval_t *upv;
  int len;
  int i;

  len = sizeof(sexe_stack_t) + (f->sizeupvalues * sizeof(sexe_upval_t));
  stack = (sexe_stack_t *)calloc(1, len);
  if (!stack)
    return (SHERR_NOMEM);

  stack->type = SESTACK_UPVAL;
  stack->size = f->sizeupvalues; 
  upv = (sexe_upval_t *)stack->stack.upv;
  for (i = 0; i < f->sizeupvalues; i++) {
    upv[i].upv_instack = f->upvalues[i].instack;
    upv[i].upv_index = f->upvalues[i].idx;
  }
  DumpBlock(stack, len, D);

  free(stack);

  return (0);
}

static int SexeDumpCode(const Proto *f, DumpState *D)
{
  sexe_stack_t *stack;
  Instruction *instr;
  int len;
  int i;

  len = sizeof(sexe_stack_t) + (f->sizecode * sizeof(Instruction));
  stack = (sexe_stack_t *)calloc(1, len);
  if (!stack)
    return (SHERR_NOMEM);

  stack->type = SESTACK_INSTRUCTION;
  stack->size = f->sizecode;

  instr = (Instruction *)stack->stack.instr;
  for (i = 0; i < f->sizecode; i++)
    instr[i] = f->code[i]; 
  DumpBlock(stack, len, D);
  VERBOSE("[CODE] dumped x%d instructions <%d bytes>\n", stack->size, len);

  free(stack);

  return (0);
}

void SexeDumpLocalVarDebug(const Proto* f, DumpState* D)
{
  sexe_stack_t stack;
  sexe_debug_lvar_t lvar;
  int i;

  memset(&stack, 0, sizeof(stack));
  stack.type = SESTACK_LOCALVAR_DEBUG;
  stack.size = f->sizelocvars;
  DumpBlock(&stack, sizeof(stack), D);

  for (i = 0; i < f->sizelocvars; i++) {
    memset(&lvar, 0, sizeof(lvar));
    strncpy(lvar.lvar_name, getstr(f->locvars[i].varname), sizeof(lvar.lvar_name) - 1);
    lvar.lvar_startpc = f->locvars[i].startpc;
    lvar.lvar_endpc = f->locvars[i].endpc;
    DumpBlock(&lvar, sizeof(lvar), D);
  }

  VERBOSE("[DEBUG] wrote x%d local variable debug entries <%d bytes>\n", stack.size, sizeof(stack) + (stack.size * sizeof(lvar)));
}

static void SexeDumpFunctions(const Proto *f, DumpState *D)
{
  sexe_stack_t stack;
  sexe_stack_t tstack;
  int i;

  memset(&stack, 0, sizeof(stack));
  stack.type = SESTACK_FUNCTION;
  stack.size = f->sizep;
  DumpBlock(&stack, sizeof(stack), D);

  for (i = 0; i < stack.size; i++) {
    SexeDumpFunction(f->p[i],D);

  }

}

void SexeDumpFunction(const Proto* f, DumpState* D)
{
  sexe_stack_t tstack;
  sexe_stack_t stack;
  sexe_func_t func;
  int i;

  /* primary fuction info */
  memset(&func, 0, sizeof(func));
  if (f->source) {
    char *str = getstr(f->source);
    func.func_source = (uint32_t)shcrc(str, strlen(str));
  }
  func.func_line = f->linedefined;
  func.func_lline = f->lastlinedefined;
  if (f->is_vararg)
    func.param_flag |= SEPARAMF_VARARG;
  func.param_max = f->numparams;
  func.stack_max = f->maxstacksize;
  DumpBlock(&func, sizeof(func), D);
  VERBOSE("[FUNCTION] initialized : source #%x\n", func.func_source);

  SexeDumpCode(f, D);
  SexeDumpConstants(f, D);
  SexeDumpFunctions(f, D);
  SexeDumpUpvalues(f,D);
#if 0
  if (!(run_flags & RUNF_STRIP))
    SexeDumpLocalVarDebug(f,D);
#endif

  /* terminator */ 
  memset(&tstack, 0, sizeof(tstack));
  DumpBlock(&tstack, sizeof(tstack), D);

}

int sexe_bin_write(lua_State* L, const Proto* f, lua_Writer w, void* data, int strip)
{
  sexe_stack_t tstack;
  DumpState D;

  memset(&D, 0, sizeof(D));
  D.L = L;
  D.writer = w;
  D.data = data;
  D.strip = strip;
  D.status = 0;
  SexeDumpHeader(f, &D);
  SexeDumpFunction(f, &D);


  return (0);
}

int sexe_dump (lua_State *L, lua_Writer writer, void *data) 
{
  int status;
  TValue *o;
  lua_lock(L);
  api_checknelems(L, 1);
  o = L->top - 1;
  if (isLfunction(o))
    status = sexe_bin_write(L, getproto(o), writer, data, 0);
  else
    status = 1;
  lua_unlock(L);
  return status;
}

