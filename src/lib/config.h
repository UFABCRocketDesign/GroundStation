#pragma once

#include <Arduino.h>


// ========================
// Configurações
// ========================
#define BUZZER 0
#define LED_RGB 1
#define LORA 1
#define GPS 0

#define URD_APP 1
#define IGN_MODE (URD_APP && 1)

#define SD_CARD 0
#define SD_NAME "GS"

#define N_SISTEMAS (SD_CARD)

#if defined(ARDUINO_ARCH_ESP32)
#if LED_RGB
#define LED_R 12
#endif

#if BUZZER
#define BUZZ_PIN 27
#define BUZZ_ON 1
#else
#define BUZZ_PIN -1
#define BUZZ_ON 1
#endif

#if SD_CARD
#define SD_CS 5  
#else
#define SD_CS -1  
#endif


#else // --------------------

#if LED_RGB
#define LED_R 32
#define LED_G 36
#define LED_B 34
#else
#define LED_R 32
#endif

#if BUZZER
#define BUZZ_PIN 22
#define BUZZ_ON 1
#else
#define BUZZ_PIN -1
#define BUZZ_ON 1
#endif

#if SD_CARD
#define SD_CS 53  
#else
#define SD_CS -1  
#endif



#endif