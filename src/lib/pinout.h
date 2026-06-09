#ifndef PINOUT_H
#define PINOUT_H
#ifndef NOME_PLACA
  #error "Inclua o config antes do pinout.h"
#endif
// ============================================================================
// PLACA MAIN OCTA PTH
// ============================================================================
#if defined(PLACA_MAIN_OCTA_PTH)

  // ------------------------------ CARTAO SD ------------------------------//
  #if (SD_CARD)
    #define SD_CS 53
  #endif

  // ------------------------------ BUZZER ------------------------------//
#if (URD_BUZZER_ENABLE)
    #define BUZZ_PIN 22
#else
    #define BUZZ_PIN -1
#endif
    #define BUZZ_TONE 1
    #define BUZZ_ON LOW

  // ------------------------------ DHT ------------------------------//
  #if (USE_DHT)
    #define DHT_PIN 3
  #endif

  // ------------------------------ SIGNAL ------------------------------//
  #if (SIGNAL)
    // #define SIGNAL_PIN 
  #endif

  // ------------------------------ LED ------------------------------//
  #if (SD_LED || BUZZER_SILENT_MODE || URD_LED_ENABLE)
    #define LED_PIN 9  
  #else
    #define LED_PIN -1
  #endif

  // ------------------------------ CAM ------------------------------//
  #if (CAM)
    // #define CAM_PIN
  #endif

  // ------------------------------ LORA_MODE ------------------------------//
  #if (LORA_MODE)
    #define M0_LORA_PIN 12
    #define M1_LORA_PIN 11
    #define AUX_LORA_PIN A8
  #endif

  // ------------------------------ GPS ------------------------------//
  #if (GPS_MODE)
    #define GPS_RX 17
    #define GPS_TX 16
  #endif

  // ------------------------------ PARAQUEDAS / PQDs ------------------------------//
  #if (Parachute_FirstNormalEvent)
    #define IGN_1 A0
    #define HEAL_1 A1
  #endif

  #if (Parachute_FirstBackupEvent)
    #define IGN_2 A2
    #define HEAL_2 A3
  #endif

  #if (Parachute_SecondNormalEvent)
    #define IGN_3 A4
    #define HEAL_3 A5
  #endif

  #if (Parachute_SecondBackupEvent)
    #define IGN_4 A6
    #define HEAL_4 A7
  #endif

// ============================================================================
// ESSENTIALS V1
// ============================================================================
#elif defined(PLACA_ESSENTIALS_V1)

  // ------------------------------ I2C ------------------------------//
    #define SDA_PIN 21
    #define SCL_PIN 22

  // ------------------------------ CARTAO SD ------------------------------//
  #if (SD_CARD)
    #define SD_MOSI 23
    #define SD_MISO 19
    #define SD_SCK 18
    #define SD_CS 5
  #endif

  // ------------------------------ BUZZER ------------------------------//
#if (URD_BUZZER_ENABLE)
    #define BUZZ_PIN 27
#else
    #define BUZZ_PIN -1
#endif
    #define BUZZ_TONE 0
    #define BUZZ_ON HIGH

  // ------------------------------ DHT ------------------------------//
  #if (USE_DHT)
    #define DHT_PIN 14
  #endif

  // ------------------------------ SIGNAL ------------------------------//
  #if (SIGNAL)
    #define SIGNAL_PIN 34
  #endif

  // ------------------------------ LED ------------------------------//
  #if (SD_LED || BUZZER_SILENT_MODE || URD_LED_ENABLE)
    #define LED_PIN 12
  #else
    #define LED_PIN -1
  #endif

  // ------------------------------ CAM ------------------------------//
  #if (CAM)
    #define CAM_PIN 7
  #endif

  // ------------------------------ LORA_MODE ------------------------------//
  #if (LORA_MODE)
    #define M0_LORA_PIN 26
    #define M1_LORA_PIN 25
    #define AUX_LORA_PIN 35
    #define LORA_RX 33
    #define LORA_TX 32
  #endif

  // ------------------------------ GPS ------------------------------//
  #if (GPS_MODE)
    #define GPS_RX 17
    #define GPS_TX 16
  #endif

// ============================================================================
// PLACA MAIN PTH
// ============================================================================
#elif defined(PLACA_MAIN_PTH)

  // ------------------------------ I2C ------------------------------//
    #define SDA_PIN 8
    #define SCL_PIN 9

  // ------------------------------ CARTAO SD ------------------------------//
  #if (SD_CARD)
    #define SD_MOSI 11
    #define SD_MISO 13
    #define SD_SCK 12
    #define SD_CS 14
  #endif

  // ------------------------------ BUZZER ------------------------------//
#if (URD_BUZZER_ENABLE)
    #define BUZZ_PIN 42
#else
    #define BUZZ_PIN -1
#endif
    #define BUZZ_TONE 0
    #define BUZZ_ON LOW

  // ------------------------------ DHT ------------------------------//
  #if (USE_DHT)
    // #define DHT_PIN 
  #endif

  // ------------------------------ SIGNAL ------------------------------//
  #if (SIGNAL)
    // #define SIGNAL_PIN
  #endif

  // ------------------------------ LED ------------------------------//
  #if (SD_LED || BUZZER_SILENT_MODE || URD_LED_ENABLE)
    #define LED_PIN 35
  #else
    #define LED_PIN -1
  #endif

  // ------------------------------ CAM ------------------------------//
  #if (CAM)
    // #define CAM_PIN
  #endif

  // ------------------------------ LORA_MODE ------------------------------//
  #if (LORA_MODE)
    #define M0_LORA_PIN 18
    #define M1_LORA_PIN 17
    #define AUX_LORA_PIN 41
    #define LORA_RX 16
    #define LORA_TX 15
  #endif

  // ------------------------------ GPS ------------------------------//
  #if (GPS_MODE)
    #define GPS_RX 40
    #define GPS_TX 39
  #endif

  // ------------------------------ PARAQUEDAS / PQDs ------------------------------//
  #if (Parachute_FirstNormalEvent)
    #define IGN_1 4
    #define HEAL_1 5
  #endif

  #if (Parachute_FirstBackupEvent)
    #define IGN_2 6
    #define HEAL_2 7
  #endif

  #if (Parachute_SecondNormalEvent)
    #define IGN_3 21
    #define HEAL_3 2
  #endif

  #if (Parachute_SecondBackupEvent)
    #define IGN_4 47
    #define HEAL_4 1
  #endif

// ============================================================================
// PLACA MAIN SMD
// ============================================================================
#elif defined(PLACA_MAIN_SMD)

  // ------------------------------ SENSORS ------------------------------//
  #define USE_ICM20948_ACCEL 1
  #define USE_ICM20948_GYRO 1
  #define USE_ICM20948_MAG 1

  // ------------------------------ BATTERY MONITOR ------------------------------//
  #if (BATTERY_MONITOR)
    #define BATTERY_PIN 14
  #endif

  // ------------------------------ I2C ------------------------------//
    #define SDA_PIN 41
    #define SCL_PIN 42

  // ------------------------------ CARTAO SD ------------------------------//
  #if (SD_CARD)
    #define SD_MOSI 37
    #define SD_MISO 35
    #define SD_SCK  36
    #define SD_CS   38
  #endif

  // ------------------------------ BUZZER ------------------------------//
#if (URD_BUZZER_ENABLE)
    #define BUZZ_PIN 2
#else
    #define BUZZ_PIN -1
#endif
    #define BUZZ_TONE 1
    #define BUZZ_ON HIGH

  // ------------------------------ DHT ------------------------------//
  #if (USE_DHT)
    #define DHT_PIN 14
  #endif

  // ------------------------------ SIGNAL ------------------------------//
  #if (SIGNAL)
    #define SIGNAL_PIN 34
  #endif

  // ------------------------------ LED ------------------------------//
  #if (SD_LED || BUZZER_SILENT_MODE || URD_LED_ENABLE)
    #define LED_PIN 1
  #else
    #define LED_PIN -1
  #endif

  
  // ------------------------------ CAM ------------------------------//
  #if (CAM)
    #define CAM_PIN 7
  #endif

  // ------------------------------ LORA_MODE ------------------------------//
  #if (LORA_MODE)
    #define M0_LORA_PIN 12
    #define M1_LORA_PIN 11
    #define AUX_LORA_PIN 3
    #define LORA_RX 10
    #define LORA_TX 9
  #endif

  // ------------------------------ GPS ------------------------------//
  #if (GPS_MODE)
    #define GPS_RX 47
    #define GPS_TX 21
  #endif

  // ------------------------------ PARAQUEDAS / PQDs ------------------------------//
  #if (Parachute_FirstNormalEvent)
    #define IGN_1 8
    #define HEAL_1 18
  #endif

  #if (Parachute_FirstBackupEvent)
    #define IGN_2 17
    #define HEAL_2 16
  #endif

  #if (Parachute_SecondNormalEvent)
    #define IGN_3 15
    #define HEAL_3 7
  #endif

  #if (Parachute_SecondBackupEvent)
    #define IGN_4 6
    #define HEAL_4 5
  #endif
// ============================================================================
// PLACA MAIN IC
// ============================================================================
#elif defined(PLACA_MAIN_IC)
  // ------------------------------ SENSORS ------------------------------//
  #define USE_BME280 1
  #define USE_ICM20948_ACCEL 1
  #define USE_ICM20948_GYRO 1
  #define USE_ICM20948_MAG 1

  // ------------------------------ BATTERY MONITOR ------------------------------//
  #if (BATTERY_MONITOR)
    #define BATTERY_PIN 10
  #endif

  // ------------------------------ I2C ------------------------------//
    #define SDA_PIN 42
    #define SCL_PIN 41

  // ------------------------------ CARTAO SD ------------------------------//
  #if (SD_CARD)
    #define SD_MOSI 11
    #define SD_MISO 13
    #define SD_SCK 12
    #define SD_CS 14
  #endif

  // ------------------------------ BUZZER ------------------------------//
#if (URD_BUZZER_ENABLE)
    #define BUZZ_PIN 37
  #else
    #define BUZZ_PIN -1
#endif
    #define BUZZ_TONE 0
    #define BUZZ_ON HIGH

  // ------------------------------ USE_DHT ------------------------------//
  #if (USE_DHT)
    // #define DHT_PIN
  #endif

  // ------------------------------ SIGNAL ------------------------------//
  #if (SIGNAL)
    // #define SIGNAL_PIN
  #endif

  // ------------------------------ LED ------------------------------//
  #if (SD_LED || BUZZER_SILENT_MODE || URD_LED_ENABLE)
    #define LED_PIN 1
  #else
    #define LED_PIN -1
  #endif


  // ------------------------------ CAM ------------------------------//
  #if (CAM)
    // #define CAM_PIN
  #endif

  // ------------------------------ LORA_MODE ------------------------------//
  #if (LORA_MODE)
    #define M0_LORA_PIN 16
    #define M1_LORA_PIN 15
    #define AUX_LORA_PIN 8
    #define LORA_RX 17
    #define LORA_TX 18
  #endif

  // ------------------------------ GPS ------------------------------//
  #if (GPS_MODE)
    #define GPS_RX 47
    #define GPS_TX 21
  #endif

  // ------------------------------ PARAQUEDAS / PQDs ------------------------------//
  #if (Parachute_FirstNormalEvent)
    #define IGN_1 6
    #define HEAL_1 7
  #endif

  #if (Parachute_FirstBackupEvent)
    #define IGN_2 4
    #define HEAL_2 5
  #endif

  #if (Parachute_SecondNormalEvent)
    #define IGN_3 36
    #define HEAL_3 9
  #endif

  #if (Parachute_SecondBackupEvent)
    #define IGN_4 35
    #define HEAL_4 3
  #endif

#else
  #error "Placa selecionada sem bloco correspondente no pinout.h"
#endif

#endif