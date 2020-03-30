#include <lua5.3/lua.hpp>

static int L_help(lua_State *L) {
  lua_pushstring(L, "This module is needed  to work with a voltmeter e24");
  return 1;
}

extern "C" int luaopen_e24(lua_State *L) {
  lua_newtable(L);

    lua_pushstring(L, "help");
    lua_pushcfunction(L, L_help);
    lua_rawset(L, -3);
    return 1;
}