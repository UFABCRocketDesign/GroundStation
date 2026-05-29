// =======================================================
// URD Ground Station 2.0
// =======================================================

// ----------------------- INCLUDES ----------------------

#include "src/lib/config.h" 
#include "src/lib/pinout.h"

#include <LoRa_E32.h>
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

// Arduino Mega
SoftwareSerial LoRaSerial(LORA_RX, LORA_TX);

#endif

// ------------------------- GPS -------------------------
#define GPS_BAUD_RATE 9600

#if defined(ARDUINO_ARCH_ESP32)

// ESP32 / ESP32-S3
HardwareSerial GpsSerial(2);

#else

// Arduino Mega
SoftwareSerial GpsSerial(GPS_RX, GPS_TX);

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

    gs.startHandshake();
}

// =======================================================
// LOOP
// =======================================================

void loop()
{
    gs.update();
}
