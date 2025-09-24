#ifndef BUZZER_H
#define BUZZER_H
#include "../config.h"
#include <Arduino.h>

/**
 * @brief Buzzer para indicar status do sistema em intervalos de tempo padronizados.
 */
class BuzzerSistema {
  private:
    bool estadoBeep = false;
    bool estadoBeepInterno = false;
    int buzzerPin;
    int ledPin;
    unsigned long previousMillis = 0;
    unsigned long previousMillisInterno = 0;
    unsigned long interval;
    unsigned long beepDuration;
    int beepCount = 0;
    int maxBeeps = N_SISTEMAS;
    bool buzzOn;
    bool toneMode;
    bool beepPattern[N_SISTEMAS];
    int writeIndex = 0;
    int readIndex = 0;

    
  public:
    BuzzerSistema(int pin, int lPin, unsigned long intervalMillis, bool on, bool toneMode);
    
    void begin();

    void beep(unsigned long currentTime);

    void addSystem(bool booleano);
};

/**
 * @brief Buzzer que gera apitos rápidos sob demanda.
 */
class BuzzerEvento {
private:
  int pin;
  bool pinOn;
  unsigned long interval;
  unsigned long onDuration;
  bool beepOnTone;

  bool pinSeal = false;
  int maxTimes = 0;
  int timesCount = 0;
  unsigned long previousMillis = 0;

public:
  BuzzerEvento(int pino, bool on, unsigned long intervalo, unsigned long duracao, bool toneMode = false);

  void loop(unsigned long time);
  void add();

private:
  void seal();
  void unseal();
};

#endif
