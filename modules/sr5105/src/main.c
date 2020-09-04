#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include "ttym.h"

//=======================================================================================
//=================== Interlan functions ================================================
//=======================================================================================


typedef struct sr5105{
  ttym_t tty;
  double sens;
}sr5105_t;

static void tty_puts(ttym_t tty, char *str){
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
static void tty_gets(ttym_t tty, char *buf, size_t len){
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

static int findCeilInArr(const double arr[], double val){
  val *= 0.9999; //поправка на машинную точность. Сам SR5105 обладает небольшим запасом, так что небольшая перегрузка допустима
  for(int i=0; arr[i] != FP_NAN; i++){
    if( val <= arr[i] )return i;
  }
  return -1;
}

static double sr5105_setSens(ttym_t tty, double sens){
  const double sens_arr[] = {
     10e-6, 31.6e-6,
    100e-6, 316e-6,
      1e-3, 3.16e-3,
     10e-3, 31.6e-3,
    100e-3, 316e-3,
    1e0,
    FP_NAN
  };
  
  char buf[20];
  int sens_num;
  if(sens > 0){
    sens_num = findCeilInArr(sens_arr, sens);
    if(sens_num < 0)return -1;
    sprintf(buf, "SEN %i\r", sens_num);
    tty_puts(tty, buf);
    tty_gets(tty, buf, 19);
  }
  tty_puts(tty, "SEN\r");
  tty_gets(tty, buf, 19);
  sens_num = atoi(buf);
  return sens_arr[sens_num];
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
  tty_puts(tty, "HPF 0; LPF 3; P 0 0; TC 3; XDB 1; XOF 0 0; YOF 0 0\r");
  
  char buf[1000];
  tty_gets(tty, buf, 1000);
}

//=======================================================================================
//=================== LUA ===============================================================
//=======================================================================================

#define ERROR_LOG(msg) fprintf(stderr, "%s.%s at %d : %s\n", __FILE__, __func__, __LINE__, msg)

sr5105_t* ReadDevice(lua_State *L){
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
    sr5105_t *device = (sr5105_t*)lua_topointer(L, -1);
  lua_pop(L, 2);
  return device;
}

static int L_OnDestroy(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  
  ttym_close(device->tty);
  free(device);
  return 0;
}

static int L_help(lua_State *L){
  const char helpstring[] = "Здесь могла быть ваша справка";
  lua_pushstring(L, helpstring);
  return 1;
}

static int L_reset(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  sr5105_reset(device->tty);
  return 0;
}

static int L_rawSend(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
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
  tty_puts(device->tty, cmd);
  tty_gets(device->tty, resp, 100);
  lua_pushstring(L, resp);
  return 1;
}

static int L_get_id(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  const char *str = sr5105_id(device->tty);
  lua_pushstring(L, str);
  return 1;
}

static int L_getErrors(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  const char *str = sr5105_get_error(device->tty);
  lua_pushstring(L, str);
  return 1;
}

static int L_getVersion(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  char buf[21];
  tty_puts(device->tty, "VER\r");
  tty_gets(device->tty, buf, 20);
  lua_pushstring(L, buf);
  return 1;
}

//if called setSens(val) then set sens and return result; else just return result
static int L_SetSens(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  double sens = -1;
  
  if(lua_gettop(L) > 1){
    if(lua_isnumber(L, -1)){
      sens = lua_tonumber(L, -1);
    }else{
      ERROR_LOG("Input data must be 'string'");
      lua_settop(L, 0);
      lua_pushnil(L);
    }
  }
  sens = sr5105_setSens(device->tty, sens);
  device->sens = sens;
  lua_pushnumber(L, sens);
  return 1;
}

static int L_GetXY(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
    
  tty_puts(device->tty, "XY\r");
  char buffer[20];
  int xval=-100500, yval=100500;
  tty_gets(device->tty, buffer, 20);
  sscanf(buffer, " %d,%d", &xval, &yval);
  lua_pushnumber(L, ((double)xval)*device->sens*0.001);
  lua_pushnumber(L, ((double)yval)*device->sens*0.001);
  return 2;
}

static int L_GetMagPhase(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  
  int mag=-100500, phase=100500;
  char buffer[20];
  tty_puts(device->tty, "MAG\r");
  tty_gets(device->tty, buffer, 19);
  printf("mag:%s\n", buffer);
  mag = atoi(buffer);
  tty_puts(device->tty, "PHA\r");
  tty_gets(device->tty, buffer, 19);
  printf("pha:%s\n", buffer);
  
  phase = atoi(buffer);
  lua_pushnumber(L, ((double)mag)*device->sens*0.001);
  lua_pushnumber(L, ((double)phase)*0.1);
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
  device->sens = 1;
  sr5105_setSens(device->tty, device->sens);
  
  lua_newtable(L);
    lua_newtable(L);
      lua_pushlightuserdata(L, device);
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
    lua_pushcfunction(L, L_getErrors);
    lua_setfield(L, -2, "getErrors");
    lua_pushcfunction(L, L_getVersion);
    lua_setfield(L, -2, "getVersion");
    lua_pushcfunction(L, L_reset);
    lua_setfield(L, -2, "reset");
    
    lua_pushcfunction(L, L_SetSens);
    lua_setfield(L, -2, "setSens");
    lua_pushcfunction(L, L_SetSens); //It is not an error, it is the same function as previous
    lua_setfield(L, -2, "getSens");
    lua_pushcfunction(L, L_GetXY);
    lua_setfield(L, -2, "getXY");
    lua_pushcfunction(L, L_GetMagPhase);
    lua_setfield(L, -2, "getMagPhase");
    
    //TODO: add methods
  return 1;
}
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

int luaopen_sr5105(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");

    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  
  return 1;
}
