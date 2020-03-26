#include <lua.hpp>
#include <iostream>
#include <string>
#include <fstream>

std::fstream fs;

static int e24_init(lua_State *L) {
  std::string filename = luaL_checkstring(L, -1);
  fs.open(filename.c_str(), std::fstream::in);
  if(!fs.is_open()) {
    lua_pushinteger(L, 0);
    return 1;
  }

  lua_pushinteger(L, 1);
  return 1;
}

static int e24_read(lua_State *L) {
  if(!fs.is_open()) {
    lua_pushinteger(L, 0);
    return 1;
  }

  double a, b;
  static double prevB = 0;
  fs >> a >> b;
  if(prevB+0.5 < b) {
    lua_pushnumber(L, a);
    lua_pushnumber(L, prevB);
    return 2;
  }

  prevB = b;
  lua_pushnumber(L, a);
  lua_pushnumber(L, b);
  return 2;
}

static int e24_close(lua_State *L) {
  fs.close();
}