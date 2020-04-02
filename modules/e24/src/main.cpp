#include <lua5.2/lua.hpp>
#include <iostream>

#define CHANNEL1 1
#define CHANNEL2 2
#define CHANNEL3 3
#define CHANNEL4 4

#define TEST_ADC

#if defined(TEST_ADC)
  #define DEVICE_PATH_FMT "modules/e24/test_adc/adc%d.adc"
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
  #define DEVICE_PATH_FMT "/opt/dev/e24/adc%d.adc"
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
  #define DEVICE_PATH_FMT "C:/dev/e24/adc%d.adc"
#else
  #error "Unsupported platform"
#endif

double ch1_range = 2.5,
       ch2_range = 2.5,
       ch3_range = 2.5,
       ch4_range = 2.5;

static int L_getVoltage(lua_State *L) {
  if(!lua_istable(L, -1)) {
    printf("Call 'readVoltage' as method: e24[0]:readVoltage()\n");
    lua_pushnil(L);
    return 1;
  }

  lua_getmetatable(L, -1);
  if(!lua_istable(L, -1)) {
    printf("E24: something is wrong...\n");
    lua_pushnil(L);
    return 1;
  }

  lua_getfield(L, -1, "ADC_channel");
  int ch = lua_tonumber(L, -1);
  if(ch < 1 || ch > 4) {
    printf("Do not edit hidden fields if you dont know why!\n");
    lua_pushnil(L);
    return 1;
  }

  char fname[100];
  sprintf(fname, DEVICE_PATH_FMT, ch);
  FILE *pf = fopen(fname, "rt");
  if(pf == NULL) {
    lua_pushnil(L);
    return 1;
  }
  double adc = 0;
  fscanf(pf, "%lf", &adc);
  fclose(pf);

  switch (ch) {
  case CHANNEL1:
    if(adc < -ch1_range || adc > ch1_range) {
      printf("The voltage value doesn't fall within the specified range\n");
      lua_pushnil(L);
      return 1;
    }
    break;
  case CHANNEL2:
    if(adc < -ch2_range || adc > ch2_range) {
      printf("The voltage value doesn't fall within the specified range\n");
      lua_pushnil(L);
      return 1;
    }
    break;
  case CHANNEL3:
    if(adc < -ch3_range || adc > ch3_range) {
      printf("The voltage value doesn't fall within the specified range\n");
      lua_pushnil(L);
      return 1;
    }
    break;
  case CHANNEL4:
    if(adc < -ch4_range || adc > ch4_range) {
      printf("The voltage value doesn't fall within the specified range\n");
      lua_pushnil(L);
      return 1;
    }
    break;
  }

  lua_pushnumber(L, adc);
  return 1;
}

static int L_setRange(lua_State *L) {
  double range = luaL_checknumber(L, -1);
  lua_pop(L, 1);
  if(range < 0.1 || range > 2.5) {
    printf("Wrong value of range. Read specification of E-24\n");
    lua_pushnil(L);
    return 1;
  }

  if(!lua_istable(L, -1)) {
    printf("Call 'setRange' as method: e24[0]:setRange()\n");
    lua_pushnil(L);
    return 1;
  }

  lua_getmetatable(L, -1);
  if(!lua_istable(L, -1)) {
    printf("E24: something is wrong...\n");
    lua_pushnil(L);
    return 1;
  }

  lua_getfield(L, -1, "ADC_channel");
  int ch = lua_tonumber(L, -1);
  if(ch < 1 || ch > 4) {
    printf("Do not edit hidden fields if you dont know why!\n");
    lua_pushnil(L);
    return 1;
  }

  switch (ch) {
  case CHANNEL1:
    ch1_range = range;
    break;
  case CHANNEL2:
    ch2_range = range;
    break;
  case CHANNEL3:
    ch3_range = range;
    break;
  case CHANNEL4:
    ch4_range = range;
    break;
  }

  lua_pushnumber(L, range);
  return 1;
}

static int L_help(lua_State *L) {
  lua_pushstring(L, "This module is needed  to work with a voltmeter e24");
  return 1;
}

extern "C" int luaopen_e24(lua_State *L) {
  lua_createtable(L, 0, 0);

    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
    for(int i=1; i<=4; i++){
      lua_createtable(L, 0, 0);
        lua_createtable(L, 0, 0);
          lua_pushnumber(L, i);
          lua_setfield(L, -2, "ADC_channel");
        lua_setmetatable(L, -2);
        lua_pushcfunction(L, L_setRange);
        lua_setfield(L, -2, "setRange");
        lua_pushcfunction(L, L_getVoltage);
        lua_setfield(L, -2, "getVoltage");
      lua_rawseti(L, -2, i);
    }
    return 1;
}
