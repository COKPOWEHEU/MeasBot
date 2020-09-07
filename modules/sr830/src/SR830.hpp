#pragma once

#include <complex>
#include "ttym.h"

#define ERROR_LOG(message) std::cerr << __FILE__ << ":" << __func__ << ":" << __LINE__ << ": " << message << std::endl

class SR830 {
  private:
    ttym_t tty;
    int findCeilInArr(const double arr[], double val);
  public:
    SR830();
    ~SR830();
    int connect(char portName[], int baud);
    void setRS232();
    void reset();
    char* readID();
    void clrRegs();

    //TODO разбить каждую команду на функции
    void setPermStatReg(int stdEventB, int liaB, int errB, int serialPollB);
    void setInSettings(int inConfig, int inShGND, int inCoupl);

    void setPhase(double phase);
    void setSens(double sens);
    void setReserveMode(int mode);
    void setRefSource(bool mode);
    void setDetHarm(int harm);
    void setRefFreq(double freq);
    void setAmplSinOut(double volt);
    void setRefTrig(int mode);
    void setNotchFiltStatus(int status);
    void setSyncFiltStatus(bool status);
    void setTimeConst(int time);
    void setFiltSlope(double slope);
    void setDisplaySettings(int nchannel, int ndisplay, int ratio);
    void setOutSource(int nchannel, int outQuan);
    void setOffsetGain(int nChannel, int perOffset, int nExpand);
    void setOffsetGainX(int perOffset, int nExpand);
    void setOffsetGainY(int perOffset, int nExpand);
    void setAuxOutVolt(int nOutput, double volt);
    void setZeroAOV();
    
    //TODO разбить каждую команду на функции
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
    std::complex<double> getOffsetGain(int nchannel);
    double getAuxOutVolt(int nOutput);
    double getX();
    double getY();
    double getR();
    double getTetta();
    
    void close();
};
