#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <math.h>
#include "SR570.hpp"

size_t findValAtArr(std::vector<float> arr, float val);

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
  SR570::setSens(0.001); // SENS - устанавливаем чувствительность усилителя
  SR570::turnInOffsetCurr(0); // IOON - выключатель входного тока смещения
  SR570::setCalOffsetCurrentLVL(2e-12); // IOLV - устанавливает калиброванный уровень входного тока смещения
  SR570::setTypeFilter(2); // FLTT - устанавливает тип фильтра
  SR570::setHighFilter(0.3); // HFRQ - устанавливает значение точки высокочастотного фильтра 3дБ
  SR570::setLowFilter(1e+4); // LFRQ - устанавливает значение точки низкочастотного фильтра 3дБ
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

void SR570::setSens(float sens) {
  char buff[256];
  size_t nsens;
  nsens = findValAtArr((std::vector<float>) {1e-12, 2e-12, 5e-12, 10e-12, 20e-12, 50e-12, 100e-12, 200e-12, 500e-12,
                                     1e-9, 2e-9, 5e-9, 10e-9, 20e-9, 50e-9, 100e-9, 200e-9, 500e-9,
                                     1e-6, 2e-6, 5e-6, 10e-6, 20e-6, 50e-6, 100e-6, 200e-6, 500e-6,
                                     1e-3}, sens);
  
  sprintf(buff, "SENS%d;\n", nsens);
  ttym_write(tty, buff, 8);
  ttym_read(tty, buff, 255);
}

void SR570::setCalOffsetCurrentLVL(float curr) {
  char buff[256];
  size_t ncurr;

  ncurr = findValAtArr((std::vector<float>) {1e-12, 2e-12, 5e-12, 10e-12, 20e-12, 50e-12, 100e-12, 200e-12, 500e-12,
                                     1e-9, 2e-9, 5e-9, 10e-9, 20e-9, 50e-9, 100e-9, 200e-9, 500e-9,
                                     1e-6, 2e-6, 5e-6, 10e-6, 20e-6, 50e-6, 100e-6, 200e-6, 500e-6,
                                     1e-3, 2e-3, 5e-3}, curr);
  
  sprintf(buff, "%s%d;\n", "IOLV", ncurr);
  
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

void SR570::setHighFilter(float freq) {
  char buff[256];
  size_t nfreq;
  nfreq = findValAtArr((std::vector<float>) {3e-2, 1e-1, 3e-1, 1e+0, 
                                             3e+0, 1e+1, 3e+1, 1e+2,
                                             3e+2, 1e+3, 3e+3, 1e+4}, freq);
  
  sprintf(buff, "%s%d;\n", "HFRQ", nfreq);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setLowFilter(float freq) {
  char buff[256];
  size_t nfreq;

  nfreq = findValAtArr((std::vector<float>) {3e-2, 1e-1, 3e-1, 1e+0, 
                                             3e+0, 1e+1, 3e+1, 1e+2,
                                             3e+2, 1e+3, 3e+3, 1e+4,
                                             3e+4, 1e+5, 3e+5, 1e+6}, freq);
  
  sprintf(buff, "%s%d;\n", "LFRQ", nfreq);
  
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

void SR570::setBiasVoltLVL(int nLevel) {
  char buff[256];
  if(nLevel < 0 || nLevel > 1) {
    ERROR_LOG("Wrong bias voltage level value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "BSLV", nLevel);
  
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

size_t findValAtArr(std::vector<float> arr, float val) {
  size_t res;

  if(val > arr[arr.size()-1])
    return arr.size()-1;
  else if(val < arr[0])
    return 0;

  for(size_t i = 0; i != arr.size(); i++) {
    if(val <= arr.at(i)) {
      res = i;
      break;
    }
  }
  return res;
}