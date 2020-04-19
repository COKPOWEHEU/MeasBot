#include <iostream>
#include <cstring>
#include <lua5.2/lua.hpp>
#include "lps305.h"


static int L_help(lua_State *L){
  lua_pushstring(L, "This module is needed  to work with power supply LPS-305");
  return 1;
}

static int L_setVoltage(lua_State *L) {
  int channel = 0;
  double volt = 0;
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, -2)) channel = lua_tointeger(L, -2);
    if(lua_isnumber(L, -1)) volt = lua_tonumber(L, -1);
  }
  
  lua_getmetatable(L, -3);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }

  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  lps->setVoltage(channel, volt);
  
  return 1;
}

static int L_setCurrent(lua_State *L) {
  int channel = 0;
  double curr = 0;
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, -2)) channel = lua_tointeger(L, -2);
    if(lua_isnumber(L, -1)) curr = lua_tonumber(L, -1);
  }
  
  lua_getmetatable(L, -3);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }

  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  lps->setCurrent(channel, curr);
  
  return 1;
}

static int L_setOutput(lua_State *L) {
  int mode = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, -1)) mode = lua_tointeger(L, -1);
  }
  
  lua_getmetatable(L, -2);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }

  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  lps->setOutput(mode);
  
  return 1;
}

static int L_setModeTracking(lua_State *L) {
  int mode = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, -1)) mode = lua_tointeger(L, -1);
  }
  
  lua_getmetatable(L, -2);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }

  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  lps->setModeTracking(mode);
  
  return 1;
}

static int L_setBeeper(lua_State *L) {
  int mode = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, -1)) mode = lua_tointeger(L, -1);
  }
  
  lua_getmetatable(L, -2);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }

  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  lps->setBeeper(mode);
  
  return 1;
}

static int L_setDigOutput(lua_State *L) {
  int mode = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, -1)) mode = lua_tointeger(L, -1);
  }
  
  lua_getmetatable(L, -2);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }

  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  lps->setDigOutput(mode);
  
  return 1;
}

static int L_getVoltage(lua_State *L) {
  int channel = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, -1)) channel = lua_tointeger(L, -1);
  }
  
  lua_getmetatable(L, -2);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }

  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  double res;
  res = lps->getVoltage(channel);
  
  lua_pushnumber(L, res);
  return 1;
}

static int L_getCurrent(lua_State *L) {
  int channel = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, -1)) channel = lua_tointeger(L, -1);
  }
  
  lua_getmetatable(L, -2);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }

  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  double res;
  res = lps->getCurrent(channel);
  
  lua_pushnumber(L, res);
  return 1;
}

static int L_getStatus(lua_State *L) {
  lua_getmetatable(L, -1);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }

  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  int res;
  res = lps->getStatus();
  
  lua_pushinteger(L, res);
  return 1;
}

static int L_getModel(lua_State *L) {
  lua_getmetatable(L, -1);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }
  
  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  char* res;
  res = lps->getModel();
  
  lua_pushlstring(L, res, strlen(res));
  free(res);
  return 1;
}

static int L_getVersion(lua_State *L) {
  lua_getmetatable(L, -1);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }
  
  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  char* res;
  res = lps->getVersion();
  
  lua_pushlstring(L, res, strlen(res));
  free(res);
  return 1;
}

static int L_close(lua_State *L) {
  lua_getmetatable(L, -1);
  if(!lua_istable(L, -1)) {
    printf("Not metatable!\n");
    return 0;
  }
  
  lua_getfield(L, -1, "_objLPS305_");
  if(!lua_islightuserdata(L, -1)) {
    printf("Not userdata!\n");
    return 0;
  }
  LPS305 *lps  = (LPS305*)lua_touserdata(L, -1);
  if(lps == NULL) {
    fprintf(stderr, "Call 'connect' before using anything functions");
    return -1;
  }
  lps->close();
  
  delete lps; // Есть шанс что программа выпадет по segmentation fault.
              // Исправление lps = nullptr;
  lua_pushlightuserdata(L, NULL);
  lua_setfield(L, 1, "_objLPS305_");
  
  return 1;
}

static int L_connect(lua_State *L) {
  char *portname = NULL;
  int baud = 0, checkError = 0;
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, -2)) portname = (char*)lua_tostring(L, -2);
    if(lua_isnumber(L, -1)) baud = lua_tonumber(L, -1);
  }

  LPS305 *lps;
  try {
    lps = new LPS305;
  } catch (std::bad_alloc &ba) {
    std::cerr << ba.what() << std::endl;
    return 0;
  }

  checkError = lps->connect(portname, baud);
  
  if(checkError == 0) {
    std::cerr << "Port isn't open" << std::endl;
    delete lps;
    lua_pushnil(L);
    return 0;
  }
  
  lua_newtable(L);
    lua_newtable(L);
      lua_pushstring(L, "_objLPS305_");
      lua_pushlightuserdata(L, lps);
      lua_rawset(L, -3);
    lua_setmetatable(L, -2);
    
    lua_pushstring(L, "setVoltage");
    lua_pushcfunction(L, L_setVoltage);
    lua_rawset(L, -3);

    lua_pushstring(L, "setCurrent");
    lua_pushcfunction(L, L_setCurrent);
    lua_rawset(L, -3);

    lua_pushstring(L, "setOutput");
    lua_pushcfunction(L, L_setOutput);
    lua_rawset(L, -3);

    lua_pushstring(L, "setModeTracking");
    lua_pushcfunction(L, L_setModeTracking);
    lua_rawset(L, -3);

    lua_pushstring(L, "setBeeper");
    lua_pushcfunction(L, L_setBeeper);
    lua_rawset(L, -3);

    lua_pushstring(L, "setDigOutput");
    lua_pushcfunction(L, L_setDigOutput);
    lua_rawset(L, -3);

    lua_pushstring(L, "getVoltage");
    lua_pushcfunction(L, L_getVoltage);
    lua_rawset(L, -3);

    lua_pushstring(L, "getCurrent");
    lua_pushcfunction(L, L_getCurrent);
    lua_rawset(L, -3);

    lua_pushstring(L, "getStatus");
    lua_pushcfunction(L, L_getStatus);
    lua_rawset(L, -3);

    lua_pushstring(L, "getModel");
    lua_pushcfunction(L, L_getModel);
    lua_rawset(L, -3);

    lua_pushstring(L, "getVersion");
    lua_pushcfunction(L, L_getVersion);
    lua_rawset(L, -3);

    lua_pushstring(L, "close");
    lua_pushcfunction(L, L_close);
    lua_rawset(L, -3);

  return 1;
}

extern "C" int luaopen_lps305(lua_State *L){
  lua_createtable(L, 0, 0);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
    
    lua_pushcfunction(L, L_connect);
    lua_setfield(L, -2, "connect");
  return 1;
}