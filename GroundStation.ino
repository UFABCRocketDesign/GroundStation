// =======================================================
// URD Ground Station 2.0
// =======================================================

// ----------------------- INCLUDES ----------------------

#include "src/lib/config.h" 
#include "src/lib/pinout.h"

#if (LORA_FREQUENCY_MODE == 900)
  #define FREQUENCY_900
  #define LORA_MAX_CHANNEL 69
#elif (LORA_FREQUENCY_MODE == 433)
  #define FREQUENCY_433
  #define LORA_MAX_CHANNEL 0
  #error "433Mhz Not implemented yet."
#else
  #error "Invalid LoRa Frequency. Use 900 or 433 MHz."
#endif

#if (LORA_MANAGER_E32 && LORA_MANAGER_E22)
  #error "Multiple LoRa Managers defined. Define only one of LORA_MANAGER_E32 or LORA_MANAGER_E22."
#endif

#if (LORA_MANAGER_E32)
  #include <LoRa_E32.h>
#elif (LORA_MANAGER_E22)
  #include <LoRa_E22.h>
#else
  #error "No LoRa Manager defined. Define LORA_MANAGER_E32 or LORA_MANAGER_E22."
#endif

#include <TinyGPS++.h>

#include "src/lib/URDGroundStation/URDGroundStation.h"

// ----------------------- SERIAL ------------------------
#define APP_BAUD_RATE 115200

// ------------------------ LoRa -------------------------
#define LORA_BAUD_RATE 9600

#if defined(ARDUINO_ARCH_ESP32)

// ESP32 / ESP32-S3
HardwareSerial LoRaSerial(1);

#else

// Arduino Mega (Placa Octa PTH)
HardwareSerial& LoRaSerial = Serial2;

#endif

// ------------------------- GPS -------------------------
#define GPS_BAUD_RATE 9600

#if defined(ARDUINO_ARCH_ESP32)

// ESP32 / ESP32-S3
HardwareSerial GpsSerial(2);

#else

// Arduino Mega (Placa Octa PTH)
HardwareSerial& GpsSerial  = Serial1;

#endif

// =======================================================
// OBJECTS
// =======================================================

#if (LORA_MANAGER)
LoRa_E32 e32Module(
    &LoRaSerial,
    AUX_LORA_PIN,
    M0_LORA_PIN,
    M1_LORA_PIN
);

LoRaManager loraManager(LoRaSerial);
#endif

#if (SD_CARD)
#if defined(ARDUINO_ARCH_ESP32)
SDManager sdManager(
    SD_CS,
    SD_SCK,
    SD_MISO,
    SD_MOSI
);
#else
SDManager sdManager(SD_CS);
#endif
#endif

#if (URD_GPS_ENABLE)
TinyGPSPlus gps;
#endif

UrdGroundStation gs(APP_BAUD_RATE);

// =======================================================
// SETUP
// =======================================================

void setup()
{
    gs.begin();

    delay(200);

#if defined(ARDUINO_ARCH_ESP32)
#if (LORA_MANAGER)
    LoRaSerial.begin(
        LORA_BAUD_RATE,
        SERIAL_8N1,
        LORA_TX,
        LORA_RX
    );
#endif

#if (URD_GPS_ENABLE)
    GpsSerial.begin(
        GPS_BAUD_RATE,
        SERIAL_8N1,
        GPS_RX,
        GPS_TX
    );
#endif

#else

#if (LORA_MANAGER)
    LoRaSerial.begin(LORA_BAUD_RATE);
#endif

#if (URD_GPS_ENABLE)
    GpsSerial.begin(GPS_BAUD_RATE);
#endif

#endif

#if (PRINT_MODE)
    gs.setPrint(true);
#else
    gs.setPrint(false);
#endif

#if (DEBUG_MODE)
    gs.setDebug(true);
#else
    gs.setDebug(false);
#endif

    gs.initLed(LED_PIN, true);
    gs.initBuzzer(BUZZ_PIN, BUZZ_ON, BUZZ_TONE, 2000);

#if (LORA_MANAGER)
    pinMode(AUX_LORA_PIN, INPUT);
    pinMode(M0_LORA_PIN, OUTPUT);
    pinMode(M1_LORA_PIN, OUTPUT);
    digitalWrite(M0_LORA_PIN, LOW);
    digitalWrite(M1_LORA_PIN, LOW);

    loraManager.setPrint(true);
    loraManager.setDebug(true);
    loraManager.initE32(e32Module);
    gs.initLoRaManager(loraManager);
#endif

#if (SD_CARD)

#if (PRINT_MODE)
    sdManager.setPrint(true);
#else
    sdManager.setPrint(false);
#endif

#if (DEBUG_MODE)
    sdManager.setDebug(true);
#else
    sdManager.setDebug(false);
#endif

    if (sdManager.begin(FILE_NAME))
    {
        gs.initSDManager(sdManager);
        sdManager.writeLine(F("URD Ground Station log started"));
    }
#endif

#if (URD_GPS_ENABLE)
    gs.initGps(
        GpsSerial,
        gps,
        3000UL,
        5000UL
    );
#endif

#if (LORA_SET_FREQUENCY_ON_STARTUP)
    if(loraManager.changeFrequency(
        LORA_BASE_CHAN,
        LORA_BASE_ADDH,
        LORA_BASE_ADDL
    ))
    {
        Serial.println(F("LoRa base frequency set on startup."));
        Serial.print(F("Frequency: "));
        Serial.print(LORA_BASE_CHAN);
        Serial.print(F(" MHz, ADDH=0x"));
        Serial.print(LORA_BASE_ADDH, HEX);
        Serial.print(F(", ADDL=0x"));
        Serial.println(LORA_BASE_ADDL, HEX);
    }
    else
    {
        Serial.println(F("Failed to set LoRa base frequency on startup."));
        Serial.print(F("Frequency: "));
        Serial.print(LORA_BASE_CHAN);
        Serial.print(F(" MHz, ADDH=0x"));
        Serial.print(LORA_BASE_ADDH, HEX);
        Serial.print(F(", ADDL=0x"));
        Serial.println(LORA_BASE_ADDL, HEX);
    }
#endif

    gs.startHandshake();
}

// =======================================================
// LOOP
// =======================================================

void loop()
{
    gs.update();
}
