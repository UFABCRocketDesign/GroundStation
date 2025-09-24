#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "../config.h"

#if (SD_CARD)
#include <SPI.h>
#include <SD.h>
#endif

#if (LED_RGB)
enum Colors { 
  white   = 0b111,
  magenta = 0b101,
  blue    = 0b001,
  cyan    = 0b011,
  green   = 0b010,
  yellow  = 0b110,
  red     = 0b100,
  black   = 0b000 
};
#endif

/**
 * @brief Classe utilitária para LORA, SD, GPS, LED RGB, RESET e MAX_VALUE.
 * 
 * 
 * Habilite os módulos desejados com defines:
 * - #define LORA
 * - #define GPS
 * - #define SD_CARD
 * - #define LED_RGB
 * 
 * Aceita ESPs ou Arduino Mega
 */
class Utils {
  private:
    int sdCsPin;
    int ledR, ledG, ledB;
    unsigned long lastMillis;
    float maxSaved;

#if (SD_CARD)
    File dataFile;
    char nomeSD[13]; // espaço suficiente para "joao0000.txt" + null terminator
#endif

#if (LED_RGB)
    void rgb(bool r, bool g, bool b);
#endif

  public:
  /**
 * @brief Construtor da classe Utils
 * 
 * @param sdPin  Pino CS do SD (ou -1 se não usar)
 * @param rPin   Pino do LED vermelho (ou -1 se não usar)
 * @param gPin   Pino do LED verde (ou -1 se não usar)
 * @param bPin   Pino do LED azul (ou -1 se não usar)
 * 
 */
    Utils(int sdPin = -1, int rPin = -1, int gPin = -1, int bPin = -1);

#if (LORA)
    void beginLoRa(unsigned long baud);
    void sendLoRa(const String& msg);
    bool availableLoRa();
    String readLoRaLine();
    bool interval(unsigned long ms);
#endif

#if (SD_CARD)
    bool beginSD(const String& baseName);
    bool logSD(const String& data);
#endif

#if (LED_RGB)
    #define L_ON LOW

    void setColor(Colors color);
    void rgbStart();
#endif

#if (GPS)
    void beginGPS(unsigned long baud);
    String readGPSLine();
#endif

    void apitar(int tempo, int apitos, int buzzPin, bool buzzOn);
    void reset();
    void updateMax(float val);
    float getMaxValue();
    void resetMaxValue();
};

#endif
