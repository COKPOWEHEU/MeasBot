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
  int sens_num = -1;
  if(sens >= 0){
    sens_num = findCeilInArr(sens_arr, sens);
    if(sens_num < 0)return -1;
    sprintf(buf, "SEN %d\r", sens_num);
    tty_puts(tty, buf);
    tty_gets(tty, buf, 19);
  }
  tty_puts(tty, "SEN\r");
  tty_gets(tty, buf, 19);
  sens_num = atoi(buf);
  return sens_arr[sens_num];
}

static double SetTimeConst(sr5105_t *device, double time){
  const double time_consts[] = {
    300e-6, 1e-3, 3e-3, 10e-3, 30e-3,
    100e-3, 300e-3, 1, 3, 10,
    FP_NAN
  };
  char buf[20];
  int time_num = -1;
  if(time > 0){
    time_num = findCeilInArr(time_consts, time);
  }
  if(time_num >= 0){
    sprintf(buf, "XTC %d\r", time_num);
    tty_puts(device->tty, buf);
    tty_gets(device->tty, buf, 19);
    if(time_num <= 3){ //time <= 10ms - see manual
      if(device->sens < 400e-6){
        printf("WARNING: sr5105 sensitivity was reduced due time constant\n");
      }
      device->sens = sr5105_setSens(device->tty, 0);
    }
  }else{
    tty_puts(device->tty, "XTC\r");
    tty_gets(device->tty, buf, 19);
    time_num = atoi(buf);
  }
  return time_consts[time_num];
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
  tty_puts(tty, "DD 32; HPF 0; LPF 3; P 0 0; TC 3; XDB 1; XOF 0 0; YOF 0 0\r");
  
  char buf[100];
  tty_gets(tty, buf, 100);
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
  const char helpstring[] = 
    "SR5105\n"
    "  connectNewDevice(path, [baudrate]):table - connect to SR5105. Default baudrate is 4800\n"
    "  help():string - return this help\n"
    "  rawSend(str):str - send raw string to device and return it response\n"
    "  getID():str - return ID string of device\n"
    "  getErrors():str - return composition of all errors (one error by line)\n"
    "  getVersion():str - return Version string from device\n"
    "  reset():nil - reset some parameters: delimiter symbol, input filters, phase, offsets, time constants, output filter\n"
    "  setSens(num):num - set sensitivity (in Volts per full scale) and return it\n"
    "  getSens():num - return current sensitivity (in Volts)\n"
    "  getXY():num,num,num - read and return X and Y values of input signal and maximum avaible value (sensitivity)\n"
    "  getMagPhase():num,num,num - read and return Magnitude and Phase and then maximum avaible value of Magnitude\n"
    "  getFreq(nil):num - return reference frequency (in Hertz)\n"
    "  setRefPhase(num):nil - set signal phase (in degrees)\n"
    "             (str 'Auto'):nil - set reference phase automatically my maximizing X component and minimizing Y compontnt\n"
    "             (nil):num - same as 'getRefPhase'\n"
    "  getRefPhase(nil):num - read current refeence phase (in degrees)\n"
    "  setFilters(num,num):num,num - set High-pass and Low-pass filters (in Hertz) and return actual values. Correct High-pass filter frequencies are 1, 10, 100, 1000 Hz. Correct Low-pass filter frequencies are 50, 500 Hz, 5, 50 kHz. If one of input values in zero, this filter does not change\n"
    "  getFilters (nil):num,num - read current High-pass and Low-pass filters (in Hertz)\n"
    "  setOutputOffsets(num,num):num,num - set output offsets (X, Y) in fraction of full scale. Avaible values are -200..200, where 1000 is full range. If one of inputs is zero, if will not change\n"
    "                  (str 'Auto'):num,num - set current values of X and Y as offsets and return them\n"
    "                  (nil):num,num - same as 'getOutputOffsets'\n"
    "  getOutputOffsets():num,num - return current offset values X and Y\n"
    "  setTimeConstant(num):num - set output Time constant (in Seconds) and return it\n"
    "  getTimeConstant():num - return current Time constant (in Seconds)\n"
    "  setDynamicReserve(num):num - set Dynamic reserve:\n"
    "                   (0) - high stability\n"
    "                   (1) - normal\n"
    "                   (2) - high reserve\n"
    "  getDynamicReserve():num - return curent Dynamic reserve\n"
    "  setOutputSlope_6():nil - set output filter slope to 6 dB/octave\n"
    "  setOutputSlope_12():nil - set output filter slope to 12 dB/octave\n"
    "  outputConfig(num, bool, bum):str - combination of setTimeConstant (1st), setOutputSlope (2nd) and setDynamicReserve (3rd) and return getErrors()\n"
  ;
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
  sscanf(buffer, " %d %d", &xval, &yval);
  lua_pushnumber(L, ((double)xval)*device->sens*0.001);
  lua_pushnumber(L, ((double)yval)*device->sens*0.001);
  lua_pushnumber(L, device->sens);
  return 3;
}

static int L_GetMagPhase(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  
  int mag=-100500, phase=100500;
  char buffer[20];
  tty_puts(device->tty, "MAG\r");
  tty_gets(device->tty, buffer, 19);
  mag = atoi(buffer);
  tty_puts(device->tty, "PHA\r");
  tty_gets(device->tty, buffer, 19);
  
  phase = atoi(buffer);
  lua_pushnumber(L, ((double)mag)*device->sens*0.001);
  lua_pushnumber(L, ((double)phase)*0.1);
  lua_pushnumber(L, device->sens);
  return 3;
}

//если вызвано с параметром "auto" (или любой другой строкой) выставляем автоматически
//если вызвано без параметров, только читаем фазу
static int L_SetPhase(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  char buf[20];
  if(lua_gettop(L) == 2){
    if(lua_isnumber(L, 2)){
      int phase = 10*lua_tonumber(L, 2);
      phase %= 3600;
      if(phase < 0)phase += 3600;
      int quad = phase / (90*10);
      phase %= (90*10);
      sprintf(buf, "P %d %d\r", quad, phase);
      tty_puts(device->tty, buf);
      tty_gets(device->tty, buf, 19);
      return 0;
    }else if(lua_isstring(L, 2)){
      char *cmd = (char*)lua_tostring(L, 2);
      if(strcasecmp(cmd, "auto")==0){
        tty_puts(device->tty, "AQN\r");
        tty_gets(device->tty, buf, 19);
        return 0;
      }else{
        ERROR_LOG("SetPhase. Incorrect string");
        return 0;
      }
    }else{
      ERROR_LOG("SetPhase. Incorrect argument");
      return 0;
    }
  }else{
    tty_puts(device->tty, "P\r");
    tty_gets(device->tty, buf, 19);
    int n1, n2;
    sscanf(buf, " %d %d", &n1, &n2);
    float phase = 90.0*n1 + (n2*0.1);
    lua_pushnumber(L, phase);
    return 1;
  }
}

static int L_GetFreq(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  int freq = -1;
  char buffer[20];
  tty_puts(device->tty, "FRQ\r");
  tty_gets(device->tty, buffer, 19);
  freq = atoi(buffer);
  lua_pushnumber(L, freq*0.001);
  return 1;
}

//если параметров нет, возвращаем текущие значения, если есть - меняем
static int L_Filters(lua_State *L){
  const double hpf_arr[] = { //небольшой хак чтобы поменять подбор "сверху" на подбор "снизу"
    -1000, -100, -10, -1, FP_NAN
  };
  const double lpf_arr[] = {
    50, 500, 5000, 50000, FP_NAN
  };
  const double lpf_arr2[] = { //TODO: WTF?!
    2.2e2, 2.2e3, 2.2e4, 2.2e5, FP_NAN
  }; (void)lpf_arr2;
  
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  char buf[20];
  double flt;
  int hpf, lpf;
  if(lua_isnumber(L, 2)){
    flt = lua_tonumber(L, 2);
    hpf = 3-findCeilInArr(hpf_arr, -(flt*1.001));
    if(hpf < 0 || hpf > 3)hpf = 0;
    sprintf(buf, "HPF %d\r", hpf);
    tty_puts(device->tty, buf);
    tty_gets(device->tty, buf, 19);
  }else{
    tty_puts(device->tty, "HPF\r");
    tty_gets(device->tty, buf, 19);
    hpf = atoi(buf);
  }
  if(lua_isnumber(L, 3)){
    flt = lua_tonumber(L, 3);
    lpf = findCeilInArr(lpf_arr, flt);
    if(lpf < 0)lpf = 0;
    sprintf(buf, "LPF %d\r", lpf);
    tty_puts(device->tty, buf);
    tty_gets(device->tty, buf, 19);
  }else{
    tty_puts(device->tty, "LPF\r");
    tty_gets(device->tty, buf, 19);
    lpf = atoi(buf);
  }
  lua_pushnumber(L, -hpf_arr[3-hpf]);
  lua_pushnumber(L, lpf_arr[lpf]);
  return 2;
}

//если вызвано без аргументов, вернуть текущие значения
//если с 1 аргументом - строкой 'Auto' - включить авто-смещение
//если с 2 аргументами (возможно, nil) - установить новые. Если любой из аргументов 0 или nil - выключить смещение на текущем канале
static int L_Offsets(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  char buf[20];
  int xen=1, xval=0, yen=1, yval=0;
    
  if(lua_gettop(L) == 2){
    char *cmd = NULL;
    if(lua_isstring(L, -1)){
      cmd = (char*)lua_tostring(L, -1);
      if(strcasecmp(cmd, "auto")==0){
        tty_puts(device->tty, "AXO\r");
        tty_gets(device->tty, buf, 19);
      }
    }
  }else if(lua_gettop(L) == 3){
    if(lua_isnumber(L, 2))xval = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3))yval = lua_tonumber(L, 3);
    if(xval == 0){
      tty_puts(device->tty, "XOF 0\r"); tty_gets(device->tty, buf, 19);
    }else{
      sprintf(buf, "XOF 1 %d\r", xval);
      tty_puts(device->tty, buf); tty_gets(device->tty, buf, 19);
    }
    if(yval == 0){
      tty_puts(device->tty, "YOF 0\r"); tty_gets(device->tty, buf, 19);
    }else{
      sprintf(buf, "YOF 1 %d\r", yval);
      tty_puts(device->tty, buf); tty_gets(device->tty, buf, 19);
    }
  }
  if(xval == 0){
    tty_puts(device->tty, "XOF\r");
    tty_gets(device->tty, buf, 19);
    sscanf(buf, " %d %d", &xen, &xval);
  }
  if(yval == 0){
    tty_puts(device->tty, "YOF\r");
    tty_gets(device->tty, buf, 19);
    sscanf(buf, " %d %d", &yen, &yval);
  }
  //TODO: возвращаемое значение
  if(xen == 0)lua_pushnumber(L, 0); else lua_pushnumber(L, xval);
  if(yen == 0)lua_pushnumber(L, 0); else lua_pushnumber(L, yval);
  return 2;
}

static int L_TimeConst(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  double time = -1;
  if(lua_isnumber(L, 2))time = lua_tonumber(L, 2);
  time = SetTimeConst(device, time);
  lua_pushnumber(L, time);
  return 1;
}

static int L_DynReserve(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  int mode = -1;
  char buf[20];
  if(lua_isnumber(L, 2))mode = lua_tonumber(L, 2);
  if(mode >=0 && mode <=2){
    sprintf(buf, "DR %d\r", mode);
    tty_puts(device->tty, buf);
    tty_gets(device->tty, buf, 19);
    lua_pushnumber(L, mode);
    return 1;
  }else{
    tty_puts(device->tty, "DR\r");
    tty_gets(device->tty, buf, 19);
    mode = atoi(buf);
    lua_pushnumber(L, mode);
    return 1;
  }
}

static int L_Slope_6(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  tty_puts(device->tty, "XDB 0\r");
  char buf[20];
  tty_gets(device->tty, buf, 19);
  return 0;
}
static int L_Slope_12(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  tty_puts(device->tty, "XDB 1\r");
  char buf[20];
  tty_gets(device->tty, buf, 19);
  return 0;
}

static int L_OutConfig(lua_State *L){
  sr5105_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  
  if(lua_isnumber(L, 2) && lua_isboolean(L, 3) && lua_isnumber(L, 4)){
    double time = lua_tonumber(L, 2);
    int slope_12 = lua_toboolean(L, 3);
    int dynrsv = lua_tonumber(L, 4);
    char buf[20];
    SetTimeConst(device, time);
    if(slope_12){
      tty_puts(device->tty, "XDB 1\r"); tty_gets(device->tty, buf, 19);
    }else{
      tty_puts(device->tty, "XDB 0\r"); tty_gets(device->tty, buf, 19);
    }
    sprintf(buf, "DR %d\r", dynrsv);
    tty_puts(device->tty, buf); tty_gets(device->tty, buf, 19);
    const char *str = sr5105_get_error(device->tty);
    lua_pushstring(L, str);
    return 1;
  }else{
    ERROR_LOG("Wrong parameters( time(float), slope12(bool), DRmode(0-2))");
    return 0;
  }
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
    lua_pushcfunction(L, L_SetSens); //(!)It is not an error, it is the same function as previous
    lua_setfield(L, -2, "getSens");
    lua_pushcfunction(L, L_GetXY);
    lua_setfield(L, -2, "getXY");
    lua_pushcfunction(L, L_GetMagPhase);
    lua_setfield(L, -2, "getMagPhase");
    lua_pushcfunction(L, L_SetPhase);
    lua_setfield(L, -2, "setRefPhase");
    lua_pushcfunction(L, L_SetPhase); //(!)It is not an error, it is the same function as previous
    lua_setfield(L, -2, "getRefPhase");
    lua_pushcfunction(L, L_GetFreq);
    lua_setfield(L, -2, "getFreq");
    lua_pushcfunction(L, L_Filters);
    lua_setfield(L, -2, "setFilters");
    lua_pushcfunction(L, L_Filters); //(!)It is not an error, it is the same function as previous
    lua_setfield(L, -2, "getFilters");
    lua_pushcfunction(L, L_Offsets);
    lua_setfield(L, -2, "setOutputOffsets");
    lua_pushcfunction(L, L_Offsets); //(!)It is not an error, it is the same function as previous
    lua_setfield(L, -2, "getOutputOffsets");
    //TODO
    lua_pushcfunction(L, L_TimeConst);
    lua_setfield(L, -2, "setTimeConstant");
    lua_pushcfunction(L, L_TimeConst);
    lua_setfield(L, -2, "getTimeConstant");
    lua_pushcfunction(L, L_DynReserve);
    lua_setfield(L, -2, "setDynamicReserve");
    lua_pushcfunction(L, L_DynReserve);
    lua_setfield(L, -2, "getDynamicReserve");
    lua_pushcfunction(L, L_Slope_6);
    lua_setfield(L, -2, "setOutputSlope_6");
    lua_pushcfunction(L, L_Slope_12);
    lua_setfield(L, -2, "setOutputSlope_12");
    lua_pushcfunction(L, L_OutConfig);
    lua_setfield(L, -2, "outputConfig");
    
    //TODO: add methods
  return 1;
}
#if 1==0

void configOut(int time, int slope, int DRmode); //timeconst + slope + drmode
void setTimeConstant(int time); //XTC
void setSlope(int slope); //XDB
void setDRmode(int DRmode); //DR
#endif

int luaopen_sr5105(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");

    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  
  return 1;
}
