//TODO возвращающее значение функций get привести к числам.

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <string.h>
#include "SR830.hpp"

SR830::SR830() {
  tty = NULL;
}

SR830::~SR830() {
  if(tty)ttym_close(tty);
}

int SR830::connect(char portName[], int baud) {
  char buff[256];
  tty = ttym_open(portName, baud);
  if(tty == NULL) {
    ERROR_LOG("No connect!\n");
    return 0;
  }
  ttym_timeout(tty, 1000);
    
  setRS232();
  clrRegs();   
  
  setPermStatReg(53, 15, 214, 63);    
  setInSettings(0, 1, 0);
  startManualSetting(0, 23, 1);

  setPhase(0);
  setSens(1);
  setReserveMode(1);

  refSetting(1, 1, 133, 0.004,  0);
  filtSetting(0, 0, 10, 3);
  setDisplaySettings(1, 0, 0);
  setOutSource(1, 1);
  setDisplaySettings(2, 0, 0);
  setOutSource(2, 1);
  setOffsetGainX(0, 0);
  setOffsetGainY(0, 0);
  setZeroAOV();
  
  ttym_read(tty,buff,200);
  return 1;
}

void SR830::setRS232() {
  int n;
  char buff[20];
  n = ttym_write(tty, (void*)"OUTX 0\r", 7);
  if(!n) {
    ERROR_LOG("Error of set RS232");
    return;
  }
  ttym_read(tty, buff, 200);
  while(!n) {
    ttym_write(tty, (void*)"*STB?\r", 6);
    n = ttym_read(tty, buff, 200);
  }
}

void SR830::reset() {
  int n;
  n = ttym_write(tty, (void*)"*RST\r", 5);
  if(!n) {
    ERROR_LOG("Error of enable reset");
    return;
  }
}

char* SR830::readID() {
  char *buff = new char[256];
  int n;
  n = ttym_write(tty, (void*)"*IDN?\r", 6);
  if(!n) {
    ERROR_LOG("Error of read ID");
    return (char*)"ID:-1";
  }
  ttym_read(tty, buff, 200);
  return buff;
}

void SR830::clrRegs() {
  int n;
  n = ttym_write(tty, (void*)"*CLS\r",6);
  if(!n) {
    ERROR_LOG("Error of clear status bytes");
    return;
  }
}

void SR830::setPermStatReg(int stdEventB, int liaB, int errB, int serialPollB) {
  int n;
  char buff[64];
  sprintf(buff, "*ESE %d;LIAE %d;ERRE %d;*SRE %d;\r", stdEventB, liaB, errB, serialPollB);
  
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Error of set status registers");
    return;
  }
}

void SR830::setInSettings(int inConfig, int inShGND, int inCoupl) {
  int n;
  char buff[64];
  sprintf(buff, "ISRC %d;IGND %d;ICPL %d;\r", inConfig, inShGND, inCoupl);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Error of set incoming settings");
    return;
  }
}

void SR830::setPhase(double phase) {
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
}

void SR830::setSens(double sens) {
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
    FP_NAN
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
}

void SR830::setReserveMode(int mode) {
  if(mode < 0 || mode > 2) {
    ERROR_LOG("Wrong value of mode");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "RMOD %d\r", mode);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Reverse mode set error");
    return;
  }
}

void SR830::setRefSource(bool mode) {
  int n;
  char buff[20];
  sprintf(buff, "FMOD %d\r", mode);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Reference source set error");
    return;
  }
}

void SR830::setDetHarm(int harm) {
  if(harm < 1 || harm > 19999) {
    ERROR_LOG("Wrong value of detection harmonic");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "HARM %d\r", harm);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Detection harmonic set error");
    return;
  }
}

void SR830::setRefFreq(double freq) {
  if(freq < 0.001 || freq > 102000) {
    ERROR_LOG("Wrong value of reference frequency");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "FREQ %f\r", freq);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Reference frequency set error");
    return;
  }
}

void SR830::setAmplSinOut(double volt) {
  if(volt < 0.004 || volt > 5.000) {
    ERROR_LOG("Wrong value of amplitude of the sine output");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "SLVL %f\r", volt);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Error set amplitude of the sine output");
    return;
  }
}

void SR830::setRefTrig(int mode) {
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
}

void SR830::setNotchFiltStatus(int status) {
  if(status < 0 || status > 3) {
    ERROR_LOG("Wrong value of notch filter status");
    return;
  }
  int n;
  char buff[20];
  sprintf(buff, "ILIN %d\r", status);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Notch filter status set error");
    return;
  }
}

void SR830::setSyncFiltStatus(bool status) {
  int n;
  char buff[20];
  sprintf(buff, "SYNC %d\r", status);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Synchronous filter status set error");
    return;
  }
}

void SR830::setTimeConst(int time) {
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
    FP_NAN
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
}

void SR830::setFiltSlope(double slope) {
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
}

void SR830::setDisplaySettings(int nchannel, int ndisplay, int ratio) {
  if(nchannel < 1 || nchannel > 2 || ndisplay < 0 || ndisplay > 4 || ratio < 0 || ratio > 2) {
    ERROR_LOG("Wrong values of display settings");
    return;
  }
  char buff[20];
  int n;
  sprintf(buff, "DDEF%d, %d, %d;\r", nchannel, ndisplay, ratio);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Display settings set error");
    return;
  }
}

void SR830::setOutSource(int nchannel, int outQuan) {
  if(nchannel < 1 || nchannel > 2 || outQuan < 0 || outQuan > 1) {
    ERROR_LOG("Wrong values of output source");
    return;
  }
  char buff[20];
  int n;
  sprintf(buff, "FPOP%d, %d;\r", nchannel, outQuan);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Output source set error");
    return;
  }
}

void SR830::setOffsetGain(int nChannel, int perOffset, int nExpand) {
  if(nChannel < 1 || nChannel > 3 || perOffset < (-105) || perOffset > 105 || nExpand < 0 || nExpand > 2) {
    ERROR_LOG("Wrong value of offset or gain");
    return;
  }
  char buff[20];
  int n;
  sprintf(buff, "OEXP%d, %d, %d;\r", nChannel, perOffset, nExpand);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Offset and gain set error");
    return;
  }
}

void SR830::setOffsetGainX(int perOffset, int nExpand) {
  if(perOffset < (-105) || perOffset > 105 || nExpand < 0 || nExpand > 2) {
    ERROR_LOG("Wrong value of offset or gain X");
    return;
  }
  setOffsetGain(1, perOffset, nExpand);
}

void SR830::setOffsetGainY(int perOffset, int nExpand) {
  if(perOffset < (-105) || perOffset > 105 || nExpand < 0 || nExpand > 2) {
    ERROR_LOG("Wrong value of offset or gain Y");
    return;
  }
  setOffsetGain(2, perOffset, nExpand);
}

void SR830::setAuxOutVolt(int nOutput, double volt) {
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
}

void SR830::setZeroAOV() {
  for(int i = 1; i <= 4; i++) setAuxOutVolt(i,0);
}

void SR830::startManualSetting(double phase, int sens, int rmod) {
  setPhase(phase);
  setSens(sens);
  setReserveMode(rmod);
}

void SR830::startAutoSetting() {
  int n;
  n = ttym_write(tty, (void*)"AGAN;ARSV;APHS\r", 15);
  if(!n) {
    ERROR_LOG("Error set phase");
    return;
  }
}

void SR830::refSetting(int RSmode, int harm, double freq, double volt, int RTmode) {
  setRefSource(RSmode);
  setDetHarm(harm);
  setRefFreq(freq);
  setAmplSinOut(volt);
  setRefTrig(RTmode);
}

void SR830::filtSetting(int Nstatus, int Sstatus, int time, int slope) {
  setNotchFiltStatus(Nstatus);
  setSyncFiltStatus(Sstatus);
  setTimeConst(time);
  setFiltSlope(slope);
}

void SR830::settingEveryAuxOutVolt(double volt1, double volt2, double volt3, double volt4) {
  if(volt1 < (-10.500) || volt1 > 10.500 || volt2 < (-10.500) || volt2 > 10.500 || volt3 < (-10.500) || volt3 > 10.500 || volt4 < (-10.500) || volt4 > 10.500) {
    ERROR_LOG("Wrong values of volt");
    return;
  }
  setAuxOutVolt(1, volt1);
  setAuxOutVolt(2, volt2);
  setAuxOutVolt(3, volt3);
  setAuxOutVolt(4, volt4);
}

double SR830::getPhase() {
  int n;
  double res;
  char buff[20];
  n = ttym_write(tty, (void*)"PHAS?\r", 6);
  if(!n) {
    ERROR_LOG("Error get phase");
    return -1001;
  }
  ttym_read(tty, buff, 20);

  res = strtod(buff, NULL);
  return res;
}

int SR830::getSens() {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"SENS?\r", 6);
  if(!n) {
    ERROR_LOG("Error get sensitivity");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atoi(buff);
  return res;
}

int SR830::getReserveMode() {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"RMOD?\r", 6);
  if(!n) {
    ERROR_LOG("Error get reverse mode");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atoi(buff);
  return res;
}

int SR830::getRefSource() {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"FMOD?\r", 6);
  if(!n) {
    ERROR_LOG("Error get reference source");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atoi(buff);
  return res;
}

int SR830::getDetHarm() {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"HARM?\r", 6);
  if(!n) {
    ERROR_LOG("Error get detection harmonic");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atoi(buff);
  return res;
}

double SR830::getRefFreq() {
  int n;
  double res;
  char buff[20];
  n = ttym_write(tty, (void*)"FREQ?\r", 6);
  if(!n) {
    ERROR_LOG("Error get reference frequency");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atof(buff);
  return res;
}

double SR830::getAmplSinOut() {
  int n;
  double res;
  char buff[20];
  n = ttym_write(tty, (void*)"SLVL?\r", 6);
  if(!n) {
    ERROR_LOG("Error get amplitude of the sine output");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atof(buff);
  return res;
}

int SR830::getRefTrig() {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"RSLP?\r", 6);
  if(!n) {
    ERROR_LOG("Error get reference trigger");
    return -1001;
  }
  ttym_read(tty, buff, 20);

  res = atoi(buff);
  return res;
}

int SR830::getNotchFiltStatus() {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"ILIN?\r", 6);
  if(!n) {
    ERROR_LOG("Error get notch filter status");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atoi(buff);
  return res;
}

int SR830::getSyncFiltStatus() {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"SYNC?\r", 6);
  if(!n) {
    ERROR_LOG("Error get synchronous filter status");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atoi(buff);
  return res;
}

int SR830::getTimeConst() {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"OFLT?\r", 6);
  if(!n) {
    ERROR_LOG("Error get time constant");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atoi(buff);
  return res;
}

int SR830::getFiltSlope() {
  int n, res;
  char buff[20];
  n = ttym_write(tty, (void*)"OFSL?\r", 6);
  if(!n) {
    ERROR_LOG("Error get filter slope");
    return -1001;
  }
  ttym_read(tty, buff, 20);

  res = atoi(buff);
  return res;
}

std::complex<double> SR830::getOffsetGain(int nchannel) {
  if(nchannel < 1 || nchannel > 3) {
    ERROR_LOG("Wrong value of offset or gain");
    return -1004;
  }
  char buff[20];
  char *end;
  int n;
  double a, b;
  sprintf(buff, "OEXP? %d;\r", nchannel);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Offset and gain get error");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  

  a = strtod(buff, &end);
  b = strtod(end+1, NULL);
  std::complex<double> z(a,b);
  return z;
}

double SR830::getAuxOutVolt(int nOutput) {
  if(nOutput < 1 || nOutput > 4) {
    ERROR_LOG("Wrong value of aux output voltage");
    return -1004;
  }
  char buff[20];
  int n;
  double res;
  sprintf(buff, "AUXV%d\r", nOutput);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    ERROR_LOG("Aux output voltage get error");
    return -1001;
  }
  ttym_read(tty, buff, 20);
  
  res = atof(buff);
  return res;
}
double SR830::getX() {
  char buff[20];
  int n;
  double res;
  n = ttym_write(tty, (void*)"OUTP? 1\r", 8);
  if(!n) {
    ERROR_LOG("Get X read error");
    return -1001;
  }
  ttym_read(tty, buff, 20);

  res = atof(buff);
  return res;
}
double SR830::getY() {
  char buff[20];
  int n;
  double res;
  n = ttym_write(tty, (void*)"OUTP? 2\r", 8);
  if(!n) {
    ERROR_LOG("Get Y read error");
    return -1001;
  }
  ttym_read(tty, buff, 20);

  res = atof(buff);
  return res;
}
double SR830::getR() {
  char buff[20];
  int n;
  double res;
  n = ttym_write(tty, (void*)"OUTP? 3\r", 8);
  if(!n) {
    ERROR_LOG("Get R read error");
    return -1001;
  }
  ttym_read(tty, buff, 20);

  res = atof(buff);
  return res;
}
double SR830::getTetta() {
  char buff[20];
  int n;
  double res;
  n = ttym_write(tty, (void*)"OUTP? 4\r", 8);
  if(!n) {
    ERROR_LOG("Get Tetta read error");
    return -1001;
  }
  ttym_read(tty, buff, 20);

  res = atof(buff);
  return res;
}

void SR830::close() {
  ttym_close(tty);
  tty = NULL;
}

int SR830::findCeilInArr(const double arr[], double val){
  for(int i=0; arr[i] != FP_NAN; i++){
    if( val <= arr[i] )return i;
  }
  return -1;
}