#include <lua5.2/lua.hpp>
#include "SR830.hpp"

static int L_help(lua_State *L) {
  const char helpstring[] = "Module for working with DSP Lock-In Amplifier SR830\n";
  lua_pushstring(L, helpstring);
  return 1;
}

static int L_connectNewDevice(lua_State *L) {

  return 1;
}

extern "C" int luaopen_sr830(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");

    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  
  return 1;
}
