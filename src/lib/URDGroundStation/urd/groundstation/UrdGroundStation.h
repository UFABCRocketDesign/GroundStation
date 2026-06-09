#ifndef URD_GROUNDSTATION_H
#define URD_GROUNDSTATION_H

#pragma once

#include <Arduino.h>
#include "../../../URDCore/URDCore.h"

#if LORA_MANAGER
#include "../../../LoRaManager/LoRaManager.h"
#endif

#if SD_CARD
#include "../../../SDManager/SDManager.h"
#endif

#if URD_GROUNDSTATION_ENABLE

#if URD_GPS_ENABLE
#if defined(__has_include)
  #if __has_include(<TinyGPS++.h>)
    #include <TinyGPS++.h>
  #elif __has_include(<TinyGPSPlus.h>)
    #include <TinyGPSPlus.h>
  #else
    #error "TinyGPSPlus library not found. Install TinyGPSPlus/TinyGPS++."
  #endif
#else
  #include <TinyGPS++.h>
#endif
#endif

class UrdGroundStation : public UrdBase
{
private:
    bool gsStarted;

    bool printEnabled;
    bool debugEnabled;

#if LORA_MANAGER
    LoRaManager* loraManager = nullptr;

    bool loraForcedRequested;
    bool loraChangeRequested;

    bool loraWaitingVals;
    bool loraWaitingFcConfirmation;
    bool loraWaitingFcFinalConfirmation;

    unsigned long loraChangeLastTime;

    String loraChangeCommand;

    String loraChangeChanText;
    String loraChangeAddressHex;

    uint8_t loraChangeChan;
    uint8_t loraChangeAddh;
    uint8_t loraChangeAddl;

    uint8_t previousChan;
    uint8_t previousAddh;
    uint8_t previousAddl;
    uint8_t currentChan;
    uint8_t currentAddh;
    uint8_t currentAddl;
#endif

#if SD_CARD
    SDManager* sdManager = nullptr;
#endif

#if URD_GPS_ENABLE
    Stream* gpsSerial = nullptr;
    TinyGPSPlus* gpsManager = nullptr;

    bool gpsAvailable;
    bool gpsReceiving;
    bool gpsEverReceivedData;

    String gpsLatitude;
    String gpsLongitude;

    unsigned long lastGpsFixTime;
    unsigned long lastGpsCharTime;
    unsigned long gpsFixTimeoutMs;
    unsigned long gpsNoDataTimeoutMs;
#endif

public:
    explicit UrdGroundStation(unsigned long baudRate = 115200UL);

    void setPrint(bool enable);
    void setDebug(bool enable);

    void startHandshake() override;
    void update();

#if LORA_MANAGER
    void initLoRaManager(LoRaManager& manager);
#endif

#if SD_CARD
    void initSDManager(SDManager& manager);
#endif

#if URD_GPS_ENABLE
    void initGps(
        Stream& gpsStream,
        TinyGPSPlus& gps,
        unsigned long fixTimeoutMs = 3000UL,
        unsigned long noDataTimeoutMs = 5000UL
    );

    bool updateGps();

    void setGpsCoordinates(const String& latitude, const String& longitude);
    void clearGpsCoordinates();

    bool hasGpsFix() const;
    bool isGpsReceiving() const;
    bool hasGpsEverReceivedData() const;

    String getGpsLatitude() const;
    String getGpsLongitude() const;

    unsigned long getLastGpsFixTime() const;
    unsigned long getLastGpsCharTime() const;
    unsigned long getGpsCharsProcessed() const;
    unsigned long getGpsSentencesWithFix() const;
    unsigned long getGpsFailedChecksum() const;
    unsigned long getGpsLocationAge() const;

    uint32_t getGpsSatellites() const;
    float getGpsHdop() const;

    void printGpsStatus(Stream& out);
#endif

    bool isGsStarted() const;

protected:
    bool processGpsCoords(const String& message);

    void processAppSerialMessages();
    void processLoraMessages();
    
    void processLoraPacket(const String& packet);

#if LORA_MANAGER
    bool processLoraChangeFrequency(const String& message);
    bool isValidLoraChangeCommand(const String& command);
    bool isDecimalText(const String& text);
    bool isHexaText(const String& text);
    void resetLoraChangeState();


    bool splitLoraChangeCommand(
        const String& command,
        String& chanText,
        String& addressText
    );

    bool isValidLoraChanText(const String& chanText);
    bool isValidLoraAddressText(const String& addressText);

    bool decodeLoraChangeCommand(const String& command);

    String buildFcConfirmationPayload() const;

    bool applyDecodedLoraConfig();

    void checkLoraChangeTimeout();

#endif

    void printInfo(const __FlashStringHelper* message);
    void debugInfo(const __FlashStringHelper* message);
    void debugInfo(const String& message);
};

#endif
#endif

