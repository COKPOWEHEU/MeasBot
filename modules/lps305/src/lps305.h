#ifndef __LPS305_H__
#define __LPS305_H__

#include "../../tty/src/ttym.h"

class LPS305 {
private:
  ttym_t tty;
public:
  LPS305();
  ~LPS305();
  int connect(char portName[] , int baud);
  
  void setVoltage(int channel, double volt);
  void setCurrent(int channel, double curr);
  void setOutput(int mode);
  void setModeTracking(int mode);
  void setBeeper(int mode);
  void setDigOutput(int mode);
  void setCompensatedOutput(int mode);
  
  double getVoltage(int channel);
  double getCurrent(int channel);
  int getStatus();
  char* getModel();
  char* getVersion();
  char* getHelp();
  
  void close();
  
};

#endif