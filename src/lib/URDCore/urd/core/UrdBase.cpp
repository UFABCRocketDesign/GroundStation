#include "UrdBase.h"

#if URD_CORE_ENABLE

#if defined(ARDUINO_ARCH_AVR)
#include <avr/wdt.h>
#endif

UrdBase::UrdBase(int baudRate)
{
    baud = baudRate;

    appStarted = false;
    readyReceived = false;

    ledPin = -1;
    ledEnabled = false;
    ledActivate = true;

    buzzerPin = -1;
    buzzerEnabled = false;
    buzzerToneMode = false;
    buzzerActivate = true;
    buzzerFrequency = 2000;
}

void UrdBase::begin()
{
    Serial.begin(baud);
    delay(500);

#if URD_LED_ENABLE
    if (ledEnabled)
    {
        blinkLed(3, 100, 100);
    }
#endif

#if URD_BUZZER_ENABLE
    if (buzzerEnabled)
    {
        playDot();
        playDash();
        playDot();
    }
#endif
}

void UrdBase::startHandshake()
{
    appStarted = false;
    readyReceived = false;
    unsigned long timer = millis();

    Serial.println(F("Starting URD App Serial Communication..."));

    while (!appStarted)
    {
#if URD_BUZZER_ENABLE
        if (buzzerEnabled && !readyReceived && (millis() - timer >= 5000UL))
        {
            playDot();
            timer = millis();
        }
#endif

        String message;

        if (readAppMessage(message))
        {
            if (processReady(message)) { continue; }
            if (processReset(message)) { continue; }

            processUnknownMessage(message);
        }

        delay(10);
    }

    ledOn();
}

bool UrdBase::readAppMessage(String& message)
{
    if (!Serial.available())
    {
        return false;
    }

    message = Serial.readStringUntil('\n');
    message.trim();

    return message.length() > 0;
}

bool UrdBase::processReady(const String& message)
{
    if (message != UrdProtocol::READY)
    {
        return false;
    }

    Serial.println(F("OK"));
    readyReceived = true;

#if URD_BUZZER_ENABLE
    if (buzzerEnabled)
    {
        playDot();
        playDot();
    }
#endif

    return true;
}

bool UrdBase::processReset(const String& message)
{
    if (message != UrdProtocol::RESET)
    {
        return false;
    }

    Serial.println(F("RESETTING"));
    delay(100);

    resetBoard();

    return true;
}

void UrdBase::processUnknownMessage(const String& message)
{
    (void)message;
}

void UrdBase::markAppStarted()
{
    appStarted = true;
    ledOn();
}

bool UrdBase::isAppStarted() const
{
    return appStarted;
}

bool UrdBase::isReadyReceived() const
{
    return readyReceived;
}

void UrdBase::initLed(int ledPinConfig, bool ledOnConfig)
{
#if URD_LED_ENABLE
    ledEnabled = true;
    ledPin = ledPinConfig;
    ledActivate = ledOnConfig;

    if (ledEnabled && ledPin >= 0)
    {
        pinMode(ledPin, OUTPUT);
        ledOff();
    }
#else
    (void)ledPinConfig;
    (void)ledOnConfig;
#endif
}

void UrdBase::ledOn()
{
#if URD_LED_ENABLE
    if (!ledEnabled || ledPin < 0)
    {
        return;
    }

    digitalWrite(ledPin, ledActivate);
#endif
}

void UrdBase::ledOff()
{
#if URD_LED_ENABLE
    if (!ledEnabled || ledPin < 0)
    {
        return;
    }

    digitalWrite(ledPin, !ledActivate);
#endif
}

void UrdBase::blinkLed(int times, int onTime, int offTime)
{
#if URD_LED_ENABLE
    if (!ledEnabled || ledPin < 0)
    {
        return;
    }

    for (int i = 0; i < times; i++)
    {
        ledOn();
        delay(onTime);

        ledOff();
        delay(offTime);
    }
#else
    (void)times;
    (void)onTime;
    (void)offTime;
#endif
}

void UrdBase::initBuzzer(
    int buzzerPinConfig,
    bool buzzerOnConfig,
    bool buzzerToneConfig,
    int buzzerFrequencyConfig
)
{
#if URD_BUZZER_ENABLE
    buzzerEnabled = true;
    buzzerPin = buzzerPinConfig;
    buzzerToneMode = buzzerToneConfig;
    buzzerActivate = buzzerOnConfig;
    buzzerFrequency = buzzerFrequencyConfig;

    if (buzzerEnabled && buzzerPin >= 0)
    {
        pinMode(buzzerPin, OUTPUT);
        buzzerOff();
    }
#else
    (void)buzzerPinConfig;
    (void)buzzerOnConfig;
    (void)buzzerToneConfig;
    (void)buzzerFrequencyConfig;
#endif
}

void UrdBase::playDot()
{
#if URD_BUZZER_ENABLE
    if (!buzzerEnabled || buzzerPin < 0)
    {
        return;
    }

    buzzerOn();
    delay(80);

    buzzerOff();
    delay(80);
#endif
}

void UrdBase::playDash()
{
#if URD_BUZZER_ENABLE
    if (!buzzerEnabled || buzzerPin < 0)
    {
        return;
    }

    buzzerOn();
    delay(250);

    buzzerOff();
    delay(120);
#endif
}

void UrdBase::buzzerOn()
{
#if URD_BUZZER_ENABLE
    if (!buzzerEnabled || buzzerPin < 0)
    {
        return;
    }

    if (buzzerToneMode)
    {
        tone(buzzerPin, buzzerFrequency);
    }
    else
    {
        digitalWrite(buzzerPin, buzzerActivate);
    }
#endif
}

void UrdBase::buzzerOff()
{
#if URD_BUZZER_ENABLE
    if (!buzzerEnabled || buzzerPin < 0)
    {
        return;
    }

    if (buzzerToneMode)
    {
        noTone(buzzerPin);
    }
    else
    {
        digitalWrite(buzzerPin, !buzzerActivate);
    }
#endif
}

void UrdBase::resetBoard()
{
#if defined(ARDUINO_ARCH_ESP32)
    ESP.restart();
#elif defined(ARDUINO_ARCH_AVR)
    wdt_enable(WDTO_15MS);
    while (true) {}
#else
    Serial.println(F("Board reset not supported on this platform."));
#endif
}

#endif
