#include <iostream>
#include <lua5.2/lua.hpp>
#include "SR570.hpp"

static int L_help(lua_State *L){
  const char helpstring[] =
                    "Module for working with low noise current preamplifier SR570\n"
                    "  connectNewDevice(path, [baudrate]):table - connecting to a given 'path' with rate a 'baudrate'\n"
                    "  reset():nil - resets the amplifier to the default settings\n"
                    "  setSens(nsens):nil - sets the sensitivity of the amplifier. See table below\n"
                    "  \t  nsens        scale\n"
                    "  \t0, 1, 2     1, 2, 5 pA/V\n"
                    "  \t3, 4, 5     10, 20, 50 pA/V\n"
                    "  \t6, 7, 8     100, 200, 500 pA/V\n"
                    "  \t9, 10, 11   1, 2, 5 nA/V\n"
                    "  \t12, 13, 14  10, 20, 50 nA/V\n"
                    "  \t15, 16, 17  100, 200, 500 nA/V\n"
                    "  \t18, 19, 20  1, 2, 5 uA/V\n"
                    "  \t21, 22, 23  10, 20, 50 uA/V\n"
                    "  \t24, 25, 26  100, 200, 500 uA/V\n"
                    "  \t27          1 mA/V\n\n"
                    "  turnInOffsetCurr(value):nil - turn the input offset current on (value=1) or off (value=0)\n"
                    "  setCalOffsetCurrentLVL(ncurr):nil - sets the calibrated input offset current level. See table below\n"
                    "  \t ncurr         scale\n"
                    "  \t0, 1, 2     1, 2, 5 pA\n"
                    "  \t3, 4, 5     10, 20, 50 pA\n"
                    "  \t6, 7, 8     100, 200, 500 pA\n"
                    "  \t9, 10, 11   1, 2, 5 nA\n"
                    "  \t12, 13, 14  10, 20, 50 nA\n"
                    "  \t15, 16, 17  100, 200, 500 nA\n"
                    "  \t18, 19, 20  1, 2, 5 uA\n"
                    "  \t21, 22, 23  10, 20, 50 uA\n"
                    "  \t24, 25, 26  100, 200, 500 uA\n"
                    "  \t27, 28, 29  1, 2, 5 mA\n\n"
                    "  setTypeFilter(ntype):nil - sets the filter type. See table below\n"
                    "  \tntype   filter type\n"
                    "  \t  0    6 dB highpass\n"
                    "  \t  1    12 dB highpass\n"
                    "  \t  2    6 dB bandpass\n"
                    "  \t  3    6 dB lowpass\n"
                    "  \t  4    12 dB lowpass\n"
                    "  \t  5    none\n\n"
                    "  setHighFilter(nfreq):nil - sets the value of the highpass filter 3dB point. nfreq ranges from 0 (0.03Hz) to 11 (10 kHz). See table below\n"
                    "  \tnfreq   filter frequency\n"
                    "  \t  0      0.03 Hz\n"
                    "  \t  1, 2   0.1, 0.3 Hz\n"
                    "  \t  3, 4   1, 3 Hz\n"
                    "  \t  5, 6   10, 30 Hz\n"
                    "  \t  7, 8   100, 300 Hz\n"
                    "  \t  9, 10  1, 3 kHz\n"
                    "  \t 11, 12  10, 30 kHz\n"
                    "  \t 13, 14  100, 300 kHz\n"
                    "  \t 15      1 MHz\n\n"
                    "  setLowFilter(nfreq):nil - sets the value of the lowpass filter 3dB point. nfreq ranges from 0 (0.03Hz) to 15 (1 MHz). See table below function 'setHighFilter'\n"
                    "  setUncalInOffsetVernier(nscale):nil - Sets the uncalibrated input offset vernier. [-1000 ≤ nscale ≤ +1000] (0 - ±100.0% of full scale)\n"
                    "  setSenCal(calmode):nil - sets the sensitivity cal mode. 0 = cal, 1 = uncal\n"
                    "  setInOffsetCurrSign(sign):nil - sets the input offset current sign. 0 = neg, 1 = pos\n"
                    "  setInOffsetCalMode(calmode):nil - sets the input offset cal mode. 0 = cal, 1 = uncal\n"
                    "  turnBiasVolt(value):nil - turn the bias voltage on (value=1) or off(value=0)\n"
                    "  setSigInvertSense(mode):nil - sets the signal invert sense. 0=non- inverted, 1=inverted\n"
                    "  setBlanksOutAmplifier(mode):nil - blanks the front-end output of the amplifier. 0=no blank, 1=blank\n"
                    "  setUncalSensVernier(nscale):nil - sets the uncalibrated sensitivity vernier. [0 ≤ nscale ≤ 100] (percent of full scale)\n"
                    "  setBiasVoltLVL(nLevel):nil - sets the bias voltage level in the range. [-5000 ≤ nLevel ≤ +5000] (-5.000V to +5.000 V)\n"
                    "  setGainMode(mode):nil -  sets the gain mode of the amplifier. See table below\n"
                    "  \tmode    gain mode\n"
                    "  \t 0     low Noise\n"
                    "  \t 1     high Bandwidth\n"
                    "  \t 2     low Drift\n\n"
                    "  resetFilCap():nil - resets the filter capacitors to clear an overload condition\n"
                    ;
  lua_pushstring(L, helpstring);
  return 1;
}

static int L_setSens(lua_State *L) {
  int sens;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) sens = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (sensitivity)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setSens(sens);
  
  return 0;
}

static int L_reset(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->reset();

  return 0;
}

static int L_setCalOffsetCurrentLVL(lua_State *L) {
  int curr;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) curr = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (current)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setCalOffsetCurrentLVL(curr);

  return 0;
}

static int L_setHighFilter(lua_State *L) {
  float freqFilter;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) freqFilter = lua_tonumber(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (freq)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setHighFilter(freqFilter);

  return 0;
}

static int L_setLowFilter(lua_State *L) {
  float freqFilter;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) freqFilter = lua_tonumber(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (freq)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setLowFilter(freqFilter);

  return 0;
}

static int L_setTypeFilter(lua_State *L) {
  int nType;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) nType = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (ntype)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setTypeFilter(nType);

  return 0;
}

static int L_setUncalInOffsetVernier(lua_State *L) {
  int scale;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) scale = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (nscale)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setUncalInOffsetVernier(scale);

  return 0;
}

static int L_setInOffsetCalMode(lua_State *L) {
  int calMode;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) calMode = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (calmode)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setInOffsetCalMode(calMode);

  return 0;
}

static int L_turnInOffsetCurr(lua_State *L) {
  int val;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) val = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (value)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->turnInOffsetCurr(val);

  return 0;
}

static int L_turnBiasVolt(lua_State *L) {
  int val;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) val = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (value)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->turnBiasVolt(val);

  return 0;
}

static int L_setSenCal(lua_State *L) {
  int calMode;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) calMode = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (calmode)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setSenCal(calMode);

  return 0;
}

static int L_setInOffsetCurrSign(lua_State *L) {
  int sign;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) sign = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (sign)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setInOffsetCurrSign(sign);

  return 0;
}

static int L_setSigInvertSense(lua_State *L) {
  int mode;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) mode = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (mode)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setSigInvertSense(mode);

  return 0;
}

static int L_setUncalSensVernier(lua_State *L) {
  int scale;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) scale = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (nscale)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setUncalSensVernier(scale);

  return 0;
}

static int L_setBiasVoltLVL(lua_State *L) {
  int nLevel;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) nLevel = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (nlevel)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setBiasVoltLVL(nLevel);

  return 0;
}

static int L_setGainMode(lua_State *L) {
  int mode;
  if(lua_gettop(L) >= 2) {
    if(lua_isnumber(L, 2)) mode = lua_tointeger(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (mode)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setGainMode(mode);

  return 0;
}

static int L_resetFilCap(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->resetFilCap();

  return 0;
}

static int L_disconnect(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR570_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR570 *sr = (SR570*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->closePort();

  delete sr;
  lua_pushlightuserdata(L, nullptr);
  lua_setfield(L, 1, "_objSR570_");

  return 1;
}

static int L_connectNewDevice(lua_State *L) {
  char *portname = nullptr;
  int baud = 9600, checkError = 0;
  if(lua_gettop(L) >= 2) {
    if(lua_isstring(L, 2)) portname = (char*)lua_tostring(L, 2);
  } else {
    ERROR_LOG("This function must contain at least 1 parameter (port name)");
    return 0;
  }
  if(lua_gettop(L) >= 3) {
    if(lua_isnumber(L, 3)) baud = lua_tonumber(L, 3);
  }

  SR570 *sr;
  try {
    sr = new SR570;
  } catch (std::bad_alloc &ba) {
    ERROR_LOG(ba.what());
    return 0;
  }

  checkError = sr->connect(portname, baud);

  if(checkError == 0) {
    ERROR_LOG("Port isn't open");
    delete sr;
    return 0;
  }

  lua_newtable(L);
    lua_newtable(L);
      lua_pushstring(L, "_objSR570_");
      lua_pushlightuserdata(L, sr);
      lua_rawset(L, -3);
      
      lua_pushstring(L, "_gcc");
      lua_pushcfunction(L, L_disconnect);
      lua_rawset(L, -3);
    lua_setmetatable(L, -2);

    lua_pushstring(L, "setSens");
    lua_pushcfunction(L, L_setSens);
    lua_rawset(L, -3);

    lua_pushstring(L, "reset");
    lua_pushcfunction(L, L_reset);
    lua_rawset(L, -3);

    lua_pushstring(L, "setCalOffsetCurrentLVL");
    lua_pushcfunction(L, L_setCalOffsetCurrentLVL);
    lua_rawset(L, -3);

    lua_pushstring(L, "setHighFilter");
    lua_pushcfunction(L, L_setHighFilter);
    lua_rawset(L, -3);

    lua_pushstring(L, "setLowFilter");
    lua_pushcfunction(L, L_setLowFilter);
    lua_rawset(L, -3);

    lua_pushstring(L, "setTypeFilter");
    lua_pushcfunction(L, L_setTypeFilter);
    lua_rawset(L, -3);

    lua_pushstring(L, "setUncalInOffsetVernier");
    lua_pushcfunction(L, L_setUncalInOffsetVernier);
    lua_rawset(L, -3);

    lua_pushstring(L, "setInOffsetCalMode");
    lua_pushcfunction(L, L_setInOffsetCalMode);
    lua_rawset(L, -3);

    lua_pushstring(L, "turnInOffsetCurr");
    lua_pushcfunction(L, L_turnInOffsetCurr);
    lua_rawset(L, -3);

    lua_pushstring(L, "turnBiasVolt");
    lua_pushcfunction(L, L_turnBiasVolt);
    lua_rawset(L, -3);

    lua_pushstring(L, "setSenCal");
    lua_pushcfunction(L, L_setSenCal);
    lua_rawset(L, -3);

    lua_pushstring(L, "setInOffsetCurrSign");
    lua_pushcfunction(L, L_setInOffsetCurrSign);
    lua_rawset(L, -3);

    lua_pushstring(L, "setSigInvertSense");
    lua_pushcfunction(L, L_setSigInvertSense);
    lua_rawset(L, -3);

    lua_pushstring(L, "setUncalSensVernier");
    lua_pushcfunction(L, L_setUncalSensVernier);
    lua_rawset(L, -3);

    lua_pushstring(L, "setBiasVoltLVL");
    lua_pushcfunction(L, L_setBiasVoltLVL);
    lua_rawset(L, -3);

    lua_pushstring(L, "setGainMode");
    lua_pushcfunction(L, L_setGainMode);
    lua_rawset(L, -3);

    lua_pushstring(L, "resetFilCap");
    lua_pushcfunction(L, L_resetFilCap);
    lua_rawset(L, -3);

  return 1;
}

extern "C" int luaopen_sr570(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");

    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  
  return 1;
}