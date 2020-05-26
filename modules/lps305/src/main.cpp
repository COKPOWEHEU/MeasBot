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

static int L_negSetCurrent(lua_State *L) {
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
  lps->setCurrent(2, curr);
  
  return 1;
}

static int L_negGetCurrent(lua_State *L) {
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
  res = lps->getCurrent(2);
  
  lua_pushnumber(L, res);
  return 1;
}

static int L_bothSetVoltage(lua_State *L) {
  double volt = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) volt = lua_tonumber(L, 2);
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
  
  volt /= 2;
  lps->setVoltage(1, volt);
  lps->setVoltage(2, volt);
  
  return 0;
}

static int L_bothGetVoltage(lua_State *L) {  
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
  res = lps->getVoltage(1) + lps->getVoltage(2);
  
  lua_pushnumber(L, res);
  return 1;
}

static int L_bothSetCurrent(lua_State *L) {
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
  lps->setCurrent(2, curr);
  
  return 1;
}

static int L_bothGetCurrent(lua_State *L) {
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

  if(res == lps->getCurrent(2))
    lua_pushnumber(L, res);
  else
    lua_pushnil(L);
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
  char *portname = nullptr;
  int baud = 2400, checkError = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isstring(L, 2)) portname = (char*)lua_tostring(L, 2);
  } else {
    std::cerr << "LPS-305 error: This function must contain at least 1 parameter (port name)" << std::endl;
  }
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, 3)) baud = lua_tonumber(L, 3);
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
    return 1;
  }

  lua_newtable(L);
    lua_newtable(L);
      lua_pushstring(L, "_objLPS305_");
      lua_pushlightuserdata(L, lps);
      lua_rawset(L, -3);

      lua_pushstring(L, "__gc");
      lua_pushcfunction(L,L_disconnect);
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

      lua_pushstring(L, "setCurrent");
      lua_pushcfunction(L, L_negSetCurrent);
      lua_rawset(L, -3);

      lua_pushstring(L, "getCurrent");
      lua_pushcfunction(L, L_negGetCurrent);
      lua_rawset(L, -3);

    lua_rawset(L, -3);

    lua_pushstring(L, "both");
    lua_newtable(L);
      lua_newtable(L);
        lua_pushstring(L, "_objLPS305_");
        lua_pushlightuserdata(L, lps);
        lua_rawset(L, -3);
      lua_setmetatable(L, -2);

      lua_pushstring(L, "setVoltage");
      lua_pushcfunction(L, L_bothSetVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, L_bothGetVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setCurrent");
      lua_pushcfunction(L, L_bothSetCurrent);
      lua_rawset(L, -3);

      lua_pushstring(L, "getCurrent");
      lua_pushcfunction(L, L_bothGetCurrent);
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