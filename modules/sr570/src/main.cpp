#include <iostream>
#include <lua5.2/lua.hpp>
#include "SR570.hpp"

static int L_help(lua_State *L){
  static const std::string helpstring =
                    "Module for working with low noise current preamplifier SR570\n"
                    ;
  lua_pushstring(L, helpstring.c_str());
  return 1;
}

extern "C" int luaopen_sr570(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
  
  return 1;
}