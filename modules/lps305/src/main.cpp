#include <iostream>
#include <cstring>
#include <lua5.2/lua.hpp>
#include "lps305.h"

static int L_help(lua_State *L){
  lua_pushstring(L, "This module is needed  to work with power supply LPS-305.\nIt contains next functions:\n-- lps305:connectNewDevice(path, baudrate) - The function of connecting to a given 'path' with rate a 'baudrate'. Returns the table for working with the device.\n-- obj:setOuput(mode) - performs enable/disable outputs device\n-- obj:getModel() - returns a string containing the model of the device\n-- obj:getVersion() - returns a string containing the version of the device\n-- obj:getDeviceHelp() - returns a string containing commands for working with the device\n-- obj:disconnect() - function to disconnect from the device.\n!'obj' is the variable to which the table was assigned using 'lps305: connectNewDevice'\n");
  return 1;
}

static int L_posSetVoltage(lua_State *L) {
  double volt = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, -1)) volt = lua_tonumber(L, -1);
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  lps->setVoltage(1, volt);
  
  return 0;
}

static int L_posGetVoltage(lua_State *L) {  
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  double res;
  res = lps->getVoltage(1);
  
  lua_pushnumber(L, res);
  return 1;
}

static int L_posSetCurrent(lua_State *L) {
  double curr = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, -1)) curr = lua_tonumber(L, -1);
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  lps->setCurrent(1, curr);
  
  return 1;
}

static int L_posGetCurrent(lua_State *L) {
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  double res;
  res = lps->getCurrent(1);
  
  lua_pushnumber(L, res);
  return 1;
}

static int L_negSetVoltage(lua_State *L) {
  double volt = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, -1)) volt = lua_tonumber(L, -1);
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  lps->setVoltage(2, volt);
  
  return 0;
}

static int L_negGetVoltage(lua_State *L) {  
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  double res;
  res = lps->getVoltage(2);
  
  lua_pushnumber(L, res);
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  lps->setOutput(mode);
  
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  char* res;
  res = lps->getVersion();
  
  lua_pushlstring(L, res, strlen(res));
  free(res);
  return 1;
}

static int L_getDeviceHelp(lua_State *L) {
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  char* res;
  res = lps->getHelp();
  
  lua_pushlstring(L, res, strlen(res));
  free(res);
  return 1;
}

static int L_disconnect(lua_State *L) {
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
    fprintf(stderr, "Call 'connectNewDevice' before using anything functions");
    return -1;
  }
  lps->close();

  delete lps; // Есть шанс что программа выпадет по segmentation fault.
              // Исправление: lps = nullptr;
  lua_pushlightuserdata(L, NULL);
  lua_setfield(L, 1, "_objLPS305_");

  return 1;
}

static int L_connectNewDevice(lua_State *L) {
  char *portname = NULL;
  int baud = 0, checkError = 0;
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, -2)) portname = (char*)lua_tostring(L, -2);
    if(lua_isnumber(L, -1)) baud = lua_tonumber(L, -1);
  } else {
    if(lua_isnumber(L, -1)) portname = (char*)lua_tostring(L, -2);
    baud = 2400;
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

    lua_pushstring(L, "pos");
    lua_newtable(L);
      lua_newtable(L);
        lua_pushstring(L, "_objLPS305_");
        lua_pushlightuserdata(L, lps);
        lua_rawset(L, -3);
      lua_setmetatable(L, -2);
      
      lua_pushstring(L, "setVoltage");
      lua_pushcfunction(L, L_posSetVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, L_posGetVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setCurrent");
      lua_pushcfunction(L, L_posSetCurrent);
      lua_rawset(L, -3);

      lua_pushstring(L, "getCurrent");
      lua_pushcfunction(L, L_posGetCurrent);
      lua_rawset(L, -3);

    lua_rawset(L, -3);

    lua_pushstring(L, "neg");
    lua_newtable(L);
      lua_newtable(L);
        lua_pushstring(L, "_objLPS305_");
        lua_pushlightuserdata(L, lps);
        lua_rawset(L, -3);
      lua_setmetatable(L, -2);
      
      lua_pushstring(L, "setVoltage");
      lua_pushcfunction(L, L_negSetVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, L_negGetVoltage);
      lua_rawset(L, -3);

    lua_rawset(L, -3);

    lua_pushstring(L, "setOutput");
    lua_pushcfunction(L, L_setOutput);
    lua_rawset(L, -3);

    lua_pushstring(L, "getModel");
    lua_pushcfunction(L, L_getModel);
    lua_rawset(L, -3);

    lua_pushstring(L, "getVersion");
    lua_pushcfunction(L, L_getVersion);
    lua_rawset(L, -3);

    lua_pushstring(L, "getDeviceHelp");
    lua_pushcfunction(L, L_getDeviceHelp);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "disconnect");
    lua_pushcfunction(L, L_disconnect);
    lua_rawset(L, -3);

  return 1;
}

extern "C" int luaopen_lps305(lua_State *L){
  lua_createtable(L, 0, 0);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
    
    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  return 1;
}