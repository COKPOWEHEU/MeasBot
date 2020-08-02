#pragma once

#include "ttym.h"

class SR570 {
  private:
    ttym_t tty;
  public:
    SR570();
    ~SR570();
    int connect(char portName[], int baud);
    void reset();

    /*
      Устанавливает чувствительность усилителя по таблице:
      n            Чувствительность
      0, 1, 2,        1, 2, 5     пА/В
      3, 4, 5,       10, 20, 50   пА/В
      6, 7, 8,      100, 200, 500 пА/В
      9, 10, 11,      1, 2, 5     нА/В
      12, 13, 14,    10, 20, 50   нА/В
      15, 16, 17,   100, 200, 500 нА/В
      18, 19, 20,     1, 2, 5     мкА/В
      21, 22, 23     10, 20, 50   мкА/В
      24, 25, 26    100, 200, 500 мкА/В
      27                  1       мА/В
    */
    void setSens(int sens);

    /*
      Устанавливает калиброванный уровень входного тока смещения по таблице:
      n                 Ток
      0, 1, 2,        1, 2, 5     пА/В
      3, 4, 5,       10, 20, 50   пА/В
      6, 7, 8,      100, 200, 500 пА/В
      9, 10, 11,      1, 2, 5     нА/В
      12, 13, 14,    10, 20, 50   нА/В
      15, 16, 17,   100, 200, 500 нА/В
      18, 19, 20,     1, 2, 5     мкА/В
      21, 22, 23     10, 20, 50   мкА/В
      24, 25, 26    100, 200, 500 мкА/В
      27                  1       мА/В
    */
    void setCalOffsetCurrentLVL(int curr);
    void setHighFilter(int freqFilter);
    void setLowFilter(int freqFilter);
    void setTypeFilter(int nType);
    void setUncalInOffsetVernier(int scale);
    void setInOffsetCalMode(int calMode);
    void turnInOffsetCurr(int val);
    void turnBiasVolt(int val);
    void setSenCal(int calMode);
    void setInOffsetCurrSign(int sign);
    void setSigInvertSense(int mode);
    void setBlanksOutAmplifier(int mode);
    void setUncalSensVernier(int scale);
    void setBiasVoltLVL(int nLevel);
    void setGainMode(int mode);
    void resetFilCap();
    void closePort();
};
