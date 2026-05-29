#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#pragma once

#include <Arduino.h>

#ifndef SD_CARD
#define SD_CARD 1
#endif

#if SD_CARD

#include <SPI.h>
#include <SD.h>

#ifndef FILE_NAME
#define FILE_NAME "DATA"
#endif

class SDManager
{
private:
    int csPin;
    int sckPin;
    int misoPin;
    int mosiPin;

#if defined(ARDUINO_ARCH_ESP32)
    SPIClass spiBus;
#endif

    bool started;
    bool printEnabled;
    bool debugEnabled;

    char nameSD[40];

    unsigned long lastSaveTimeUs;

public:
    SDManager(
        int csPinConfig,
        int sckPinConfig = -1,
        int misoPinConfig = -1,
        int mosiPinConfig = -1
    );

    void setPrint(bool enable);
    void setDebug(bool enable);

    bool begin(const String& baseName = FILE_NAME);

    bool write(const String& data);
    bool writeLine(const String& data);

    bool read(Print& output, const char* path = nullptr, unsigned long maxReadMs = 5000);

    bool listFiles(Print& output, const char* dirname = "/", uint8_t levels = 0);

    bool isReady() const;
    const char* getFileName() const;
    unsigned long getLastSaveTimeUs() const;

private:
    bool beginSdCard();
    void createSdFileName(const String& baseName);

    bool openAndWrite(const String& data, bool newLine);

    String normalizePath(const char* path) const;
    File openForWrite(const char* path);

    void listDirectory(File dir, Print& output, uint8_t levels);
};

#endif
#endif