#pragma once

#include <cstdlib>
#include <complex>
#include "ttym.h"

class SR830 {
  private:
    ttym_t tty;
  public:
    SR830();
    ~SR830();
    int connect(char portName[], int baud);
    void setRS232();
    void reset();
    char* readID();
    void clrRegs();
    void setPermStatReg(int stdEventB, int liaB, int errB, int serialPollB);
    void setInSettings(int inConfig, int inShGND, int inCoupl);
    void setPhase(double phase);
    void setSens(int sens);
    void setReserveMode(int mode);
    void setRefSource(int mode);
    void setDetHarm(int harm);
    void setRefFreq(double freq);
    void setAmplSinOut(double volt);
    void setRefTrig(int mode);
    void setNotchFiltStatus(int status);
    void setSyncFiltStatus(int status);
    void setTimeConst(int time);
    void setFiltSlope(int slope);
    void setDisplaySettings(int nchannel, int ndisplay, int ratio);
    void setOutSource(int nchannel, int outQuan);
    void setOffsetGain(int nChannel, int perOffset, int nExpand);
    void setOffsetGainX(int perOffset, int nExpand);
    void setOffsetGainY(int perOffset, int nExpand);
    void setAuxOutVolt(int nOutput, double volt);
    void setZeroAOV();
    
    void startManualSetting(double phase, int sens, int rmod);
    void startAutoSetting();
    void refSetting(int RSmode, int harm, double freq, double volt, int RTmode);
    void filtSetting(int Nstatus, int Sstatus, int time, int slope);
    void settingEveryAuxOutVolt(double volt1, double volt2, double volt3, double volt4);
    
    double getPhase();
    int getSens();
    int getReserveMode();
    int getRefSource();
    int getDetHarm();
    double getRefFreq();
    double getAmplSinOut();
    int getRefTrig();
    int getNotchFiltStatus();
    int getSyncFiltStatus();
    int getTimeConst();
    int getFiltSlope();
    std::complex<double> getOffsetGain(int nchannel);//TODO команда возращает 2 параметра, а функция 1
    double getAuxOutVolt(int nOutput);
    double getX();
    double getY();
    double getR();
    double getTetta();
    
    void close();
};
