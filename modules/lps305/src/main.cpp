#include <lua5.2/lua.hpp>

static int L_help(lua_State *L){
  lua_pushstring(L, "This module is needed  to work with power supply LPS-305");
  return 1;
}

extern "C" int luaopen_lps305(lua_State *L){
  lua_createtable(L, 0, 0);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
  return 1;
}