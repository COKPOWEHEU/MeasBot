#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>

static int L_help(lua_State *L){
  const char helpstring[] = "Здесь могла быть ваша справка";
  lua_pushstring(L, helpstring);
  return 1;
}


static int L_connectNewDevice(lua_State *L) {
  
  return 1;
}

int luaopen_sr570(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");

    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  
  return 1;
}
