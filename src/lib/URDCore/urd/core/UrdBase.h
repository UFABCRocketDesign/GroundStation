#ifndef URD_BASE_H
#define URD_BASE_H

#pragma once

#include <Arduino.h>
#include "src/lib/config.h"
#include "UrdProtocol.h"

#if URD_CORE_ENABLE

class UrdBase
{
protected:
    int baud;

    bool appStarted;
    bool readyReceived;

    int ledPin;
    bool ledEnabled;
    bool ledActivate;

    int buzzerPin;
    bool buzzerEnabled;
    bool buzzerToneMode;
    bool buzzerActivate;
    int buzzerFrequency;

public:
    explicit UrdBase(int baudRate = 115200);

    void begin();

    virtual void startHandshake();

    bool isAppStarted() const;
    bool isReadyReceived() const;

    void initLed(int ledPinConfig, bool ledOnConfig = true);
    void ledOn();
    void ledOff();
    void blinkLed(int times, int onTime, int offTime);

    void initBuzzer(
        int buzzerPinConfig,
        bool buzzerOnConfig = true,
        bool buzzerToneConfig = false,
        int buzzerFrequencyConfig = 2000
    );

    void playDot();
    void playDash();
    void buzzerOn();
    void buzzerOff();

protected:
    bool readAppMessage(String& message);

    virtual bool processReady(const String& message);
    virtual bool processReset(const String& message);
    virtual void processUnknownMessage(const String& message);

    void markAppStarted();
    void resetBoard();
};

#endif
#endif