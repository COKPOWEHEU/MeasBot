#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SR570.hpp"


SR570::SR570() {
  tty = NULL;
}

SR570::~SR570() {
  if(tty)ttym_close(tty);
}

int SR570::connect(char portName[], int baud) {
  char buff[256];
  tty = ttym_open(portName, baud);
  if(tty == NULL){
    fprintf(stderr, "Can not open %s\n", portName);
    return 0;
  }
  ttym_timeout(tty, 1000);
  
  SR570::reset(); // *RST - сброс усилителя в настройки по умолчанию
  SR570::setSens(1e-3); // SENS - устанавливаем чувствительность усилителя
  SR570::turnInOffsetCurr(0); // IOON - выключатель входного тока смещения
  SR570::setCalOffsetCurrentLVL(2e-12); // IOLV - устанавливает калиброванный уровень входного тока смещения
  SR570::setTypeFilter(2); // FLTT - устанавливает тип фильтра
  SR570::setHighFilter(0.3); // HFRQ - устанавливает значение точки высокочастотного фильтра 3дБ
  SR570::setLowFilter(10*1000); // LFRQ - устанавливает значение точки низкочастотного фильтра 3дБ
  SR570::setUncalInOffsetVernier(0); // IOUV - установка некалиброванного входного  смещения веренье. Веренье используется в калбировке для компенсации колебаний усиления, вознакающих при изменениях конфигурации, таких как входное соединение и настройки фильтра. Так же для некалбированных.
  SR570::setSenCal(0); // SUCM - устанавливает режим калибровки чувствительности
  SR570::setInOffsetCurrSign(1); // IOSN - устанавливает знак(+/-) входного тока смещения
  SR570::setInOffsetCalMode(0); // IOUC - устанавливает режим калибровки входного тока смещения
  SR570::turnBiasVolt(0); // BSON - выключатель смещения напряжения
  SR570::setSigInvertSense(1); // INVT - устанавливает обратный сигнал чувствительности
  SR570::setBlanksOutAmplifier(0); // BLNK - заглушает(выключает) выход усилителя
  SR570::setUncalSensVernier(0); // SUCV - установка некалиброванной чувствительности веренье. Веренье используется в калбировке для компенсации колебаний усиления, вознакающих при изменениях конфигурации, таких как входное соединение и настройки фильтра. Так же для некалбированных.
  SR570::setBiasVoltLVL(0); // BSLV - устанавливает уровень смещения напряжения 
  SR570::setGainMode(0); // GNMD - устанавливает режим усиления усилителя
  //SR570::resetFilCap(); // ROLD - сброс конденсаторов фильтра, чтобы очистить условие перегрузки
  //SR570::reset(); // *RST
  ttym_read(tty, buff, 255);
  
  return 1;
}

void SR570::reset() {
  ttym_write(tty, (void*)"*RST;\n", 6);
}

void SR570::setSens(float sens_A_V){
  const float sens[] = {
    1e-12,  2e-12,  5e-12,
    10e-12, 20e-12, 50e-12,
    100e-12,200e-12,500e-12,
    1e-9,   2e-9,   5e-9,
    10e-9,  20e-9,  50e-9,
    100e-9, 200e-9, 500e-9,
    1e-6,   2e-6,   5e-6,
    10e-6,  20e-6,  50e-6,
    100e-6, 200e-6, 500e-6,
    1e-3,
    FP_NAN
  };
  
  int sens_num = findCeilInArr(sens, sens_A_V);
  if(sens_num < 0){
    ERROR_LOG("Wrong sens");
    return;
  }
  char buff[256];

  sprintf(buff, "SENS%d;\n", sens_num);
  ttym_write(tty, buff, 8);
  ttym_read(tty, buff, 255);
}

void SR570::setCalOffsetCurrentLVL(float curr_A){
  const float curr[] = {
    1e-12,  2e-12,  5e-12,
    10e-12, 20e-12, 50e-12,
    100e-12,200e-12,500e-12,
    1e-9,   2e-9,   5e-9,
    10e-9,  20e-9,  50e-9,
    100e-9, 200e-9, 500e-9,
    1e-6,   2e-6,   5e-6,
    10e-6,  20e-6,  50e-6,
    100e-6, 200e-6, 500e-6,
    1e-3,   2e-3,   5e-3,
    FP_NAN
  };
  
  int curr_num = findCeilInArr(curr, curr_A);
  
  if(curr_num < 0){
    ERROR_LOG("Wrong current value");
    return;
  }
  
  char buff[256];
  sprintf(buff, "%s%d;\n", "IOLV", curr_num);
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setTypeFilter(int nType) {
  char buff[256];
  if(nType < 0 || nType > 5) {
    ERROR_LOG("Wrong type value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "FLTT", nType);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setHighFilter(float freqFilter){
  const float freq[] = {
    0.03,
    0.1, 0.3,
    1e0, 3e0,
    1e1, 3e1,
    1e2, 3e2,
    1e3, 3e3,
    1e4,
    // Допустимое значения для High Filter - 10 кГц
    // 3e4,  
    // 1e5, 3e5,
    // 1e6,
    FP_NAN
  };
  int freq_num = findCeilInArr(freq, freqFilter);
  
  if( freq_num < 0 ){
    ERROR_LOG("Wrong Highpass filter frequency value");
    return;
  }
  
  char buff[256];
  sprintf(buff, "%s%d;\n", "HFRQ", freq_num);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setLowFilter(float freqFilter){
  const float freq[] = {
    0.03,
    0.1, 0.3,
    1e0, 3e0,
    1e1, 3e1,
    1e2, 3e2,
    1e3, 3e3,
    1e4, 3e4, 
    1e5, 3e5,
    1e6,
    FP_NAN
  };
  int freq_num = findCeilInArr(freq, freqFilter);
  
  if( freq_num < 0 ){
    ERROR_LOG("Wrong Lowpass filter frequency value");
    return;
  }
  
  char buff[256];
  sprintf(buff, "%s%d;\n", "LFRQ", freqFilter);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setUncalInOffsetVernier(int scale) {
  char buff[256];
  if(scale < -1000 || scale > 1000) {
    ERROR_LOG("Wrong value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "IOUV", scale);
  
  ttym_write(tty, buff, strlen(buff));  
}

void SR570::setInOffsetCalMode(int calMode) {
  char buff[256];
  if(calMode < 0 || calMode > 1) {
    ERROR_LOG("Wrong mode value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "IOUC", calMode);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::turnInOffsetCurr(int val) {
  char buff[256];
  if(val < 0 || val > 1) {
    ERROR_LOG("Wrong turn the input offset current value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "IOON", val);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::turnBiasVolt(int val) {
  char buff[256];
  if(val < 0 || val > 1) {
    ERROR_LOG("Wrong turn bias voltage value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "BSON", val);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setSenCal(int calMode) {
  char buff[256];
  if(calMode < 0 || calMode > 1) {
    ERROR_LOG("Wrong cal mode value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "SUCM", calMode);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setInOffsetCurrSign(int sign) {
  char buff[256];
  if(sign < 0 || sign > 1) {
    ERROR_LOG("Wrong sign value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "IOSN", sign);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setSigInvertSense(int mode) {
  char buff[256];
  if(mode < 0 || mode > 1) {
    ERROR_LOG("Wrong invert sense value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "INVT", mode);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setBlanksOutAmplifier(int mode) {
  char buff[256];
  if(mode < 0 || mode > 1) {
    ERROR_LOG("Wrong output amplifier value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "BLNK", mode);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setUncalSensVernier(int scale) {
  char buff[256];
  if(scale < 0 || scale > 100) {
    ERROR_LOG("Wrong scale value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "SUCV", scale);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setBiasVoltLVL(float bias_V){
  char buff[256];
  if(bias_V < -5 || bias_V > 5){
    ERROR_LOG("Wrong bias voltage level value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "BSLV", (int)(bias_V*1000));
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setGainMode(int mode) {
  char buff[256];
  if(mode < 0 || mode > 2) {
    ERROR_LOG("Wrong gain mode value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "GNMD", mode);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::resetFilCap() {
  ttym_write(tty, (void*)"ROLD;\n", 6);
}

void SR570::closePort() {
  ttym_close(tty);
  tty=NULL;
}

int SR570::findCeilInArr(const float arr[], float val){
  for(int i=0; arr[i] != FP_NAN; i++){
    if( val <= arr[i] )return i;
  }
  return -1;
}
