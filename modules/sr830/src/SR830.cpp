//TODO возвращающее значение функций get привести к числам.

#include <iostream>
#include <stdlib.h>
#include <math.h>
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
    printf("No connect!\n");
    return 0;
  }
  ttym_timeout(tty, 1000);
    
  setRS232();
  clrRegs();   
  
  setPermStatReg(53, 15, 214, 63);    
  setInSettings(0, 1, 0);
  startManualSetting(0, 23, 1);
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
  char buff[256];
  n = ttym_write(tty, (void*)"OUTX 0\r", 7);
  if(!n) {
    printf("Error of set RS232");
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
    printf("Error of enable reset");
    return;
  }
}

char* SR830::readID() {
  char *buff = (char*)malloc(sizeof(char)*256);
  int n;
  n = ttym_write(tty, (void*)"*IDN?\r", 6);
  if(!n) {
    printf("Error of read ID");
    return (char*)"ID:-1";
  }
  ttym_read(tty, buff, 200);
  return buff;
}

void SR830::clrRegs() {
  int n;
  n = ttym_write(tty, (void*)"*CLS\r",6);
  if(!n) {
    printf("Error of clear status bytes");
    return;
  }
}
//TODO разбить каждую команду на функции
void SR830::setPermStatReg(int stdEventB, int liaB, int errB, int serialPollB) {
  int n;
  char buff[256];
  sprintf(buff, "*ESE %d;LIAE %d;ERRE %d;*SRE %d;\r", stdEventB, liaB, errB, serialPollB);
  
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Error of set status registers");
    return;
  }
}
//TODO разбить каждую команду на функции
void SR830::setInSettings(int inConfig, int inShGND, int inCoupl) {
  int n;
  char buff[256];
  sprintf(buff, "ISRC %d;IGND %d;ICPL %d;\r", inConfig, inShGND, inCoupl);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Error of set incoming settings");
    return;
  }
}

void SR830::setPhase(double phase) {
  if(phase < (-360.00) || phase > 729.99) {
    printf("Wrong value of phase");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "PHAS %f\r", phase);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Phase set error");
    return;
  }
}

void SR830::setSens(int sens) {
  if(sens < 0 || sens > 26) {
    printf("Wrong value of sensitivity");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "SENS %d\r", sens);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Sensitivity set error");
    return;
  }
}

void SR830::setReserveMode(int mode) {
  if(mode < 0 || mode > 2) {
    printf("Wrong value of mode");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "RMOD %d\r", mode);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Reverse mode set error");
    return;
  }
}

void SR830::setRefSource(int mode) {
  if(mode < 0 || mode > 1) {
    printf("Wrong value of mode");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "FMOD %d\r", mode);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Reference source set error");
    return;
  }
}

void SR830::setDetHarm(int harm) {
  if(harm < 1 || harm > 19999) {
    printf("Wrong value of detection harmonic");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "HARM %d\r", harm);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Detection harmonic set error");
    return;
  }
}

void SR830::setRefFreq(double freq) {
  if(freq < 0.001 || freq > 102000) {
    printf("Wrong value of reference frequency");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "FREQ %f\r", freq);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Reference frequency set error");
    return;
  }
}

void SR830::setAmplSinOut(double volt) {
  if(volt < 0.004 || volt > 5.000) {
    printf("Wrong value of amplitude of the sine output");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "SLVL %f\r", volt);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Error set amplitude of the sine output");
    return;
  }
}

void SR830::setRefTrig(int mode) {
  if(mode < 0 || mode > 2) {
    printf("Wrong value of reference trigger");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "RSLP %d\r", mode);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Reference trigger set error");
    return;
  }
}

void SR830::setNotchFiltStatus(int status) {
  if(status < 0 || status > 3) {
    printf("Wrong value of notch filter status");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "ILIN %d\r", status);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Notch filter status set error");
    return;
  }
}

void SR830::setSyncFiltStatus(int status) {
  if(status < 0 || status > 1) {
    printf("Wrong value of synchronous filter status");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "SYNC %d\r", status);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Synchronous filter status set error");
    return;
  }
}

void SR830::setTimeConst(int time) {
  if(time < 0 || time > 19) {
    printf("Wrong value of time constant");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "OFLT %d\r", time);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Time constant set error");
    return;
  }
}

void SR830::setFiltSlope(int slope) {
  if(slope < 0 || slope > 3) {
    printf("Wrong value of the low pass filter slope");
    return;
  }
  int n;
  char buff[256];
  sprintf(buff, "OFLT %d\r", slope);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Low pass filter slope set error");
    return;
  }
}

void SR830::setDisplaySettings(int nchannel, int ndisplay, int ratio) {
  if(nchannel < 1 || nchannel > 2 || ndisplay < 0 || ndisplay > 4 || ratio < 0 || ratio > 2) {
    printf("Wrong values of display settings");
    return;
  }
  char buff[256];
  int n;
  sprintf(buff, "DDEF%d, %d, %d;\r", nchannel, ndisplay, ratio);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Display settings set error");
    return;
  }
}

void SR830::setOutSource(int nchannel, int outQuan) {
  if(nchannel < 1 || nchannel > 2 || outQuan < 0 || outQuan > 1) {
    printf("Wrong values of output source");
    return;
  }
  char buff[256];
  int n;
  sprintf(buff, "FPOP%d, %d;\r", nchannel, outQuan);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Output source set error");
    return;
  }
}

void SR830::setOffsetGain(int nChannel, int perOffset, int nExpand) {
  if(nChannel < 1 || nChannel > 3 || perOffset < (-105) || perOffset > 105 || nExpand < 0 || nExpand > 2) {
    printf("Wrong value of offset or gain");
    return;
  }
  char buff[256];
  int n;
  sprintf(buff, "OEXP%d, %d, %d;\r", nChannel, perOffset, nExpand);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Offset and gain set error");
    return;
  }
}

void SR830::setOffsetGainX(int perOffset, int nExpand) {
  if(perOffset < (-105) || perOffset > 105 || nExpand < 0 || nExpand > 2) {
    printf("Wrong value of offset or gain X");
    return;
  }
  setOffsetGain(1, perOffset, nExpand);
}

void SR830::setOffsetGainY(int perOffset, int nExpand) {
  if(perOffset < (-105) || perOffset > 105 || nExpand < 0 || nExpand > 2) {
    printf("Wrong value of offset or gain Y");
    return;
  }
  setOffsetGain(2, perOffset, nExpand);
}

void SR830::setAuxOutVolt(int nOutput, double volt) {
  if(nOutput < 1 || nOutput > 4 || volt < (-10.500) || volt > (10.500)) {
    printf("Wrong value of aux output voltage");
    return;
  }
  char buff[256];
  int n;
  sprintf(buff, "AUXV%d, %f\r", nOutput, volt);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Aux output voltage set error");
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
    printf("Error set phase");
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
    printf("Wrong values of volt");
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
  char buff[256];
  n = ttym_write(tty, (void*)"PHAS?\r", 6);
  if(!n) {
    printf("Error get phase");
    return -1001;
  }
  ttym_read(tty, buff, 255);

  res = strtod(buff, NULL);
  return res;
}

int SR830::getSens() {
  int n, res;
  char buff[256];
  n = ttym_write(tty, (void*)"SENS?\r", 6);
  if(!n) {
    printf("Error get sensitivity");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atoi(buff);
  return res;
}

int SR830::getReserveMode() {
  int n, res;
  char buff[256];
  n = ttym_write(tty, (void*)"RMOD?\r", 6);
  if(!n) {
    printf("Error get reverse mode");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atoi(buff);
  return res;
}

int SR830::getRefSource() {
  int n, res;
  char buff[256];
  n = ttym_write(tty, (void*)"FMOD?\r", 6);
  if(!n) {
    printf("Error get reference source");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atoi(buff);
  return res;
}

int SR830::getDetHarm() {
  int n, res;
  char buff[256];
  n = ttym_write(tty, (void*)"HARM?\r", 6);
  if(!n) {
    printf("Error get detection harmonic");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atoi(buff);
  return res;
}

double SR830::getRefFreq() {
  int n;
  double res;
  char buff[256];
  n = ttym_write(tty, (void*)"FREQ?\r", 6);
  if(!n) {
    printf("Error get reference frequency");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atof(buff);
  return res;
}

double SR830::getAmplSinOut() {
  int n;
  double res;
  char buff[256];
  n = ttym_write(tty, (void*)"SLVL?\r", 6);
  if(!n) {
    printf("Error get amplitude of the sine output");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atof(buff);
  return res;
}

int SR830::getRefTrig() {
  int n, res;
  char buff[256];
  n = ttym_write(tty, (void*)"RSLP?\r", 6);
  if(!n) {
    printf("Error get reference trigger");
    return -1001;
  }
  ttym_read(tty, buff, 255);

  res = atoi(buff);
  return res;
}

int SR830::getNotchFiltStatus() {
  int n, res;
  char buff[256];
  n = ttym_write(tty, (void*)"ILIN?\r", 6);
  if(!n) {
    printf("Error get notch filter status");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atoi(buff);
  return res;
}

int SR830::getSyncFiltStatus() {
  int n, res;
  char buff[256];
  n = ttym_write(tty, (void*)"SYNC?\r", 6);
  if(!n) {
    printf("Error get synchronous filter status");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atoi(buff);
  return res;
}

int SR830::getTimeConst() {
  int n, res;
  char buff[256];
  n = ttym_write(tty, (void*)"OFLT?\r", 6);
  if(!n) {
    printf("Error get time constant");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atoi(buff);
  return res;
}

int SR830::getFiltSlope() {
  int n, res;
  char buff[256];
  n = ttym_write(tty, (void*)"OFSL?\r", 6);
  if(!n) {
    printf("Error get filter slope");
    return -1001;
  }
  ttym_read(tty, buff, 255);

  res = atoi(buff);
  return res;
}

std::complex<double> SR830::getOffsetGain(int nchannel) {
  if(nchannel < 1 || nchannel > 3) {
    printf("Wrong value of offset or gain");
    return -1004;
  }
  char buff[256];
  char *end;
  int n;
  double a, b;
  sprintf(buff, "OEXP? %d;\r", nchannel);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Offset and gain get error");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  

  a = strtod(buff, &end);
  b = strtod(end+1, NULL);
  std::complex<double> z(a,b);
  return z;
}

double SR830::getAuxOutVolt(int nOutput) {
  if(nOutput < 1 || nOutput > 4) {
    printf("Wrong value of aux output voltage");
    return -1004;
  }
  char buff[256];
  int n;
  double res;
  sprintf(buff, "AUXV%d\r", nOutput);
  n = ttym_write(tty, buff, strlen(buff));
  if(!n) {
    printf("Aux output voltage get error");
    return -1001;
  }
  ttym_read(tty, buff, 255);
  
  res = atof(buff);
  return res;
}
double SR830::getX() {
  char buff[256];
  int n;
  double res;
  n = ttym_write(tty, (void*)"OUTP? 1\r", 8);
  if(!n) {
    printf("Get X read error");
    return -1001;
  }
  ttym_read(tty, buff, 255);

  res = atof(buff);
  return res;
}
double SR830::getY() {
  char buff[256];
  int n;
  double res;
  n = ttym_write(tty, (void*)"OUTP? 2\r", 8);
  if(!n) {
    printf("Get Y read error");
    return -1001;
  }
  ttym_read(tty, buff, 255);

  res = atof(buff);
  return res;
}
double SR830::getR() {
  char buff[256];
  int n;
  double res;
  n = ttym_write(tty, (void*)"OUTP? 3\r", 8);
  if(!n) {
    printf("Get R read error");
    return -1001;
  }
  ttym_read(tty, buff, 255);

  res = atof(buff);
  return res;
}
double SR830::getTetta() {
  char buff[256];
  int n;
  double res;
  n = ttym_write(tty, (void*)"OUTP? 4\r", 8);
  if(!n) {
    printf("Get Tetta read error");
    return -1001;
  }
  ttym_read(tty, buff, 255);

  res = atof(buff);
  return res;
}

void SR830::close() {
  ttym_close(tty);
  tty = NULL;
}
