#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include "ttym.h"

#define ERROR_LOG(message) printf("%s:%s:%d:%s\n", __FILE__, __func__, __LINE__, message)

typedef struct sr830 {
  ttym_t tty;
} sr830_t;
#if 0
static void stackDump (lua_State *L) {
      int i;
      int top = lua_gettop(L);
      for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {
    
          case LUA_TSTRING:  /* strings */
            printf("`%s'", lua_tostring(L, i));
            break;
    
          case LUA_TBOOLEAN:  /* booleans */
            printf(lua_toboolean(L, i) ? "true" : "false");
            break;
    
          case LUA_TNUMBER:  /* numbers */
            printf("%g", lua_tonumber(L, i));
            break;
    
          default:  /* other values */
            printf("%s", lua_typename(L, t));
            break;
    
        }
        printf("  ");  /* put a separator */
      }
      printf("\n");  /* end the listing */
    }
#endif

int findCeilInArr(const double arr[], double val){
  for(int i = 0; arr[i] != NAN; i++){
    if(val <= arr[i] )return i;
  }
  return -1;
}

//=======================================================================================
//=================== C prototypes ======================================================
//=======================================================================================

void reset_device(ttym_t tty) {
  int n;
  char buff[20];
  n = ttym_write(tty, (void*)"*RST\r", 5);
  if(n != 5) {
    ERROR_LOG("Error of enable reset");
    return;
  }
  ttym_read(tty, buff, 19);
}

void clear_registers(ttym_t tty) {
  int n;
  char buff[20];
  n = ttym_write(tty, (void*)"*CLS\r",5);
  if(n != 5) {
    ERROR_LOG("Error of clear status bytes");
    return;
  }
  ttym_read(tty, buff, 19);
}

void setInputMode(ttym_t tty, int mode) {
  if(mode < 0 || mode > 3) {
    ERROR_LOG("Wrong value of mode");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "ISRC %d\r", mode);
  n = ttym_write(tty, buff, 7);
  if(n != 7) {
    ERROR_LOG("Error of set input mode");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getInputMode(ttym_t tty) {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"ISRC?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get input mode");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0)buff[n]=0;
  res = atoi(buff);
  return res;
}

void setInputGrounded(ttym_t tty, int mode) {
  int n;
  char buff[20];
  sprintf(buff, "IGND %d\r", mode);
  n = ttym_write(tty, buff, 7);
  if(n != 7) {
    ERROR_LOG("Error of set input grounded");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getInputGrounded(ttym_t tty) {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"IGND?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get input grounded");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0)buff[n]=0;
  res = atoi(buff);
  return res;
}

void setInputCap(ttym_t tty, int mode) {
  int n;
  char buff[20];
  sprintf(buff, "ICPL %d\r", mode);
  n = ttym_write(tty, buff, 7);
  if(n != 7) {
    ERROR_LOG("Error of set input cup");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getInputCap(ttym_t tty) {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"ICPL?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error of get input cup");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0)buff[n]=0;
  res = atoi(buff);
  return res;
}

void setPhase(ttym_t tty, double phase) {
  if(phase < (-360.00) || phase > 729.99) {
    ERROR_LOG("Wrong value of phase");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "PHAS %.2f\r", phase);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Phase set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

double getPhase(ttym_t tty) {
  int n;
  double res;
  char buff[20];
  n = ttym_write(tty, (void*)"PHAS?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get input mode");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0)buff[n]=0;
  res = atof(buff);
  return res;
}

void setSens(ttym_t tty, double sens) {
  const double sens_arr[] = {
    2e-9, 5e-9, 1e-8,
    2e-8, 5e-8, 1e-7,
    2e-7, 5e-7, 1e-6,
    2e-6, 5e-6, 1e-5,
    2e-5, 5e-5, 1e-4,
    2e-4, 5e-4, 1e-3,
    2e-3, 5e-3, 1e-2,
    2e-2, 5e-2, 1e-1,
    2e-1, 5e-1, 1   ,
    NAN
  };

  int n, sens_num = -1;
  char buff[20];

  sens_num = findCeilInArr(sens_arr, sens);
  if(sens_num == -1) {
    ERROR_LOG("Wrong sens");
    return;
  }
  sprintf(buff, "SENS %d\r", sens_num);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Sensitivity set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

double getSens(ttym_t tty) {
  const double sens_arr[] = {
    2e-9, 5e-9, 1e-8,
    2e-8, 5e-8, 1e-7,
    2e-7, 5e-7, 1e-6,
    2e-6, 5e-6, 1e-5,
    2e-5, 5e-5, 1e-4,
    2e-4, 5e-4, 1e-3,
    2e-3, 5e-3, 1e-2,
    2e-2, 5e-2, 1e-1,
    2e-1, 5e-1, 1   ,
    NAN
  };

  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"SENS?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get sensitivity");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0)buff[n]=0;
  res = atoi(buff);
  return sens_arr[res];
}

void setHarmonic(ttym_t tty, int harm) {
  if(harm < 1 || harm > 19999) {
    ERROR_LOG("Wrong value of detection harmonic");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "HARM %d\r", harm);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Error set harmonic");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getHarmonic(ttym_t tty) {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"HARM?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get harmonic");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0)buff[n]=0;
  res = atoi(buff);
  return res;
}

void setFrequency(ttym_t tty, double freq) {
  if(freq < 0.001 || freq > 102000) {
    ERROR_LOG("Wrong value of reference frequency");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "FREQ %lf\r", freq);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Error set reference frequency");
    return;
  }
  ttym_read(tty, buff, 19);
}

double getFrequency(ttym_t tty) {
  int n;
  double res;
  char buff[20];
  n = ttym_write(tty, (void*)"FREQ?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get reference frequency");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0)buff[n]=0;
  res = atof(buff);
  return res;
}

void setRefSource(ttym_t tty, int mode) {
  int n;
  char buff[20];
  sprintf(buff, "FMOD %d\r", mode);
  n = ttym_write(tty, buff, 7);
  if(!n) {
    ERROR_LOG("Reference source set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getRefSource(ttym_t tty) {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"FMOD?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get reference source");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0)buff[n]=0;
  res = atoi(buff);
  return res;
}

void setTimeConst(ttym_t tty, double time) {
  const double time_arr[] = {
    1e-5, 3e-5,
    1e-4, 3e-4,
    1e-3, 3e-3,
    1e-2, 3e-2,
    1e-1, 3e-1,
    1e+0, 3e+0,
    1e+1, 3e+1,
    1e+2, 3e+2,
    1e+3, 3e+3,
    1e+4, 3e+4,
    NAN
  };

  int n, time_num = -1;
  char buff[20];

  time_num = findCeilInArr(time_arr, time);
  if(time_num < 0) {
    ERROR_LOG("Wrong time value");
    return;
  }
  sprintf(buff, "OFLT %d\r", time_num);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Time constant set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

double getTimeConst(ttym_t tty) {
  const double time_arr[] = {
    1e-5, 3e-5,
    1e-4, 3e-4,
    1e-3, 3e-3,
    1e-2, 3e-2,
    1e-1, 3e-1,
    1e+0, 3e+0,
    1e+1, 3e+1,
    1e+2, 3e+2,
    1e+3, 3e+3,
    1e+4, 3e+4,
    NAN
  };
  int n, res;
  char buff[20];
  
  n = ttym_write(tty, (void*)"OFLT?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get time constant");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0) buff[n] = 0;
  res = atoi(buff);
  return time_arr[res];
}

void setSlope(ttym_t tty, double slope) {
  const double slope_arr[] = {6, 12, 18, 24};

  int n, slope_num = -1;
  slope_num = findCeilInArr(slope_arr, slope);
  if(slope_num < 0) {
    ERROR_LOG("Wrong value of the low pass filter slope");
    return;
  }
  char buff[20];
  sprintf(buff, "OFSL %d\r", slope_num);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Low pass filter slope set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getSlope(ttym_t tty) {
  const double slope_arr[] = {6, 12, 18, 24};
  int n, res;
  char buff[20];
  
  n = ttym_write(tty, (void*)"OFSL?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get filter slope");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0) buff[n] = 0;
  res = atoi(buff);
  return (int)slope_arr[res];
}

void setDynReserve(ttym_t tty, int mode) {
  int n;
  char buff[20];
  if(mode < 0 || 2 < mode) {
    ERROR_LOG("Wrong value of mode");
    return;
  }
  
  sprintf(buff, "RMOD %d\r", mode);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Reverse mode set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getDynReserve(ttym_t tty) {
  int n, mode;
  char buff[20];
  n = ttym_write(tty, (void*)"RMOD?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get reverse mode");
    return -1001;
  }
  n = ttym_read(tty, buff, 19);
  if(n > 0) buff[n] = 0;
  mode = atoi(buff);
  return mode;
}

void setAmplitude(ttym_t tty, double volt) {
  if(volt < 0 || volt > 5.000) {
    ERROR_LOG("Wrong value of amplitude of the sine output");
    return;
  }
  if(volt < 0.004)volt = 0.004;
  int n;
  char buff[20];
  sprintf(buff, "SLVL %f\r", volt);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Error set amplitude of the sine output");
    return;
  }
  ttym_read(tty, buff, 19);
}

double getAmplitude(ttym_t tty) {
  int n;
  double res;
  char buff[20];

  n = ttym_write(tty, (void*)"SLVL?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get amplitude of the sine output");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0) buff[n] = 0;
  res = atof(buff);
  return res;
}

//Замена имени
void setRefTrig(ttym_t tty, int mode) {
  if(mode < 0 || mode > 2) {
    ERROR_LOG("Wrong value of reference trigger");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "RSLP %d\r", mode);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Reference trigger set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getRefTrig(ttym_t tty) {
  int n, mode;
  char buff[20];
  n = ttym_write(tty, (void*)"RSLP?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error get reference trigger");
    return -1001;
  }
  n = ttym_read(tty, buff, 19);
  if(n > 0) buff[n] = 0;
  mode = atoi(buff);
  return mode;
}

//Замена имени
void setStateOfBandpassFilter(ttym_t tty, int state) {
  if(state < 0 || state > 3) {
    ERROR_LOG("Wrong value of state");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "ILIN %d\r", state);
  n = ttym_write(tty, buff, 7);
  if(n != 7) {
    ERROR_LOG("Bandpass filter state set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getStateOfBandpassFilter(ttym_t tty) {
  int n, state;
  char buff[20];
  n = ttym_write(tty, (void*)"ILIN?\r", 6);
  if(!n) {
    ERROR_LOG("Error get bandpass filter state");
    return -1001;
  }
  n = ttym_read(tty, buff, 19);
  if(n > 0) buff[n] = 0;
  state = atoi(buff);
  return state;
}

//Замена имени
void setStateOfSyncFilter(ttym_t tty, int state) {
  int n;
  char buff[20];
  sprintf(buff, "SYNC %d\r", state);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Synchronous filter state set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

int getStateOfSyncFilter(ttym_t tty) {
  int n, state;
  char buff[20];
  n = ttym_write(tty, (void*)"SYNC?\r", 6);
  if(!n) {
    ERROR_LOG("Error get synchronous filter status");
    return -1001;
  }
  n = ttym_read(tty, buff, 19);
  if(n > 0) buff[n] = 0;
  state = atoi(buff);
  return state;
}

//Замена имени
void setAuxOutVolt(ttym_t tty, int nOutput, double volt) {
  if(nOutput < 1 || nOutput > 4 || volt < (-10.500) || volt > (10.500)) {
    ERROR_LOG("Wrong value of aux output voltage");
    return;
  }
  char buff[20];
  int n;
  sprintf(buff, "AUXV%d, %f\r", nOutput, volt);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Aux output voltage set error");
    return;
  }
  ttym_read(tty, buff, 19);
}

double getAuxOutVolt(ttym_t tty, int nOutput) {
  if(nOutput < 1 || nOutput > 4) {
    ERROR_LOG("Wrong value of aux output voltage");
    return -1004;
  }
  int n;
  double res;
  char buff[20];
  sprintf(buff, "AUXV? %d\r", nOutput);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Aux output voltage get error");
    return -1001;
  }

  n = ttym_read(tty, buff, 19);
  if(n > 0) buff[n] = 0;
  res = atof(buff);
  return res;
}

//=======================================================================================
//=================== LUA ===============================================================
//=======================================================================================

sr830_t* ReadDevice(lua_State *L){
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
    sr830_t *device = (sr830_t*)lua_topointer(L, -1);
  lua_pop(L, 2);
  return device;
}

int ReadChannel(lua_State *L) {
  lua_getmetatable(L, 1);
    if(!lua_istable(L, -1)){
      ERROR_LOG("Not metatable!");
      return -1;
    }
    lua_getfield(L, -1, "channel");
      if(!lua_isnumber(L, -1)){
      ERROR_LOG("Not channel\n");
      return -1;
    }
    int ch = lua_tonumber(L, -1);
  lua_pop(L, 2);
  return ch;
}

static int L_help(lua_State *L) {
  const char helpstring[] = 
    "SR830\n"
    "  connectNewDevice(path, [baudrate]):table - connect to SR830\n"
    "  help():string - return this help\n"
    "  rawSend(str):str - send raw string to device and return it response\n"
    "  getID():str - return ID string of device\n"
    "  reset():nil - reset device\n"
    "\n"
    "  SineOut: table\n"
    "    setVoltage(num):nil - set output amplitude (in Volts)\n"
    "    getVoltage():num - get setted amplitude (in Volts)\n"
    "    setRange():num - set maximum output amplitude (in Volts). Dummy function due fixed range (0...+5 V)\n"
    "    setFreq(num):nil - set output frequency (in Hertz)\n"
    "    getFreq():num - get setted frequency (in Hertz)\n"
    "    setIntRefSource(bool):nil - set internal (true) or external (false) reference signal\n"
    "    getRefSource():bool - return source of reference signal: true=internal, false=external\n"
    "    setTrigger(num):nil - set condition of external reference signal:\n"
    "      0 - zero value in sine signal\n"
    "      1 - on rising edge (0->1) in TTL signal\n"
    "      2 - on falling edge (1->0) in TTL signal\n"
    "    getTrigger(nil):num - get trigger condition\n"
    "    setRefPhase(num):nil - set reference signal phase relative to output (in degrees)\n"
    "    getRefPhase():num - get reference signal phase\n"
    //TODO harmonic
    "\n"
    "  SineIn:table\n"
    "    getVoltage():num - get amplitude of input signal (recommended to use this function as link to getX, getY or getR) (in Volts)\n"
    "    setRange(num):nil - set maximum of input signal (in Volts)\n"
    "    getRange():num - get setted maximun\n"
    "    getX():num - get X (real part) of complex input signal (in Volts)\n"
    "    getY():num - get Y (imaginary part) of complex input signal (in Volts)\n"
    "    getR():num - get Amplitude of exponential form of complex input signal (in Volts)\n"
    "    getTetta():num - get Phase of exponential form of complex input signal (in degrees)\n"
    "    setInputMode(num):nil - set input mode:\n"
    "      0 - 'A' (sinonym: setInputA)\n"
    "      1 - 'A-B' (sinonym: setInputAB)\n"
    "      2 - Current input with resistance 1 MOhm (sinonym: setCurrentIn1Mohm)\n"
    "      3 - Current input with resistance 100 MOhm (sinonym: setCurrentIn100Mohm)\n"
    "    getInputMode():num - get setted input mode\n"
    "    setInputA = setInputMode(0)\n"
    "    setInputAB = setInputMode(1)\n"
    "    setCurrentIn1Mohm = setInputMode(2)\n"
    "    setCurrentIn100Mohm = setInputMode(3)\n"
    "    setInputGrounded(bool):nil - use grounded (true) or floating (false) input mode\n"
    "    getInputGrounded():bool - get setted value 'setInputGrounded'\n"
    "    setInputCap(bool):nil - use AC (true) or DC (false) input mode\n"
    "    getInputCap():bool - get setted value 'setInputCap'\n"
    "    setDynamicReserve(num):nil - set dynamic reserve:\n"
    "      0 - High reserve\n"
    "      1 - Normal\n"
    "      2 - Low reserve\n"
    "    getDynamicReserve():num - get setted dynamic reserve\n"
    "    setTimeConst(num):nil - set time constant (in seconds)\n"
    "    getTimeConst():num - get time constant (in seconds)\n"
    "    setSlope(num):nil - set High-pass filter slope:\n"
    "      0 - 6 dB/oct\n"
    "      1 - 12 dB/oct\n"
    "      2 - 18 dB/oct\n"
    "      3 - 24 dB/oct\n"
    "    getSlope():num - get setted slope\n"
    "    bandpassFiltersEnable(bool) - enable or disable bandpass filter\n"
    "    setStateOfSyncFilter(bool):nil - enable or disable synchronous filter below 200 Hz\n"
    "    getStateOfSyncFilter():bool - get status of sync filter\n"
    "\n"
    "  dac[4]: table\n"
    "    setVoltage(num):nil - set voltage on AUX outputs (in Volts)\n"
    "    getVoltage():num - get setted voltage on AUX outputs\n"
    "    setRange(num):nil - set maximum voltage on AUX outputs (in Volts). Dummy function due fixed range (-10.5 ... +10.5 V)\n"
    "\n"
    "  adc[4]: table\n"
    "    getVoltage():num - read voltage from AUX inputs (in Volts)\n"
    "    setRange(num):nil - set maximun input voltage (in Volts). Dummy function due fixed range (-10.5 ... +10.5 V)\n"
    ;
  
  lua_pushstring(L, helpstring);
  return 1;
}

static int L_rawSend(lua_State *L) {
  sr830_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  if(lua_gettop(L) < 2){
    return 0;
  }
  if(!lua_isstring(L, 2)){
    ERROR_LOG("Input data must be 'string'");
    return 0;
  }
  char *cmd = (char*)lua_tostring(L, 2);
  char resp[101];
  ttym_write(device->tty, cmd, strlen(cmd));
  int res = ttym_read(device->tty, resp, 100);
  if(res > 0) resp[res] = 0;
  lua_pushstring(L, resp);
  return 1;
}

static int L_readID(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  
  int n;
  char buff[64];
  n = ttym_write(device->tty, (void*)"*IDN?\r", 6);
  if(n != 6) {
    ERROR_LOG("Error of enable reset");
    return 0;
  }
  int res = ttym_read(device->tty, buff, 63);
  if(res > 0)buff[res]=0;

  lua_pushstring(L, buff);
  return 1;
}

static int L_reset_device(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  
  reset_device(device->tty);
  return 0;
}

static int L_setInputMode(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int mode;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      mode = lua_tonumber(L, 2);
      setInputMode(device->tty, mode);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (integer)");
      lua_pushnil(L);
      return 0;
    }

  } else {
    mode = getInputMode(device->tty);
    lua_pushinteger(L, mode);
    return 1;
  }
}

static int L_setInA(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;

  setInputMode(device->tty, 0);
  return 0;
}

static int L_setInAB(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;

  setInputMode(device->tty, 1);
  return 0;
}

static int L_setInCurr1Mohm(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;

  setInputMode(device->tty, 2);
  return 0;
}

static int L_setInCurr100Mohm(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;

  setInputMode(device->tty, 3);
  return 0;
}

static int L_setPhase(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  double phase;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      phase = lua_tonumber(L, 2);
      setPhase(device->tty, phase);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (number)");
      return 0;
    }
  } else {
    phase = getPhase(device->tty);
    lua_pushnumber(L, phase);
    return 1;
  }
}

static int L_setSens(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  double sens;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      sens = lua_tonumber(L, 2);
      setSens(device->tty, sens);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (number)");
      return 0;
    }
  } else {
    sens = getSens(device->tty);
    lua_pushnumber(L, sens);
    return 1;
  }
}

static int L_setHarmonic(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int harm;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      harm = lua_tonumber(L, 2);
      setHarmonic(device->tty, harm);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (integer)");
      return 0;
    }
  } else {
    harm = getHarmonic(device->tty);
    lua_pushnumber(L, harm);
    return 1;
  }
}

static int L_setFrequency(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  double freq;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      freq = lua_tonumber(L, 2);
      setFrequency(device->tty, freq);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (number)");
      return 0;
    }
  } else {
    freq = getFrequency(device->tty);
    lua_pushnumber(L, freq);
    return 1;
  }
}

static int L_setInputGrounded(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int mode;
  if(lua_gettop(L) == 2) {
    if(lua_isboolean(L, 2)) {
      mode = lua_toboolean(L, 2);
      setInputGrounded(device->tty, mode);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (bool)");
      return 0;
    }
  } else {
    mode = getFrequency(device->tty);
    lua_pushboolean(L, mode);
    return 1;
  }
}

static int L_setInputCap(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int mode;
  if(lua_gettop(L) == 2) {
    if(lua_isboolean(L, 2)) {
      mode = lua_toboolean(L, 2);
      setInputCap(device->tty, mode);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (bool)");
      return 0;
    }
  } else {
    mode = getInputCap(device->tty);
    lua_pushboolean(L, mode);
    return 1;
  }
}

static int L_dynReserve(lua_State *L) {
  sr830_t *device = ReadDevice(L);
  if(device == NULL)return 0;
  int mode;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      mode = lua_tonumber(L, 2);
      setDynReserve(device->tty, mode);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (integer)");
      return 0;
    }
  } else {
    mode = getDynReserve(device->tty);
    lua_pushnumber(L, mode);
    return 1;
  }
}

static int L_refSource(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int mode;
  if(lua_gettop(L) == 2) {
    if(lua_isboolean(L, 2)) {
      mode = lua_toboolean(L, 2);
      setRefSource(device->tty, mode);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (bool)");
      return 0;
    }
  } else {
    mode = getRefSource(device->tty);
    lua_pushboolean(L, mode);
    return 1;
  }
}

static int L_timeConst(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  double time;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      time = lua_tonumber(L, 2);
      setTimeConst(device->tty, time);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (number)");
      return 0;
    }
  } else {
    time = getTimeConst(device->tty);
    lua_pushnumber(L, time);
    return 1;
  }
}

static int L_setSlope(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  double slope;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      slope = lua_tonumber(L, 2);
      setSlope(device->tty, slope);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (number)");
      return 0;
    }
  } else {
    slope = getSlope(device->tty);
    lua_pushnumber(L, slope);
    return 1;
  }
}

static int L_getX(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int n;
  double res;
  char buff[20];
  n = ttym_write(device->tty, (void*)"OUTP? 1\r", 8);
  if(n != 8) {
    ERROR_LOG("");
    return 0;
  }

  n = ttym_read(device->tty, buff, 19);
  if(n > 0) buff[n] = 0;
  res = atof(buff);
  lua_pushnumber(L, res);
  return 1;
}

static int L_getY(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int n;
  double res;
  char buff[20];
  n = ttym_write(device->tty, (void*)"OUTP? 2\r", 8);
  if(n != 8) {
    ERROR_LOG("");
    return 0;
  }

  n = ttym_read(device->tty, buff, 19);
  if(n > 0) buff[n] = 0;
  res = atof(buff);
  lua_pushnumber(L, res);
  return 1;
}

static int L_getR(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int n;
  double res;
  char buff[20];
  n = ttym_write(device->tty, (void*)"OUTP? 3\r", 8);
  if(n != 8) {
    ERROR_LOG("");
    return 0;
  }

  n = ttym_read(device->tty, buff, 19);
  if(n > 0) buff[n] = 0;
  res = atof(buff);
  lua_pushnumber(L, res);
  return 1;
}

static int L_getTetta(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int n;
  double res;
  char buff[20];
  n = ttym_write(device->tty, (void*)"OUTP? 4\r", 8);
  if(n != 8) {
    ERROR_LOG("");
    return 0;
  }

  n = ttym_read(device->tty, buff, 19);
  if(n > 0) buff[n] = 0;
  res = atof(buff);
  lua_pushnumber(L, res);
  return 1;
}

static int L_setAmplitude(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  double volt;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      volt = lua_tonumber(L, 2);
      setAmplitude(device->tty, volt);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (number)");
      return 0;
    }
  } else {
    volt = getAmplitude(device->tty);
    lua_pushnumber(L, volt);
    return 1;
  }
}

static int L_setRefTrig(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int mode;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      mode = lua_tonumber(L, 2);
      setRefTrig(device->tty, mode);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (integer)");
      return 0;
    }
  } else {
    mode = getRefTrig(device->tty);
    lua_pushnumber(L, mode);
    return 1;
  }
}

static int L_setStateOfBandpassFilter(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int filter1, filter2;
  if(lua_gettop(L) == 3) {
    if(lua_isboolean(L,2) && lua_isboolean(L, 3)) {
      filter1 = lua_toboolean(L, 2);
      filter2 = lua_toboolean(L, 3);
      
      filter1 = (((int)!!filter2<<1) | (!!filter1));
      setStateOfBandpassFilter(device->tty, filter1);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (integer)");
      return 0;
    }
  } else {
    filter1 = getStateOfBandpassFilter(device->tty);
    filter2 = !!((1 << 1) & filter1);
    filter1 = !!((1 << 0) & filter1);
    lua_pushboolean(L, filter1);
    lua_pushboolean(L, filter2);
    return 2;
  }
}

static int L_setStateOfSyncFilter(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int state;
  if(lua_gettop(L) == 2) {
    if(lua_isboolean(L, 2)) {
      state = lua_toboolean(L, 2);
      setStateOfSyncFilter(device->tty, state);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (integer)");
      return 0;
    }
  } else {
    state = getStateOfSyncFilter(device->tty);
    lua_pushboolean(L, state);
    return 1;
  }
}

static int L_setVoltOut(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int channel = ReadChannel(L);
  double volt;
  if(lua_gettop(L) == 2) {
    if(lua_isnumber(L, 2)) {
      volt = lua_tonumber(L, 2);
      setAuxOutVolt(device->tty, channel, volt);
      return 0;
    } else {
      ERROR_LOG("This function must contain at least 1 parameter (number)");
      return 0;
    }
  } else {
    volt = getAuxOutVolt(device->tty, channel);
    lua_pushnumber(L, volt);
    return 1;
  }
}

static int L_getVoltIn(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  int channel = ReadChannel(L);
  double volt;
  int n;
  char buff[20];
  sprintf(buff, "OAUX? %i\r", channel);
  n = ttym_write(device->tty, buff, 8);
  if(n != 8) {
    ERROR_LOG("");
    return 0;
  }

  n = ttym_read(device->tty, buff, 19);
  if(n > 0) buff[n] = 0;
  volt = atof(buff);
  lua_pushnumber(L, volt);
  return 1;
}

static int L_getErr(lua_State *L) {
  ERROR_LOG("Function isn't ready");
  return 0;
}

static int L_setRange(lua_State *L) {
  double range = luaL_checknumber(L, -1);
  lua_pop(L, 1);
  if(fabs(range) > 10.5){
    ERROR_LOG("Wrong value of range (+-10.5). Read specification of SR830");
    return 0;
  }
  lua_pushnumber(L, range);
  return 1;
}

static int L_SineSetRange(lua_State *L){
  double range = luaL_checknumber(L, -1);
  lua_pop(L, 1);
  if(range > 5 || range < 0 ){
    ERROR_LOG("Wrong value of range (0 ... +5). Read specification of SR830");
    return 0;
  }
  lua_pushnumber(L, range);
  return 1;
}

static int L_OnDestroy(lua_State *L) {
  sr830_t* device = ReadDevice(L);
  if(device == NULL) return 0;
  ttym_close(device->tty);
  if(device->tty != NULL) device->tty = NULL;
  free(device);
  return 0;
}



static int L_connectNewDevice(lua_State *L) {
  const char* ttypath;
  unsigned int baudrate = 9600;
  if(lua_gettop(L) >= 2) {
    if(lua_isstring(L, 2)) ttypath = (char*)lua_tostring(L, 2);
    else {
      ERROR_LOG("Specify path to COM-port");
      return 0;
    }
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (port name)");
    return 0;
  }
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, 3)) baudrate = lua_tonumber(L, 3);
  }
  sr830_t *device = (sr830_t*)malloc(sizeof(sr830_t));
  if(device == NULL) {
    ERROR_LOG("Connection to device failed.\nTry again!");
    return 0;
  }

  device->tty = ttym_open((char*)ttypath, baudrate);
  if(device->tty == NULL) {
    ERROR_LOG("Can't open tty");
    return 0;
  }

  ttym_timeout(device->tty, 1000);
  
  //set RS232
  {
    int n;
    char buff[20];
    n = ttym_write(device->tty, (void*)"OUTX 0\r", 7);
    if(n != 7) {
      ERROR_LOG("Error of set RS232");
      return 0;
    }
    ttym_read(device->tty, buff, 19);
  }
  
  //init
  reset_device(device->tty);
  clear_registers(device->tty);

  //set permission state registers
  {
    int n;
    char buff[20];
    n = ttym_write(device->tty, (void*)"*ESE 53;LIAE 15;ERRE 214;*SRE 63;\r", 34);
    if(n != 34) {
      ERROR_LOG("Error of set status registers");
      return 0;
    }
    ttym_read(device->tty, buff, 19);
  }
  setInputMode(device->tty, 0);
  setInputGrounded(device->tty, 1);
  setInputCap(device->tty, 0);
  setPhase(device->tty, 0);
  setSens(device->tty, 1);
  setDynReserve(device->tty, 1);
  setRefSource(device->tty, 1);
  setHarmonic(device->tty, 1);
  setFrequency(device->tty, 133);
  setAmplitude(device->tty, 0.004);
  setRefTrig(device->tty, 0);
  setStateOfSyncFilter(device->tty, 0);
  setStateOfBandpassFilter(device->tty, 0);
  setTimeConst(device->tty, 1);
  setSlope(device->tty, 24);

  setSens(device->tty, 1);
  setInputMode(device->tty, 0);
  
  lua_newtable(L);

  lua_getglobal(L, "SR830");
  lua_setfield(L, -2, "SR830");
  lua_setglobal(L, "SR830");
  lua_getglobal(L, "SR830");

    lua_newtable(L);
      lua_pushlightuserdata(L, device);
      lua_setfield(L, -2, "handle");
      lua_pushcfunction(L, L_OnDestroy);
      lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);

    lua_pushcfunction(L, L_rawSend);
    lua_setfield(L, -2, "rawSend");
    lua_pushcfunction(L, L_readID);
    lua_setfield(L, -2, "readID");
    lua_pushcfunction(L, L_reset_device);
    lua_setfield(L, -2, "reset");
    
    lua_newtable(L);
      lua_newtable(L);
        lua_getglobal(L, "SR830");
        lua_setfield(L, -2, "device");
        lua_pushlightuserdata(L, device);
        lua_setfield(L, -2, "handle");
      lua_setmetatable(L, -2);
      //abstract DAC
      lua_pushcfunction(L, L_setAmplitude);
      lua_setfield(L, -2, "setVoltage");
      lua_pushcfunction(L, L_setAmplitude);
      lua_setfield(L, -2, "getVoltage");
      lua_pushcfunction(L, L_SineSetRange);
      lua_setfield(L, -2, "setRange");
      //SRS specific function
      lua_pushcfunction(L, L_setFrequency);
      lua_setfield(L, -2, "setFreq");
      lua_pushcfunction(L, L_setFrequency);
      lua_setfield(L, -2, "getFreq");
      lua_pushcfunction(L, L_setRefTrig);
      lua_setfield(L, -2, "setTrigger");
      lua_pushcfunction(L, L_setRefTrig);
      lua_setfield(L, -2, "getTrigger");
      lua_pushcfunction(L, L_refSource);
      lua_setfield(L, -2, "setIntRefSource");
      lua_pushcfunction(L, L_refSource);
      lua_setfield(L, -2, "getRefSource");
      lua_pushcfunction(L, L_setPhase);
      lua_setfield(L, -2, "setRefPhase");
      lua_pushcfunction(L, L_setPhase);
      lua_setfield(L, -2, "getRefPhase");
      //TODO: разобраться связана ли гармоника с выходным сигналом или все же с входным
      lua_pushcfunction(L, L_setHarmonic);
      lua_setfield(L, -2, "setHarmonic");
      lua_pushcfunction(L, L_setHarmonic);
      lua_setfield(L, -2, "getHarmonic");
    lua_setfield(L, -2, "SineOut");
    
    lua_newtable(L);
      lua_newtable(L);
        lua_getglobal(L, "SR830");
        lua_setfield(L, -2, "device");
        lua_pushlightuserdata(L, device);
        lua_setfield(L, -2, "handle");
      lua_setmetatable(L, -2);
      //Abstract voltmeter
      lua_pushcfunction(L, L_getR);
      lua_setfield(L, -2, "getVoltage"); //sinonym getR
      lua_pushcfunction(L, L_setSens);
      lua_setfield(L, -2, "setRange");
      lua_pushcfunction(L, L_setSens);
      lua_setfield(L, -2, "getRange");
      //SRS
      lua_pushcfunction(L, L_getX);
      lua_setfield(L, -2, "getX");
      lua_pushcfunction(L, L_getY);
      lua_setfield(L, -2, "getY");
      lua_pushcfunction(L, L_getR);
      lua_setfield(L, -2, "getR");
      lua_pushcfunction(L, L_getTetta);
      lua_setfield(L, -2, "getTetta");
      lua_pushcfunction(L, L_setInputMode);
      lua_setfield(L, -2, "getInputMode");
      lua_pushcfunction(L, L_setInputMode);
      lua_setfield(L, -2, "setInputMode");
        lua_pushcfunction(L, L_setInA);
        lua_setfield(L, -2, "setInputA");
        lua_pushcfunction(L, L_setInAB);
        lua_setfield(L, -2, "setInputAB");
        lua_pushcfunction(L, L_setInCurr1Mohm);
        lua_setfield(L, -2, "setCurrentIn1Mohm");//длинное название
        lua_pushcfunction(L, L_setInCurr100Mohm);
        lua_setfield(L, -2, "setCurrentIn100Mohm");//длинное название
      lua_pushcfunction(L, L_setInputGrounded);
      lua_setfield(L, -2, "setInputGrounded");
      lua_pushcfunction(L, L_setInputGrounded);
      lua_setfield(L, -2, "getInputGrounded");
      lua_pushcfunction(L, L_setInputCap);
      lua_setfield(L, -2, "setInputCap");
      lua_pushcfunction(L, L_setInputCap);
      lua_setfield(L, -2, "getInputCap");
      lua_pushcfunction(L, L_dynReserve);
      lua_setfield(L, -2, "setDynamicReserve");
      lua_pushcfunction(L, L_dynReserve);
      lua_setfield(L, -2, "getDynamicReserve");
      lua_pushcfunction(L, L_timeConst);
      lua_setfield(L, -2, "setTimeConst");
      lua_pushcfunction(L, L_timeConst);
      lua_setfield(L, -2, "getTimeConst");
      lua_pushcfunction(L, L_setSlope);
      lua_setfield(L, -2, "setSlope");
      lua_pushcfunction(L, L_setSlope);
      lua_setfield(L, -2, "getSlope");
      lua_pushcfunction(L, L_setStateOfBandpassFilter);
      lua_setfield(L, -2, "bandpassFiltersEnable");
      lua_pushcfunction(L, L_setStateOfSyncFilter);
      lua_setfield(L, -2, "setStateOfSyncFilter");
      lua_pushcfunction(L, L_setStateOfSyncFilter);
      lua_setfield(L, -2, "getStateOfSyncFilter");
    lua_setfield(L, -2, "SineIn");

    lua_newtable(L);
      lua_getglobal(L, "SR830");
      lua_setfield(L, -2, "device");
      for(int i = 1; i <= 4; i++) {
        lua_newtable(L);
          lua_newtable(L);
            lua_pushlightuserdata(L, device);
            lua_setfield(L, -2, "handle");
            lua_pushnumber(L, i);
            lua_setfield(L, -2, "channel");
            lua_getglobal(L, "SR830");
            lua_setfield(L, -2, "device");
          lua_setmetatable(L, -2);

          lua_pushcfunction(L, L_setVoltOut);
          lua_setfield(L, -2, "setVoltage");
          lua_pushcfunction(L, L_setVoltOut);
          lua_setfield(L, -2, "getVoltage");
          lua_pushcfunction(L, L_setRange);
          lua_setfield(L, -2, "setRange");
        lua_rawseti(L, -2, i);
      }
    lua_setfield(L, -2, "dac");
    

    lua_newtable(L);
      lua_getglobal(L, "SR830");
      lua_setfield(L, -2, "device");
      for(int i = 1; i <= 4; i++) {
        lua_newtable(L);
          lua_newtable(L);
            lua_pushlightuserdata(L, device);
            lua_setfield(L, -2, "handle");
            lua_pushnumber(L, i);
            lua_setfield(L, -2, "channel");
            lua_getglobal(L, "SR830");
            lua_setfield(L, -2, "device");
          lua_setmetatable(L, -2);

          lua_pushcfunction(L, L_getVoltIn);
          lua_setfield(L, -2, "getVoltage");
          lua_pushcfunction(L, L_setRange);
          lua_setfield(L, -2, "setRange");
        lua_rawseti(L, -2, i);
      }
    lua_setfield(L, -2, "adc");

    lua_getfield(L, -1, "SR830");
    lua_setglobal(L, "SR830");
    lua_pushnil(L);
    lua_setfield(L, -2, "SR830");

#if 0 //Not realized yet
    lua_pushcfunction(L, L_getErr);
    lua_setfield(L, -2, "setDisplaySettings");
    lua_pushcfunction(L, L_getErr);
    lua_setfield(L, -2, "getDisplaySettings");
    lua_pushcfunction(L, L_getErr);
    lua_setfield(L, -2, "setOutSource");
    lua_pushcfunction(L, L_getErr);
    lua_setfield(L, -2, "getOutSource");
    lua_pushcfunction(L, L_getErr);
    lua_setfield(L, -2, "setOffsetGain");
    lua_pushcfunction(L, L_getErr);
    lua_setfield(L, -2, "getOffsetGain");
#endif

  return 1;
}

int luaopen_sr830(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");

    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");

  return 1;
}
