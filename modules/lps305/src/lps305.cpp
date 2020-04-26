#include <cmath>
#include <cstring>
#include "lps305.h"
#include <iostream>


LPS305::LPS305() {
  tty = NULL;
}

LPS305::~LPS305() {
  if(tty)ttym_close(tty);
}

int LPS305::connect(char portName[] , int baud) {
  char buff[256];
  tty = ttym_open(portName, baud);
  if(tty == NULL) {
    printf("No connect!\n");
    return (int)NULL;
  }
  ttym_timeout(tty, 1000);
  
  setOutput(0);
  setVoltage(1, 0);
  setVoltage(2, 0);
  setCurrent(1, 0);
  setCurrent(2, 0);
  
  ttym_read(tty,buff,200);
  return 1;
}
  
void LPS305::setVoltage(int channel, double volt) {
  if(channel < 1 || channel > 2 || volt < (-30) || volt > 30) {
    printf("Setup voltage error");
    return;
  }
  char buff[256];
  sprintf(buff, "VSET%d %f\n", channel, volt);
  ttym_write(tty, buff, strlen(buff));
  ttym_read(tty, buff, 255);
}

void LPS305::setCurrent(int channel, double curr) {
  if(channel < 1 || channel > 2 || curr < (-2.5) || curr > 2.5) {
    printf("Setup current error");
    return;
  }
  char buff[256];
  sprintf(buff, "ISET%d %f\n", channel, curr);
  ttym_write(tty, buff, strlen(buff));
  ttym_read(tty, buff, 255);
}

void LPS305::setOutput(int mode) {
  if(mode < 0 || mode > 1) {
    printf("Setup output error");
    return;
  }
  char buff[256];
  sprintf(buff, "OUT%d\n", mode);
  ttym_write(tty, buff, strlen(buff));
  ttym_read(tty, buff, 255);
}

void LPS305::setModeTracking(int mode) {
  if(mode < 0 || mode > 2) {
    printf("Setup tracking error");
    return;
  }
  char buff[256];
  sprintf(buff, "TRACK%d\n", mode);
  ttym_write(tty, buff, strlen(buff));
  ttym_read(tty, buff, 255);
}

void LPS305::setBeeper(int mode) {
  if(mode < 0 || mode > 1) {
    printf("Setup beeper error");
    return;
  }
  char buff[256];
  sprintf(buff, "BEEP%d\n", mode);
  ttym_write(tty, buff, strlen(buff));
  ttym_read(tty, buff, 255);
} 

void LPS305::setDigOutput(int mode) {
  if(mode != 0 || mode != 3 || mode != 5) {
    printf("Setup digital output error");
    return;
  }
  char buff[256];
  sprintf(buff, "VDD%d\n", mode);
  ttym_write(tty, buff, strlen(buff));
  ttym_read(tty, buff, 255);
}

void LPS305::setCompensatedOutput(int mode) {
  if(mode < 0 || mode > 1) {
    std::cout << "Setup compensated output error" << std::endl;
    return;
  }
  char buff[256];
  sprintf(buff, "LOWA%d\n", mode);
  ttym_write(tty, buff, strlen(buff));
  ttym_read(tty, buff, 255);
}
  
double LPS305::getVoltage(int channel) {
  if(channel < 1 || channel > 2) {
    printf("Setup output error");
    return -101;
  }
  double res;
  char buff[256];
  sprintf(buff, "VOUT%d\n", channel);
  ttym_write(tty, buff, strlen(buff));
  ttym_read(tty, buff, 255);

  res = atof(buff);
  return res;
}

double LPS305::getCurrent(int channel) {
  if(channel < 1 || channel > 2) {
    printf("Setup output error");
    return -101;
  }
  double res;
  char buff[256];
  sprintf(buff, "IOUT%d\n", channel);
  ttym_write(tty, buff, strlen(buff));
  ttym_read(tty, buff, 255);
  
  res = atof(buff);
  return res;
}

int LPS305::getStatus() {
  int res;
  char buff[256];
  ttym_write(tty, (void*)"STATUS\n", 7);
  ttym_read(tty, buff, 255);
  
  res = atoi(buff);
  return res;
}

char* LPS305::getModel() {
  char *buff = (char*)malloc(sizeof(char)*256);
  ttym_write(tty, (void*)"MODEL\n", 6);
  ttym_read(tty, buff, 255);
  return buff;
}

char* LPS305::getVersion() {
  char *buff = (char*)malloc(sizeof(char)*256);
  ttym_write(tty, (void*)"VERSION\n", 8);
  ttym_read(tty, buff, 255);
  return buff;
}

char* LPS305::getHelp() {
  char *buff = (char*)malloc(sizeof(char)*256);
  ttym_write(tty, (void*)"HELP\n", 5);
  ttym_read(tty, buff, 255);
  return buff;
}

void LPS305::close() {
  if(tty != NULL) {
    ttym_close(tty);
    tty = NULL;
  }
}


