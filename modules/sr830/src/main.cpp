#include <iostream>
#include <lua5.2/lua.hpp>
#include "SR830.hpp"

static int L_help(lua_State *L) {
  const char helpstring[] =
      "SR830\n"
      "  help():string - return help string\n"
      "  connectNewDevice(path, [baudrate]):table - connect to SR830. The baud rate from 300 to 19200 baud (Default is 9600)\n"
      ;
  lua_pushstring(L, helpstring);
  return 1;
}

static int L_setRS232(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setRS232();

  return 0;
}

static int L_clrRegs(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->clrRegs();

  return 0;
}

static int L_reset(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->reset();

  return 0;
}

static int L_readID(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  char* res;
  res = sr->readID();

  lua_pushstring(L, res);
  return 1;
}

static int L_setPermStatReg (lua_State *L) {
  int rese = 0, rliae = 0, rerre = 0, rsre = 0;
  if(lua_gettop(L) >= 5) {
    if(lua_isnumber(L, 2)) rese = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3)) rliae = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4)) rerre = lua_tonumber(L, 4);
    if(lua_isnumber(L, 5)) rsre = lua_tonumber(L, 5);
  } else {
    ERROR_LOG("This function must contain at least 4 parameters");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setPermStatRegs(rese, rliae, rerre, rsre);
  
  return 0;
}

/*Template
static int name (lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
}
*/


static int L_disconnect(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->close();

  delete sr;
  lua_pushlightuserdata(L, nullptr);
  lua_setfield(L, 1, "_objSR830_");

  return 1;
}

static int L_connectNewDevice(lua_State *L) {
  char *portname = nullptr;
  int baud = 9600, checkError = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isstring(L, 2)) portname = (char*)lua_tostring(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (port name)");
    return 0;
  }
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, 3)) baud = lua_tonumber(L, 3);
  }

  SR830 *sr;
  try {
    sr = new SR830;
  } catch (std::bad_alloc &ba) {
    ERROR_LOG(ba.what());
    return 0;
  }

  checkError = sr->connect(portname, baud);

  if(checkError == 0) {
    ERROR_LOG("Port isn't open");
    delete sr;
    return 0;
  }

  lua_newtable(L);
    lua_newtable(L);
      lua_pushstring(L, "_objSR830_");
      lua_pushlightuserdata(L, sr);
      lua_rawset(L, -3);
      
      lua_pushstring(L, "_gcc");
      lua_pushcfunction(L, L_disconnect);
      lua_rawset(L, -3);
    lua_setmetatable(L, -2);

    lua_pushstring(L, "setRS232");
    lua_pushcfunction(L, L_setRS232);
    lua_rawset(L, -3);

    lua_pushstring(L, "clrRegs");
    lua_pushcfunction(L, L_clrRegs);
    lua_rawset(L, -3);

    lua_pushstring(L, "setPermStatusRegs");
    lua_pushcfunction(L, L_setPermStatReg);
    lua_rawset(L, -3);

    lua_pushstring(L, "reset");
    lua_pushcfunction(L, L_reset);
    lua_rawset(L, -3);

    lua_pushstring(L, "readID");
    lua_pushcfunction(L, L_readID);
    lua_rawset(L, -3);

    /*Template
    lua_pushstring(L, "name");
    lua_pushcfunction(L, name);
    lua_rawset(L, -3);
    */



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
