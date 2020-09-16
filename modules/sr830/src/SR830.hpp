#pragma once

#include <complex>
#include "ttym.h"

#define ERROR_LOG(message) std::cerr << __FILE__ << ":" << __func__ << ":" << __LINE__ << ": " << message << std::endl

class SR830 {
  private:
    ttym_t tty;
    int findCeilInArr(const double arr[], double val);
    std::string getstr();
  public:
    SR830();
    ~SR830();
    int connect(char portName[], int baud);
    void setRS232();
    void reset();
    //char* readID();
    std::string readID();

    void clrRegs();

    //TODO разбить каждую команду на функции
    //Стр. 104-107 (с 5-20 до 5-23)
    void setPermStatRegs(int stdEventB, int liaB, int errB, int serialPollB);

    void setInConfig(int inConf);
    void setInGndShield(bool gndval);
    void setInCoupling(bool coupling);
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
    void setTimeConst(double time);
    void setFiltSlope(double slope);
    void setDisplaySettings(int nchannel, int ndisplay, int ratio);
    void setOutSource(int nchannel, int outQuan);
    void setOffsetGain(int nChannel, int perOffset, int nExpand);
    void setOffsetGainX(int perOffset, int nExpand);
    void setOffsetGainY(int perOffset, int nExpand);
    void setAuxOutVolt(int nOutput, double volt);
    void setZeroAOV();
    
    void startManualSetting(double phase, int sens, int rmod);
    //TODO разбить каждую команду на функции
    //Стр. 95 (5-11)
    void startAutoSetting();
    void refSetting(bool RSmode, int harm, double freq, double volt, int RTmode);
    void filtSetting(int Nstatus, bool Sstatus, int time, int slope);
    void settingEveryAuxOutVolt(double volt1, double volt2, double volt3, double volt4);
    
    int getInConfig();
    int getInGndShield();
    int getInCoupling();
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
    int getOutSource(int nchannel);
    std::complex<double> getOffsetGain(int nchannel);
    double getAuxOutVolt(int nOutput);
    double getX();
    double getY();
    double getR();
    double getTetta();
    
    void close();
};
