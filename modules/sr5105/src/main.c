#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include "ttym.h"

//=======================================================================================
//=================== Interlan functions ================================================
//=======================================================================================


typedef struct sr5105{
  ttym_t tty;
}sr5105_t;

void tty_puts(ttym_t tty, char *str){
  char *ch = str;
  char repl;
  ssize_t res;
  for(;ch[0] != 0; ch++){
    ttym_write(tty, &ch[0], 1);
    for(int i=0; i<1000; i++){
      res = ttym_read(tty, &repl, 1);
      if(res != 0)break;
    }
  }
}
void tty_gets(ttym_t tty, char *buf, size_t len){
  ssize_t res;
  ssize_t i;
  for(i=0; i<len; i++){
    for(int j=0; j<100; j++){
      res = ttym_read(tty, &buf[i], 1);
      if(res > 0)break;
    }
    if(buf[i] == '*' || buf[i] == '?')break;
  }
  buf[i+1] = 0;
}

static const char* sr5105_id(ttym_t tty){
  static char buf[20];
  tty_puts(tty, "ID\r");
  tty_gets(tty, buf, 20);
  return buf;
}

#define str_append(str, app) do{memcpy(str, app, sizeof(app)); str+=sizeof(app)-1; }while(0)
static const char* sr5105_get_error(ttym_t tty){
  static char buf[100];
  char *ch = buf;
  tty_puts(tty, "ST\r");
  tty_gets(tty, buf, 20);
  unsigned int errcode = 0;
  for(;ch[0]!=0; ch++){
    if(ch[0]>='0' && ch[0]<='9')errcode = errcode*10 + ch[0]-'0';
  }
  
  if(errcode == 1)return NULL;
  ch = buf;
  if(errcode & (1<<0))str_append(ch, "OK\n");
  if(errcode & (1<<1))str_append(ch, "Invalid Command\n");
  if(errcode & (1<<2))str_append(ch, "Parameter error\n");
  if(errcode & (1<<3))str_append(ch, "Reference unlock\n");
  if(errcode & (1<<4))str_append(ch, "Overload\n");
  
  ch[0] = 0;
  return buf;
}

static void sr5105_reset(ttym_t tty){
  tty_puts(tty, "HPF 0; LPF 3; P 0 0; SEN 10; TC 3; XDB 1; XOF 0 0; YOF 0 0\r");
  
  char buf[1000];
  tty_gets(tty, buf, 1000);
}

//=======================================================================================
//=================== LUA ===============================================================
//=======================================================================================

#if 1==0
ID
VERSION
reset

void defDelimeterControl(int n); //DD %i
void setFreqOfHPF(int n); //HPF %i
void setFreqOfLPF(int n); //LPF %i
void setSens(int mode); //SEN %d
void configRefChannel(int refPh); //P %d %d
void configOutChannels(int statusX, int rangeX, int statusY, int rangeY); //XOF %d %d or YOF %d %d
void configOut(int time, int slope, int DRmode); //timeconst + slope + drmode
void runAutoPhase(); //AQN
void runAutoOffset(); //AXO
double readX(); //XOF
double readY(); //YOF
void setTimeConstant(int time); //XTC
void setSlope(int slope); //XDB
void setDRmode(int DRmode); //DR
double getSens(); //SEN
double outRefPh(); //P
double outChannelX(); //X
complex<double> outChannelXY(); //XY
double outMagnitude(); //MAG
double outSigPh(); //PHA
double outFrequency(); //FRQ        
#endif

#define ERROR_LOG(msg) fprintf(stderr, "%s.%s at %d : %s\n", __FILE__, __func__, __LINE__, msg)

static int L_help(lua_State *L){
  const char helpstring[] = "Здесь могла быть ваша справка";
  lua_pushstring(L, helpstring);
  return 1;
}

static int L_reset(lua_State *L){
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      ERROR_LOG("Not metatable!");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      ERROR_LOG("Not handle\n");
      return 0;
    }
    sr5105_t *device = (sr5105_t*)lua_topointer(L, -1);
  lua_pop(L, 2);
  sr5105_reset(device->tty);
  return 0;
}

static int L_get_id(lua_State *L){
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      ERROR_LOG("Not metatable!");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      ERROR_LOG("Not handle\n");
      return 0;
    }
    sr5105_t *device = (sr5105_t*)lua_topointer(L, -1);
    const char *str = sr5105_id(device->tty);
    
    lua_pop(L, 2);
    lua_pushstring(L, str);
  return 1;
}

static int L_getErrors(lua_State *L){
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      ERROR_LOG("Not metatable!");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      ERROR_LOG("Not handle\n");
      return 0;
    }
    sr5105_t *device = (sr5105_t*)lua_topointer(L, -1);
    const char *str = sr5105_get_error(device->tty);
    
    lua_pop(L, 2);
    lua_pushstring(L, str);
  return 1;
}

static int L_getVersion(lua_State *L){
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      ERROR_LOG("Not metatable!");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      ERROR_LOG("Not handle\n");
      return 0;
    }
    sr5105_t *device = (sr5105_t*)lua_topointer(L, -1);
  lua_pop(L, 2);
  char buf[21];
  tty_puts(device->tty, "VER\r");
  tty_gets(device->tty, buf, 20);
  lua_pushstring(L, buf);
  return 1;
}

static int L_OnDestroy(lua_State *L){
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      ERROR_LOG("Not metatable!");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      ERROR_LOG("Not handle\n");
      return 0;
    }
    sr5105_t *device = (sr5105_t*)lua_topointer(L, -1);
    ttym_close(device->tty);
    free(device);
  lua_pop(L, 2);
  return 0;
}

static int L_GetXY(lua_State *L){
  lua_getmetatable(L, -1);
    if(!lua_istable(L, -1)){
      ERROR_LOG("Not metatable!");
      return 0;
    }
    lua_getfield(L, -1, "handle");
      if(!lua_islightuserdata(L, -1)){
      ERROR_LOG("Not handle\n");
      return 0;
    }
    sr5105_t *device = (sr5105_t*)lua_topointer(L, -1);
  lua_pop(L, 2);
  ttym_t tty = device->tty;
    
  tty_puts(tty, "XY\r");
  char buffer[20];
  int xval=-100500, yval=100500;
  tty_gets(tty, buffer, 20);
  sscanf(buffer, "%i%i", &xval, &yval);
  lua_pushnumber(L, xval);
  lua_pushnumber(L, yval);
  return 2;
}

static int L_connectNewDevice(lua_State *L){
  const char *ttypath;
  unsigned int baudrate = 4800;
  
  if(lua_gettop(L) < 2){
    printf("Call function as METHOD and specify path to COM-port!\n");
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
  }
  if(!lua_isstring(L, 2)){
    printf("Specify path to COM-port\n");
    lua_settop(L, 0);
    lua_pushnil(L);
  }
  ttypath = lua_tostring(L, 2);
  if((lua_gettop(L) == 3) && lua_isnumber(L,3)){
    baudrate = lua_tonumber(L, 3);
  }
  
  sr5105_t *device = (sr5105_t*)malloc(sizeof(sr5105_t));
  if(device == NULL){
    //TODO: обработать ошибку
  }
  
  device->tty = ttym_open((char*)ttypath, baudrate);
  if(device->tty == NULL){
    printf("Can not open tty at [%s]\n", ttypath);
    lua_settop(L, 0);
    lua_pushnil(L);
    return 0;
  }
  ttym_timeout(device->tty, 100);
  
  sr5105_id(device->tty);
  sr5105_get_error(device->tty);
  sr5105_reset(device->tty);
  
  lua_newtable(L);
    lua_newtable(L);
      lua_pushlightuserdata(L, device);
      lua_setfield(L, -2, "handle");
      lua_pushcfunction(L, L_OnDestroy);
      lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
    lua_pushcfunction(L, L_get_id);
    lua_setfield(L, -2, "getID");
    lua_pushcfunction(L, L_getErrors);
    lua_setfield(L, -2, "getErrors");
    lua_pushcfunction(L, L_getVersion);
    lua_setfield(L, -2, "getVersion");
    lua_pushcfunction(L, L_reset);
    lua_setfield(L, -2, "reset");
    lua_pushcfunction(L, L_GetXY);
    lua_setfield(L, -2, "getXY");
  //TODO TODO
  
  return 1;
}

int luaopen_sr5105(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");

    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  
  return 1;
}
