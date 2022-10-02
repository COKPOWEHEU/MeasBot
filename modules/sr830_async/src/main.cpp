#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include "ttym.h"

#define ERROR_LOG(message) printf("%s:%s:%d:%s\n", __FILE__, __func__, __LINE__, message)

typedef struct sr830_thread{
  
  
  
}sr830_thread_t;



static int L_help(lua_State *L) {
  const char helpstring[] = "SR830\n";
  lua_pushstring(L, helpstring);
  return 1;
}

static int L_connectNewDevice(lua_State *L) {
  lua_newtable(L);
  return 1;
}

int luaopen_sr830_async(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");

    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");

  return 1;
}
