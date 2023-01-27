#include <stdio.h>
#include <stdlib.h>
#include "ttym.h"
#include <lua5.2/lua.h>
#include <string.h>

//#define tty_log

typedef struct{
  ttym_t tty;
}device_t;

static void tty_puts(ttym_t tty, char *str){
  int len = strlen(str);
#ifdef tty_log
  printf("write [%s](%i)\n", str, len);
#endif
  ttym_write(tty, str, len);
}

static void tty_gets(ttym_t tty, char *buf, size_t len){
  ssize_t res;
  ssize_t i, j;
  len -= 2;
  for(i=0; i<len; i++){
    for(j=0; j<20; j++){
      res = ttym_read(tty, &buf[i], 1);
      if(res > 0){j=0; break;}
    }
    if(j>0){
      printf("AKIP-114x read timeout\n");
      buf[0] = 0;
      return;     
    }
    if(buf[i] == '\r' || buf[i] == '\n')break;
  }
  if(buf[i] == '\r' || buf[i] == '\n')buf[i] = 0;
  buf[i+1] = 0;
#ifdef tty_log
  printf("read [%s](%i / %i)\n", buf, (int)i, (int)len);
#endif
}

static int L_help(lua_State *L){
  const char helpstring[] = "";

  lua_pushstring(L, helpstring);
  return 1;
}

#define ERROR_LOG(msg) fprintf(stderr, "AKIP-114x.%s.%s at %d : %s\n", __FILE__, __func__, __LINE__, msg)

device_t* ReadDevice(lua_State *L){
  lua_getmetatable(L, 1);
    if(!lua_istable(L, -1)){
      ERROR_LOG("Not metatable!");
      return NULL;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      ERROR_LOG("Not handle\n");
      return NULL;
    }
    device_t *dev = (device_t*)lua_topointer(L, -1);
  lua_pop(L, 2);
  return dev;
}

void dev_reset(ttym_t tty){
  char buf[100];
  tty_puts(tty, "SYST:REM\r\n");
  tty_puts(tty, "*RST\r\n");
  tty_puts(tty, "*CLS\r\n");
  tty_puts(tty, "SYST:ERR?\r\n");
  tty_gets(tty, buf, sizeof(buf));
  tty_puts(tty, "SYST:COMM:SEL RS232\r\n");
}

void dev_disconnect(ttym_t tty){
  tty_puts(tty, "SYST:LOC\r\n");
}

static int L_reset(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  if(lua_gettop(L) < 2){
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  dev_reset(dev->tty);
  return 0;
}

static int L_rawSend(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  if(lua_gettop(L) < 2){
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  if(!lua_isstring(L, 2)){
    ERROR_LOG("Input data must be 'string'");
    lua_settop(L, 0);
    lua_pushnil(L);
  }
  char *cmd = (char*)lua_tostring(L, 2);
  char resp[101];
  tty_puts(dev->tty, cmd);
  tty_gets(dev->tty, resp, 100);
  lua_pushstring(L, resp);
  return 1;
}

static int L_get_id(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  char buf[100];
  tty_puts(dev->tty, "*IDN?\r\n");
  tty_gets(dev->tty, buf, sizeof(buf));
  lua_pushstring(L, buf);
  return 1;
}
/*
static int L_syst_comm(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  char buf[50];
  tty_puts(dev->tty, "SYST:COMM:SEL RS232\r\n"); //без понятия зачем это нужно. Если оно не подключено к RS232, то я его и переключить не смогу
  tty_gets(dev->tty, buf, 50);
  tty_puts(dev->tty, "SYST:COMM:SEL?\r\n");
  tty_gets(dev->tty, buf, 50);
  lua_pushstring(L, buf);
  return 1;
}*/

static int L_setVoltage(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  double U = 0;
  char buf[100];
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) U = lua_tonumber(L, 2);
  }
  sprintf(buf, "VOLT %lg\r\n", U);
  tty_puts(dev->tty, buf);
  return 0;
}

static int L_getVoltage(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  char buf[100];
  tty_puts(dev->tty, "MEAS:VOLT?\r\n");
  tty_gets(dev->tty, buf, sizeof(buf));
  float res;
  sscanf(buf, "%f", &res);
  lua_pushnumber(L, res);
  return 1;
}
static int L_setCurrent(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  double I = 0;
  char buf[100];
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) I = lua_tonumber(L, 2);
  }
  sprintf(buf, "CURR %lg\r\n", I);
  tty_puts(dev->tty, buf);
  return 0;
}
static int L_getCurrent(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  char buf[100];
  tty_puts(dev->tty, "MEAS:CURR?\r\n");
  tty_gets(dev->tty, buf, sizeof(buf));
  float res;
  sscanf(buf, "%f", &res);
  lua_pushnumber(L, res);
  return 1;
}

static int L_CurrentChangeTime(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  double t = 0;
  char buf[100];
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) t = lua_tonumber(L, 2);
  }
  sprintf(buf, "CURR:RISE %lg\r\nCURR:FALL %lg\r\n", t, t);
  tty_puts(dev->tty, buf);
  return 0;
}

static int L_VoltageChangeTime(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  double t = 0;
  char buf[100];
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) t = lua_tonumber(L, 2);
  }
  sprintf(buf, "VOLT:RISE %lg\r\nVOLT:FALL %lg\r\n", t, t);
  tty_puts(dev->tty, buf);
  return 0;
}
static int L_setOutput(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  char buf[100];
  int mode = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) mode = lua_tonumber(L, 2);
  }
  sprintf(buf, "OUTP %i\r\n", mode);
  tty_puts(dev->tty, buf);
  return 0;
}

static int L_getPower(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  char buf[100];
  tty_puts(dev->tty, "MEAS:POW?\r\n");
  tty_gets(dev->tty, buf, sizeof(buf));
  float res;
  sscanf(buf, "%f", &res);
  lua_pushnumber(L, res);
  return 1;
}
static int L_getMaxPower(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  char buf[100];
  tty_puts(dev->tty, "POW:MAX?\r\n");
  tty_gets(dev->tty, buf, sizeof(buf));
  float res;
  sscanf(buf, "%f", &res);
  lua_pushnumber(L, res);
  return 1;
}
static int L_setMaxPower(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  double P = 0;
  char buf[100];
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) P = lua_tonumber(L, 2);
  }
  sprintf(buf, "POW:MAX %lg\r\n", P);
  tty_puts(dev->tty, buf);
  return 1;
}

static int L_frontPanel(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  tty_puts(dev->tty, "SYST:LOC\r\n");
  return 1;
}

static int L_Err(lua_State *L){
  device_t *dev = ReadDevice(L);
  if(dev == NULL)return 0;
  char buf[100];
  
  tty_puts(dev->tty, "SYST:ERR?\r\n");
  tty_gets(dev->tty, buf, sizeof(buf));
  lua_pushstring(L, buf);
  return 1;
}

static int L_OnDestroy(lua_State *L){
  device_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  dev_disconnect(device->tty);
  ttym_close(device->tty);
  free(device);
  //printf("AKIP-114x close\n");
  return 0;
}

static int L_reconnect(lua_State *L){
  device_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  char *portname = NULL;
  int baud = 9600;
  if(lua_gettop(L) >= 2){
    if(lua_isstring(L, 2)) portname = (char*)lua_tostring(L, 2);
  }else{
    fprintf(stderr, "AKIP-114x error: This function must contain at least 1 parameter (port name)");
    return 0;
  }
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, 3)) baud = lua_tonumber(L, 3);
  }
  device->tty = ttym_open(portname, baud);
  if(device->tty == NULL){
    fprintf(stderr, "AKIP-114x error: Can not open [%s]\n", portname);
    free(device); device = NULL;
    return 0;
  }
  ttym_timeout(device->tty, 100);
  dev_reset(device->tty);
  return 0;
}

static int L_connectNewDevice(lua_State *L) {
  char *portname = NULL;
  int baud = 9600;
  if(lua_gettop(L) >= 2){
    if(lua_isstring(L, 2)) portname = (char*)lua_tostring(L, 2);
  }else{
    fprintf(stderr, "AKIP-114x error: This function must contain at least 1 parameter (port name)");
    return 0;
  }
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, 3)) baud = lua_tonumber(L, 3);
  }
  
  device_t *dev = (device_t*)malloc(sizeof(device_t));
  if(dev == NULL){
    fprintf(stderr, "AKIP-114x error: not enough space\n");
    return 0;
  }
  dev->tty = ttym_open(portname, baud);
  if(dev->tty == NULL){
    fprintf(stderr, "AKIP-114x error: Can not open [%s]\n", portname);
    free(dev); dev = NULL;
    return 0;
  }
  ttym_timeout(dev->tty, 100);
  dev_reset(dev->tty);

  lua_newtable(L);
    lua_newtable(L);
      lua_pushlightuserdata(L, dev);
      lua_setfield(L, -2, "handle");
      lua_pushcfunction(L, L_OnDestroy);
      lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
    lua_pushcfunction(L, L_rawSend);
    lua_setfield(L, -2, "rawSend");
    lua_pushcfunction(L, L_get_id);
    lua_setfield(L, -2, "getID");
    lua_pushcfunction(L, L_reconnect);
    lua_setfield(L, -2, "reconnect");
    
    lua_pushcfunction(L, L_setVoltage);
    lua_setfield(L, -2, "setVoltage");
    lua_pushcfunction(L, L_getVoltage);
    lua_setfield(L, -2, "getVoltage");
    lua_pushcfunction(L, L_setCurrent);
    lua_setfield(L, -2, "setCurrent");
    lua_pushcfunction(L, L_getCurrent);
    lua_setfield(L, -2, "getCurrent");
    lua_pushcfunction(L, L_setOutput);
    lua_setfield(L, -2, "setOutput");
    
    lua_pushcfunction(L, L_reset);
    lua_setfield(L, -2, "reset");
    lua_pushcfunction(L, L_frontPanel);
    lua_setfield(L, -2, "frontPanel");
    lua_pushcfunction(L, L_getMaxPower);
    lua_setfield(L, -2, "getMaxPower");
    lua_pushcfunction(L, L_setMaxPower);
    lua_setfield(L, -2, "setMaxPower");
    lua_pushcfunction(L, L_CurrentChangeTime);
    lua_setfield(L, -2, "CurrentChangeTime");
    lua_pushcfunction(L, L_VoltageChangeTime);
    lua_setfield(L, -2, "VoltageChangeTime");
    lua_pushcfunction(L, L_getPower);
    lua_setfield(L, -2, "getPower");
    /*lua_pushcfunction(L, L_dummy);
    lua_setfield(L, -2, "getModel");
    lua_pushcfunction(L, L_dummy);
    lua_setfield(L, -2, "getVersion");
    lua_pushcfunction(L, L_dummy);
    lua_setfield(L, -2, "getDeviceHelp");
    */
    lua_pushcfunction(L, L_Err);
    lua_setfield(L, -2, "error");
  return 1;
}

int luaopen_akip_114x(lua_State *L){
  lua_createtable(L, 0, 0);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
    
    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  return 1;
}
