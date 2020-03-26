#include <lua.hpp>
#include <iostream>
#include <string>
#include <fstream>

std::fstream fs;

static int e24_init(lua_State *L) {
  std::string filename = luaL_checkstring(L, -1);
  fs.open(filename.c_str(), std::fstream::out);
  if(!fs.is_open()) {
    lua_pushinteger(L, 0);
    return 1;
  }

  lua_pushinteger(L, 1);
  return 1;
}

static int e24_read(lua_State *L) {
  
}

static int e24_close(lua_State *L) {
  fs.close();
}