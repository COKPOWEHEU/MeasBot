#pragma once

#include "ttym.h"

#define ERROR_LOG(message) std::cerr << __FILE__ << ":" << __func__ << ":" << __LINE__ << ": " << message << std::endl

class SR570 {
  private:
    ttym_t tty;
    int findCeilInArr(const float arr[], float val);
  public:
    SR570();
    ~SR570();
    //Подключиться к удалённому прибору по заданному portName и скорости передачи baud
    int connect(char portName[], int baud);

    //Сбросить усилитель к настройкам по умолчанию
    void reset();

    void setSens(float sens_A_V);
    
    void setCalOffsetCurrentLVL(float curr_A);

    //Установить значение точки 3 дБ фильтра высоких частот. freqFilter находится в диапазоне от 0 (0,03 Гц) до 11 (10 кГц)
    void setHighFilter(int freqFilter);
    
    //Установить значение точки 3 дБ фильтра нижних частот. freqFilter находится в диапазоне от 0 (0,03 Гц) до 15 (1 МГц)
    void setLowFilter(int freqFilter);
    
    /*Установить тип фильтра по таблице:
    ntype   тип фильтра
      0    6 дБ ВЧ
      1    12 дБ ВЧ
      2    6 дБ полосовой
      3    6 дБ НЧ
      4    12 дБ НЧ
      5    никакой
    */
    void setTypeFilter(int nType);
    
    //Установить верньер некалиброванного входного смещения. [-1000 ≤ scale ≤ +1000]
    void setUncalInOffsetVernier(int scale);
    
    //Установить режим калибровки входного смещения. 0 = калибруется, 1 = не калибруется
    void setInOffsetCalMode(int calMode);
    
    //Включить ток смещения входа(val = 1) или выключить(val = 0)
    void turnInOffsetCurr(int val);
    
    //Включить напряжение смещения(val = 1) или выключить(val = 0)
    void turnBiasVolt(int val);
    
    //Установить режим калибровки чувствительности. 0 = калибруется, 1 = не калибруется
    void setSenCal(int calMode);
    
    //Установить текущий знак входного смещения. 0 = отрицательный, 1 = положительный
    void setInOffsetCurrSign(int sign);
    
    //Установить инверсию сигнала. 0 = не инвертированный, 1 = инвертированный
    void setSigInvertSense(int mode);
    
    //Заглушить внешний выход усилителя. 0 = выключить заглушку, 1 = включить заглушку
    void setBlanksOutAmplifier(int mode);
    
    //Установить верньер некалиброванной чувствительности. [0 ≤ scale ≤ 100]
    void setUncalSensVernier(int scale);
    
    //Установить уровень напряжения смещения в диапазоне [-5000 ≤ nLevel ≤ +5000] (от -5,000 В до +5,000 В)
    void setBiasVoltLVL(int nLevel);
    
    /*Установить режим усиления усилителя.
      mode  режим усиления
       0     тихий шум
       1     высокая пропускная способность
       2     низкий дрейф
    */
    void setGainMode(int mode);
    
    //Сбросить конденсаторы фильтра для устранения состояния перегрузки
    void resetFilCap();
    
    //Отключиться от прибора
    void closePort();
};
