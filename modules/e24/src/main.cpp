#include <lua5.2/lua.hpp>
#include <fstream>

double ch1_begin = -5, ch1_end = 5,
       ch2_begin = -5, ch2_end = 5,
       ch3_begin = -5, ch3_end = 5,
       ch4_begin = -5, ch4_end = 5;

static int ch1_getVoltage(lua_State *L) {
  double voltage;
  std::fstream fs ("/opt/dev/e24/adc1.adc", std::fstream::in);
  if(!fs.is_open()) {
    lua_pushnil(L);
    return 1;
  }

  fs >> voltage;
  fs.close();

  if(ch1_begin > voltage || ch1_end < voltage) {
    lua_pushnil(L);
    return 1;
  }

  lua_pushnumber(L, voltage);
  return 1;
}

static int ch1_setVoltScale(lua_State *L) {
  double begin = luaL_checknumber(L, -2);
  double end = luaL_checknumber(L, -1);

  if(begin < (double)-5 || end > (double)5 || begin > (double)5 || end < (double)-5) {
    ch1_begin = -5;
    ch1_end = 5;
  } else {
    ch1_begin = begin;
    ch1_end = end;
  }
  return 0;
}

static int ch2_getVoltage(lua_State *L) {
  double voltage;
  std::fstream fs ("/opt/dev/e24/adc2.adc", std::fstream::in);
  if(!fs.is_open()) {
    lua_pushnil(L);
    return 1;
  }

  fs >> voltage;
  fs.close();

  if(ch2_begin > voltage || ch2_end < voltage) {
    lua_pushnil(L);
    return 1;
  }

  lua_pushnumber(L, voltage);
  return 1;
}

static int ch2_setVoltScale(lua_State *L) {
  double begin = luaL_checknumber(L, -2);
  double end = luaL_checknumber(L, -1);

  if(begin < (double)-5 || end > (double)5 || begin > (double)5 || end < (double)-5) {
    ch2_begin = -5;
    ch2_end = 5;
  } else {
    ch2_begin = begin;
    ch2_end = end;
  }
  return 0;
}

static int ch3_getVoltage(lua_State *L) {
  double voltage;
  std::fstream fs ("/opt/dev/e24/adc3.adc", std::fstream::in);
  if(!fs.is_open()) {
    lua_pushnil(L);
    return 1;
  }

  fs >> voltage;
  fs.close();

  if(ch3_begin > voltage || ch3_end < voltage) {
    lua_pushnil(L);
    return 1;
  }

  lua_pushnumber(L, voltage);
  return 1;
}

static int ch3_setVoltScale(lua_State *L) {
  double begin = luaL_checknumber(L, -2);
  double end = luaL_checknumber(L, -1);

  if(begin < (double)-5 || end > (double)5 || begin > (double)5 || end < (double)-5) {
    ch3_begin = -5;
    ch3_end = 5;
  } else {
    ch3_begin = begin;
    ch3_end = end;
  }
  return 0;
}

static int ch4_getVoltage(lua_State *L) {
  double voltage;
  std::fstream fs ("/opt/dev/e24/adc4.adc", std::fstream::in);
  if(!fs.is_open()) {
    lua_pushnil(L);
    return 1;
  }

  fs >> voltage;
  fs.close();

  if(ch4_begin > voltage || ch4_end < voltage) {
    lua_pushnil(L);
    return 1;
  }

  lua_pushnumber(L, voltage);
  return 1;
}

static int ch4_setVoltScale(lua_State *L) {
  double begin = luaL_checknumber(L, -2);
  double end = luaL_checknumber(L, -1);

  if(begin < (double)-5 || end > (double)5 || begin > (double)5 || end < (double)-5) {
    ch4_begin = -5;
    ch4_end = 5;
  } else {
    ch4_begin = begin;
    ch4_end = end;
  }
  return 0;
}

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

    lua_pushstring(L, "channel1");
    lua_newtable(L);
      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, ch1_getVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setVoltScale");
      lua_pushcfunction(L, ch1_setVoltScale);
      lua_rawset(L, -3);
    lua_rawset(L, -3);

    lua_pushstring(L, "channel2");
    lua_newtable(L);
      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, ch2_getVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setVoltScale");
      lua_pushcfunction(L, ch2_setVoltScale);
      lua_rawset(L, -3);
    lua_rawset(L, -3);

    lua_pushstring(L, "channel3");
    lua_newtable(L);
      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, ch3_getVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setVoltScale");
      lua_pushcfunction(L, ch3_setVoltScale);
      lua_rawset(L, -3);
    lua_rawset(L, -3);

    lua_pushstring(L, "channel4");
    lua_newtable(L);
      lua_pushstring(L, "getVoltage");
      lua_pushcfunction(L, ch4_getVoltage);
      lua_rawset(L, -3);

      lua_pushstring(L, "setVoltScale");
      lua_pushcfunction(L, ch4_setVoltScale);
      lua_rawset(L, -3);
    lua_rawset(L, -3);

    return 1;
}
