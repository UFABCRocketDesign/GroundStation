#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#pragma once

#include <Arduino.h>

#ifndef LORA_MANAGER
#define LORA_MANAGER 1
#endif

#if LORA_MANAGER

#include <LoRa_E32.h>

class LoRaManager
{
private:
    Stream* loraSerial;
    LoRa_E32* e32Module;

    bool printEnabled;
    bool debugEnabled;

public:
    explicit LoRaManager(Stream& serialConfig);

    void initE32(LoRa_E32& e32Config);

    void setPrint(bool enable);
    void setDebug(bool enable);

    void send(const String& message);
    void sendLine(const String& message);

    bool available();
    bool readLine(String& message);

    bool waitForMessage(
        const String& expectedMessage,
        unsigned long timeoutMs
    );

    bool changeFrequency(
        uint8_t channel,
        uint8_t addh,
        uint8_t addl
    );

private:
    void printDebug(const __FlashStringHelper* message);
};

#endif
#endif