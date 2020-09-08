#include <iostream>
#include <lua5.2/lua.hpp>
#include "SR830.hpp"

static int L_help(lua_State *L) {
  const char helpstring[] =
      "SR830\n"
      "  help():string - return help string\n"
      "  connectNewDevice(path, [baudrate]):table - connect to SR830. The baud rate from 300 to 19200 baud (Default is 9600)\n"
      "  setRS232():nil - set up work with RS232 interface\n"
      "  reset():nil - resets the SR830 to its default configurations\n"
      "  readID():string - returns the SR830's device identification string\n"
      "  clrRegs():nil - clears all status registers\n"
      "  setPermStatRegs(rese, rliae, rerre, rsre):nil - setup some status registers(read manual from 5-20 to 5-23)\n"
      "  setInConfig(inConf):nil - sets the input configuration\n"
      "  setInGndShield(gndval):nil - sets the input shield grounding\n"
      "  setInCoupling(coupling):nil - sets the input coupling\n"
      "  setPhase(phase):nil - sets the reference phase shift\n"
      "  setSens(sens):nil -  sets the sensitivity\n"
      "  setReserveMode(mode):nil - sets the reserve mode\n"
      "  setRefSource(mode):nil - sets the reference source\n"
      "  setDetHarm(harm):nil - sets the detection harmonic\n"
      "  setRefFreq(freq):nil - sets the reference frequency\n"
      "  setAmplSinOut(volt):nil - sets the amplitude of the sine output\n"
      "  setRefTrig(mode):nil - sets the reference trigger when using the external reference mode\n"
      "  setNotchFiltStatus(status):nil - sets the input line notch filter status\n"
      "  setSyncFiltStatus(status):nil - sets the synchronous filter status\n"
      "  setTimeConst(time):nil - sets the time constan\n"
      "  setFiltSlope(slope):nil - sets the low pass filter slope\n"
      "  setDisplaySettings(nchannel, ndisplay, ratio):nil - selects the CH1 and CH2 displays\n"
      "  setOutSource(nchannel, outQuan):nil - sets the front panel (CH1 and CH2) output sources\n"
      "  setOffsetGain(nchannel, perOffset, nExpand):nil - sets the output offsets and expands\n"
      "  setOffsetGainX(perOffset, nExpand):nil - sets the X output offsets and expands\n"
      "  setOffsetGainY(perOffset, nExpand):nil - sets the Y output offsets and expands\n"
      "  setAuxOutVolt(nOutput, volt):nil - sets the Aux Output voltage when the output\n"
      "  setZeroAOV():nil - sets all(4) Aux output at 0\n"
      "  getPhase():number - queries the reference phase shift\n"
      "  getSens():number -  queries the sensitivity\n"
      "  getReserveMode():number - queries the reserve mode\n"
      "  getRefSource():bool - queries the reference source\n"
      "  getDetHarm():number - queries the detection harmonic\n"
      "  getRefFreq():number - queries the reference frequency\n"
      "  getAmplSinOut():number - queries the amplitude of the sine output\n"
      "  getRefTrig():number - queries the reference trigger when using the external reference mode\n"
      "  getNotchFiltStatus():number - queries the input line notch filter status\n"
      "  getSyncFiltStatus():bool - queries the synchronous filter status\n"
      "  getTimeConst():number - queries the time constan\n"
      "  getFiltSlope():number - queries the low pass filter slope\n"
      "  getOffsetGain(nchannel):number,number - \n"
      "  getAuxOutVolt(nOutput):number - queries the output offsets and expands\n"
      "  getX():number - read the value of X\n"
      "  getY():number - read the value of Y\n"
      "  getR():number - read the value of R\n"
      "  getTetta():number - read the value of tetta\n"
      ;
  lua_pushstring(L, helpstring);
  return 1;
}

static int L_setRS232(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setRS232();

  return 0;
}

static int L_clrRegs(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->clrRegs();

  return 0;
}

static int L_reset(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->reset();

  return 0;
}

static int L_readID(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  char* res;
  res = sr->readID();

  lua_pushstring(L, res);
  return 1;
}

static int L_setPermStatReg (lua_State *L) {
  int rese = 0, rliae = 0, rerre = 0, rsre = 0;
  if(lua_gettop(L) >= 5) {
    if(lua_isnumber(L, 2)) rese = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3)) rliae = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4)) rerre = lua_tonumber(L, 4);
    if(lua_isnumber(L, 5)) rsre = lua_tonumber(L, 5);
  } else {
    ERROR_LOG("This function must contain at least 4 parameters");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setPermStatRegs(rese, rliae, rerre, rsre);
  
  return 0;
}

static int L_setInConfig(lua_State *L) {
  int cfg_num = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) cfg_num = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (cfg_num)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setInConfig(cfg_num);

  return 0;
}

static int L_setInGndShield(lua_State *L) {
  bool gndval = true;
  if(lua_gettop(L) >= 2 && lua_isboolean(L, 2)) gndval = lua_toboolean(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (gndval)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setInGndShield(gndval);

  return 0;
}

static int L_setInCoupling(lua_State *L) {
  bool coupling = true;
  if(lua_gettop(L) >= 2 && lua_isboolean(L, 2)) coupling = lua_toboolean(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (coupling)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setInCoupling(coupling);

  return 0;
}

static int L_setPhase(lua_State *L) {
  double phase = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) phase = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (phase)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setPhase(phase);

  return 0;
}

static int L_setSens(lua_State *L) {
  double sens = 1;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) sens = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (sens)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setSens(sens);

  return 0;
}

static int L_setReserveMode(lua_State *L) {
  int mode = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) mode = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (mode)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setReserveMode(mode);

  return 0;
}

static int L_setRefSource(lua_State *L) {
  bool mode = false;
  if(lua_gettop(L) >= 2 && lua_isboolean(L, 2)) mode = lua_toboolean(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (mode)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setRefSource(mode);
  
  return 0;
}

static int L_setDetHarm(lua_State *L) {
  int harm = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) harm = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (harm)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr-> setDetHarm(harm);

  return 0;
}

static int L_setRefFreq(lua_State *L) {
  double freq = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) freq = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (freq)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setRefFreq(freq);

  return 0;
}

static int L_setAmplSinOut(lua_State *L) {
  double volt = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) volt = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (volt)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setAmplSinOut(volt);

  return 0;
}

static int L_setRefTrig(lua_State *L) {
  int mode = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) mode = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (mode)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setRefTrig(mode);

  return 0;
}

static int L_setNotchFiltStatus(lua_State *L) {
  int status = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) status = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (status)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setNotchFiltStatus(status);

  return 0;
}

static int L_setSyncFiltStatus(lua_State *L) {
  bool status = 0;
  if(lua_gettop(L) >= 2 && lua_isboolean(L, 2)) status = lua_toboolean(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (status)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setSyncFiltStatus(status);

  return 0;
}

static int L_setTimeConst(lua_State *L) {
  double time = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) time = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (time)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setTimeConst(time);

  return 0;
}

static int L_setFiltSlope(lua_State *L) {
  double slope = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) slope = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (slope)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setFiltSlope(slope);

  return 0;
}

static int L_setDisplaySettings(lua_State *L) {
  int nchannel = 0, ndisplay = 0, ratio = 0;
  if(lua_gettop(L) >= 4) { 
    if(lua_isnumber(L, 2)) nchannel = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3)) ndisplay = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4)) ratio = lua_tonumber(L, 4);
  } else {
    ERROR_LOG("This function must contain at least 3 parameter (nchannel, ndisplay, ratio)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setDisplaySettings(nchannel, ndisplay, ratio);

  return 0;
}

static int L_setOutSource(lua_State *L) {
  int nchannel = 0, outQuan = 0;
  if(lua_gettop(L) >= 3) { 
    if(lua_isnumber(L, 2)) nchannel = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3)) outQuan = lua_tonumber(L, 3);
  } else {
    ERROR_LOG("This function must contain at least 2 parameter (nchannel, outQuan)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setOutSource(nchannel, outQuan);

  return 0;
}

static int L_setOffsetGain(lua_State *L) {
  int nchannel = 0, perOffset = 0, nExpand = 0;
  if(lua_gettop(L) >= 4) { 
    if(lua_isnumber(L, 2)) nchannel = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3)) perOffset = lua_tonumber(L, 3);
    if(lua_isnumber(L, 4)) nExpand = lua_tonumber(L, 4);
  } else {
    ERROR_LOG("This function must contain at least 3 parameter (nchannel, perOffset, nExpand)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setOffsetGain(nchannel, perOffset, nExpand);

  return 0;
}

static int L_setOffsetGainX(lua_State *L) {
  int perOffset = 0, nExpand = 0;
  if(lua_gettop(L) >= 3) { 
    if(lua_isnumber(L, 2)) perOffset = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3)) nExpand = lua_tonumber(L, 3);
  } else {
    ERROR_LOG("This function must contain at least 2 parameter (perOffset, nExpand)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setOffsetGainX(perOffset, nExpand);

  return 0;
}

static int L_setOffsetGainY(lua_State *L) {
  int perOffset = 0, nExpand = 0;
  if(lua_gettop(L) >= 3) { 
    if(lua_isnumber(L, 2)) perOffset = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3)) nExpand = lua_tonumber(L, 3);
  } else {
    ERROR_LOG("This function must contain at least 2 parameter (perOffset, nExpand)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setOffsetGainY(perOffset, nExpand);

  return 0;
}

static int L_setAuxOutVolt(lua_State *L) {
  int nOutput = 0; double volt = 0;
  if(lua_gettop(L) >= 3) { 
    if(lua_isnumber(L, 2)) nOutput = lua_tonumber(L, 2);
    if(lua_isnumber(L, 3)) volt = lua_tonumber(L, 3);
  } else {
    ERROR_LOG("This function must contain at least 2 parameter (nOutput, volt)");
    return 0;
  }
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setAuxOutVolt(nOutput, volt);

  return 0;
}

static int L_setZeroAOV(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->setZeroAOV();

  return 0;
}

static int L_getPhase(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getPhase();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getSens(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getSens();
  
  lua_pushnumber(L, res);
  return 1;
}

static int L_getReserveMode(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getReserveMode();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getRefSource(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getRefSource();

  lua_pushboolean(L, res);
  return 1;
}

static int L_getDetHarm(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getDetHarm();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getRefFreq(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getRefFreq();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getAmplSinOut(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getAmplSinOut();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getRefTrig(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getRefTrig();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getNotchFiltStatus(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getNotchFiltStatus();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getSyncFiltStatus(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getSyncFiltStatus();

  lua_pushboolean(L, res);
  return 1;
}

static int L_getTimeConst(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getTimeConst();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getFiltSlope(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getFiltSlope();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getOffsetGain(lua_State *L) {
  int nchannel = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) nchannel = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (nchannel)");
    return 0;
  }
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getOffsetGain(nchannel);

  lua_pushnumber(L, res.real());
  lua_pushnumber(L, res.imag());
  return 2;
}

static int L_getAuxOutVolt(lua_State *L) {
  int nOutput = 0;
  if(lua_gettop(L) >= 2 && lua_isnumber(L, 2)) nOutput = lua_tonumber(L, 2);
  else {
    ERROR_LOG("This function must contain at least 1 parameter (nOutput)");
    return 0;
  }

  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getAuxOutVolt(nOutput);

  lua_pushnumber(L, res);
  return 1;
}

static int L_getX(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getX();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getY(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getY();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getR(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getR();

  lua_pushnumber(L, res);
  return 1;
}

static int L_getTetta(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)){
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)){
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  auto res = sr->getTetta();

  lua_pushnumber(L, res);
  return 1;
}

static int L_disconnect(lua_State *L) {
  lua_getmetatable(L, 1);
  if(!lua_istable(L, -1)) {
    ERROR_LOG("Not metatable!");
    return 0;
  }

  lua_getfield(L, -1, "_objSR830_");
  if(!lua_islightuserdata(L, -1)) {
    ERROR_LOG("Not userdata!");
    return 0;
  }

  SR830 *sr = (SR830*)lua_touserdata(L, -1);
  if(sr == NULL) {
    ERROR_LOG("Call 'connectNewDevice' before using anything functions");
    return 0;
  }
  sr->close();

  delete sr;
  lua_pushlightuserdata(L, nullptr);
  lua_setfield(L, 1, "_objSR830_");

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

  SR830 *sr;
  try {
    sr = new SR830;
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
      lua_pushstring(L, "_objSR830_");
      lua_pushlightuserdata(L, sr);
      lua_rawset(L, -3);
      
      lua_pushstring(L, "_gcc");
      lua_pushcfunction(L, L_disconnect);
      lua_rawset(L, -3);
    lua_setmetatable(L, -2);

    lua_pushstring(L, "setRS232");
    lua_pushcfunction(L, L_setRS232);
    lua_rawset(L, -3);

    lua_pushstring(L, "clrRegs");
    lua_pushcfunction(L, L_clrRegs);
    lua_rawset(L, -3);

    lua_pushstring(L, "reset");
    lua_pushcfunction(L, L_reset);
    lua_rawset(L, -3);

    lua_pushstring(L, "readID");
    lua_pushcfunction(L, L_readID);
    lua_rawset(L, -3);

    lua_pushstring(L, "setPermStatusRegs");
    lua_pushcfunction(L, L_setPermStatReg);
    lua_rawset(L, -3);

    lua_pushstring(L, "setInConfig");
    lua_pushcfunction(L, L_setInConfig);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setInGndShield");
    lua_pushcfunction(L, L_setInGndShield);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setInCoupling");
    lua_pushcfunction(L, L_setInCoupling);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setPhase");
    lua_pushcfunction(L, L_setPhase);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setSens");
    lua_pushcfunction(L, L_setSens);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setReserveMode");
    lua_pushcfunction(L, L_setReserveMode);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setRefSource");
    lua_pushcfunction(L, L_setRefSource);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setDetHarm");
    lua_pushcfunction(L, L_setDetHarm);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setRefFreq");
    lua_pushcfunction(L, L_setRefFreq);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setAmplSinOut");
    lua_pushcfunction(L, L_setAmplSinOut);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setRefTrig");
    lua_pushcfunction(L, L_setRefTrig);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setNotchFiltStatus");
    lua_pushcfunction(L, L_setNotchFiltStatus);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setSyncFiltStatus");
    lua_pushcfunction(L, L_setSyncFiltStatus);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setTimeConst");
    lua_pushcfunction(L, L_setTimeConst);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setFiltSlope");
    lua_pushcfunction(L, L_setFiltSlope);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setDisplaySettings");
    lua_pushcfunction(L, L_setDisplaySettings);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setOutSource");
    lua_pushcfunction(L, L_setOutSource);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setOffsetGain");
    lua_pushcfunction(L, L_setOffsetGain);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setOffsetGainX");
    lua_pushcfunction(L, L_setOffsetGainX);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setOffsetGainY");
    lua_pushcfunction(L, L_setOffsetGainY);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setAuxOutVolt");
    lua_pushcfunction(L, L_setAuxOutVolt);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "setZeroAOV");
    lua_pushcfunction(L, L_setZeroAOV);
    lua_rawset(L, -3);
    
    lua_pushstring(L, "getPhase");
    lua_pushcfunction(L, L_getPhase);
    lua_rawset(L, -3);

    lua_pushstring(L, "getSens");
    lua_pushcfunction(L, L_getSens);
    lua_rawset(L, -3);

    lua_pushstring(L, "getReserveMode");
    lua_pushcfunction(L, L_getReserveMode);
    lua_rawset(L, -3);

    lua_pushstring(L, "getRefSource");
    lua_pushcfunction(L, L_getRefSource);
    lua_rawset(L, -3);

    lua_pushstring(L, "getDetHarm");
    lua_pushcfunction(L, L_getDetHarm);
    lua_rawset(L, -3);

    lua_pushstring(L, "getRefFreq");
    lua_pushcfunction(L, L_getRefFreq);
    lua_rawset(L, -3);

    lua_pushstring(L, "getAmplSinOut");
    lua_pushcfunction(L, L_getAmplSinOut);
    lua_rawset(L, -3);

    lua_pushstring(L, "getRefTrig");
    lua_pushcfunction(L, L_getRefTrig);
    lua_rawset(L, -3);

    lua_pushstring(L, "getNotchFiltStatus");
    lua_pushcfunction(L, L_getNotchFiltStatus);
    lua_rawset(L, -3);

    lua_pushstring(L, "getSyncFiltStatus");
    lua_pushcfunction(L, L_getSyncFiltStatus);
    lua_rawset(L, -3);

    lua_pushstring(L, "getTimeConst");
    lua_pushcfunction(L, L_getTimeConst);
    lua_rawset(L, -3);

    lua_pushstring(L, "getFiltSlope");
    lua_pushcfunction(L, L_getFiltSlope);
    lua_rawset(L, -3);

    lua_pushstring(L, "getOffsetGain");
    lua_pushcfunction(L, L_getOffsetGain);
    lua_rawset(L, -3);

    lua_pushstring(L, "getAuxOutVolt");
    lua_pushcfunction(L, L_getAuxOutVolt);
    lua_rawset(L, -3);

    lua_pushstring(L, "getX");
    lua_pushcfunction(L, L_getX);
    lua_rawset(L, -3);

    lua_pushstring(L, "getY");
    lua_pushcfunction(L, L_getY);
    lua_rawset(L, -3);

    lua_pushstring(L, "getR");
    lua_pushcfunction(L, L_getR);
    lua_rawset(L, -3);

    lua_pushstring(L, "getTetta");
    lua_pushcfunction(L, L_getTetta);
    lua_rawset(L, -3);



    /*Template
    lua_pushstring(L, "name");
    lua_pushcfunction(L, name);
    lua_rawset(L, -3);
    */



  return 1;
}

extern "C" int luaopen_sr830(lua_State *L) {
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");

    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  
  return 1;
}
