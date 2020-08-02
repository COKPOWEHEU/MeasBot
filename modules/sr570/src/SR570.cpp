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
  SR570::setSens(27); // SENS - устанавливаем чувствительность усилителя
  SR570::turnInOffsetCurr(0); // IOON - выключатель входного тока смещения
  SR570::setCalOffsetCurrentLVL(1); // IOLV - устанавливает калиброванный уровень входного тока смещения
  SR570::setTypeFilter(2); // FLTT - устанавливает тип фильтра
  SR570::setHighFilter(2); // HFRQ - устанавливает значение точки высокочастотного фильтра 3дБ
  SR570::setLowFilter(11); // LFRQ - устанавливает значение точки низкочастотного фильтра 3дБ
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

void SR570::setSens(int sens) {
  char buff[256];
  if(sens < 0 || sens > 27) {
    printf("Wrong sens");
    return;
  }
  
  sprintf(buff, "SENS%d;\n", sens);
  ttym_write(tty, buff, 8);
  ttym_read(tty, buff, 255);
}

void SR570::setCalOffsetCurrentLVL(int curr) {
  char buff[256];
  if(curr < 0 || curr > 29) {
    printf("Wrong current value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "IOLV", curr);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setTypeFilter(int nType) {
  char buff[256];
  if(nType < 0 || nType > 5) {
    printf("Wrong type value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "FLTT", nType);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setHighFilter(int freqFilter) {
  char buff[256];
  if(freqFilter < 0 || freqFilter > 11) {
    printf("Wrong Highpass filter frequency value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "HFRQ", freqFilter);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setLowFilter(int freqFilter) {
  char buff[256];
  if(freqFilter < 0 || freqFilter > 15) {
    printf("Wrong Lowpass filter frequency value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "LFRQ", freqFilter);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setUncalInOffsetVernier(int scale) {
  char buff[256];
  if(scale < -1000 || scale > 1000) {
    printf("Wrong value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "IOUV", scale);
  
  ttym_write(tty, buff, strlen(buff));  
}

void SR570::setInOffsetCalMode(int calMode) {
  char buff[256];
  if(calMode < 0 || calMode > 1) {
    printf("Wrong mode value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "IOUC", calMode);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::turnInOffsetCurr(int val) {
  char buff[256];
  if(val < 0 || val > 1) {
    printf("Wrong turn the input offset current value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "IOON", val);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::turnBiasVolt(int val) {
  char buff[256];
  if(val < 0 || val > 1) {
    printf("Wrong turn bias voltage value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "BSON", val);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setSenCal(int calMode) {
  char buff[256];
  if(calMode < 0 || calMode > 1) {
    printf("Wrong cal mode value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "SUCM", calMode);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setInOffsetCurrSign(int sign) {
  char buff[256];
  if(sign < 0 || sign > 1) {
    printf("Wrong sign value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "IOSN", sign);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setSigInvertSense(int mode) {
  char buff[256];
  if(mode < 0 || mode > 1) {
    printf("Wrong invert sense value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "INVT", mode);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setBlanksOutAmplifier(int mode) {
  char buff[256];
  if(mode < 0 || mode > 1) {
    printf("Wrong output amplifier value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "BLNK", mode);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setUncalSensVernier(int scale) {
  char buff[256];
  if(scale < 0 || scale > 100) {
    printf("Wrong scale value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "SUCV", scale);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setBiasVoltLVL(int nLevel) {
  char buff[256];
  if(nLevel < 0 || nLevel > 1) {
    printf("Wrong bias voltage level value");
    return;
  }
  
  sprintf(buff, "%s%d;\n", "BSLV", nLevel);
  
  ttym_write(tty, buff, strlen(buff));
}

void SR570::setGainMode(int mode) {
  char buff[256];
  if(mode < 0 || mode > 2) {
    printf("Wrong gain mode value");
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