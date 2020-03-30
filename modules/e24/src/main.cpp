#include <lua5.3/lua.hpp>
#include <fstream>

static int L_help(lua_State *L) {
  lua_pushstring(L, "This module is needed  to work with a voltmeter e24");
  return 1;
}

static int L_ReadADC(lua_State *L) {
  int nchannel = luaL_checkinteger(L, -1);
  double voltage;

  char path[22];
  sprintf(path, "/opt/dev/e24/adc%i.adc", nchannel);

  std::fstream fs (path, std::fstream::in);
  if(!fs.is_open()) {
    lua_pushnil(L);
    return 1;
  }

  fs >> voltage;
  fs.close();

  lua_pushnumber(L, voltage);
  return 1;
}

extern "C" int luaopen_e24(lua_State *L) {
  lua_newtable(L);

    lua_pushstring(L, "help");
    lua_pushcfunction(L, L_help);
    lua_rawset(L, -3);

    lua_pushstring(L, "readADC");
    lua_pushcfunction(L, L_ReadADC);
    lua_rawset(L, -3);

    return 1;
}