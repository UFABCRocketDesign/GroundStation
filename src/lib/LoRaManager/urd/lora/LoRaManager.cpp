#include "LoRaManager.h"

#if LORA_MANAGER

LoRaManager::LoRaManager(Stream& serialConfig)
{
    loraSerial = &serialConfig;
    e32Module = nullptr;

    printEnabled = true;
    debugEnabled = false;
}

void LoRaManager::initE32(LoRa_E32& e32Config)
{
    e32Module = &e32Config;
    e32Module->begin();
}

void LoRaManager::setPrint(bool enable)
{
    printEnabled = enable;
}

void LoRaManager::setDebug(bool enable)
{
    debugEnabled = enable;
}

void LoRaManager::send(const String& message)
{
    if (loraSerial == nullptr)
    {
        return;
    }

    loraSerial->print(message);
}

void LoRaManager::sendLine(const String& message)
{
    if (loraSerial == nullptr)
    {
        return;
    }

    loraSerial->println(message);
}

bool LoRaManager::available()
{
    if (loraSerial == nullptr)
    {
        return false;
    }

    return loraSerial->available();
}

bool LoRaManager::readLine(String& message)
{
    if (loraSerial == nullptr)
    {
        return false;
    }

    if (!loraSerial->available())
    {
        return false;
    }

    message = loraSerial->readStringUntil('\n');
    message.trim();

    return message.length() > 0;
}

bool LoRaManager::waitForMessage(
    const String& expectedMessage,
    unsigned long timeoutMs
)
{
    if (loraSerial == nullptr)
    {
        printDebug(F("[LORA DEBUG] LoRa serial not initialized."));
        return false;
    }

    unsigned long startTime = millis();

    while (millis() - startTime < timeoutMs)
    {
        String receivedMessage;

        if (readLine(receivedMessage))
        {
            receivedMessage.trim();

            if (debugEnabled)
            {
                Serial.print(F("[LORA DEBUG] received: "));
                Serial.println(receivedMessage);
            }

            if (receivedMessage == expectedMessage)
            {
                return true;
            }
        }

        delay(1);
    }

    if (debugEnabled)
    {
        Serial.print(F("[LORA DEBUG] timeout waiting for: "));
        Serial.println(expectedMessage);
    }

    return false;
}

bool LoRaManager::changeFrequency(
    uint8_t channel,
    uint8_t addh,
    uint8_t addl
)
{
    if (e32Module == nullptr)
    {
        printDebug(F("[LORA MANAGER DEBUG] E32 module not initialized."));
        return false;
    }

    if (debugEnabled)
    {
        Serial.print(F("[LORA MANAGER DEBUG] Applying direct config: CHAN="));
        Serial.print(channel);

        Serial.print(F(" ADDH=0x"));
        if (addh < 0x10) Serial.print('0');
        Serial.print(addh, HEX);

        Serial.print(F(" ADDL=0x"));
        if (addl < 0x10) Serial.print('0');
        Serial.println(addl, HEX);
    }

    ResponseStructContainer configContainer = e32Module->getConfiguration();

    if (configContainer.status.code != SUCCESS || configContainer.data == nullptr)
    {
        if (debugEnabled)
        {
            Serial.print(F("[LORA DEBUG] getConfiguration failed: "));
            Serial.println(configContainer.status.getResponseDescription());
        }

        if (configContainer.status.code == SUCCESS)
        {
            configContainer.close();
        }
        return false;
    }

    Configuration configuration = *(Configuration*) configContainer.data;

    configuration.CHAN = channel;
    configuration.ADDH = addh;
    configuration.ADDL = addl;
    configuration.SPED.airDataRate = AIR_DATA_RATE_101_192;
    configuration.OPTION.fec = FEC_1_ON;
    configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
    configuration.OPTION.transmissionPower = POWER_20;
    configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;

    ResponseStatus response =
        e32Module->setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);

    configContainer.close();

    if (debugEnabled)
    {
        Serial.print(F("[LORA DEBUG] setConfiguration: "));
        Serial.println(response.getResponseDescription());
    }

    if (response.code != SUCCESS)
    {
        return false;
    }

    delay(500);

    ResponseStructContainer checkContainer = e32Module->getConfiguration();

    if (checkContainer.status.code != SUCCESS || checkContainer.data == nullptr)
    {
        if (debugEnabled)
        {
            Serial.print(F("[LORA DEBUG] getConfiguration check failed: "));
            Serial.println(checkContainer.status.getResponseDescription());
        }

        if (checkContainer.status.code == SUCCESS)
        {
            checkContainer.close();
        }
        return false;
    }

    Configuration checkConfig = *(Configuration*) checkContainer.data;

    bool success =
        checkConfig.CHAN == channel &&
        checkConfig.ADDH == addh &&
        checkConfig.ADDL == addl;

    if (debugEnabled)
    {
        Serial.print(F("[LORA DEBUG] Readback CHAN="));
        Serial.print(checkConfig.CHAN);

        Serial.print(F(" ADDH=0x"));
        if (checkConfig.ADDH < 0x10) Serial.print('0');
        Serial.print(checkConfig.ADDH, HEX);

        Serial.print(F(" ADDL=0x"));
        if (checkConfig.ADDL < 0x10) Serial.print('0');
        Serial.println(checkConfig.ADDL, HEX);
    }

    checkContainer.close();

    if (printEnabled)
    {
        Serial.println(success ? F("LoRa config ok") : F("LoRa config err"));
    }

    if (success)
    {
        delay(200); // Settling time for hardware stability
    }

    return success;
}

bool LoRaManager::changeFrequency(const String& message) {
    String m = message; m.replace("\\t", "\t"); m.trim();
    int t = m.indexOf('\t'); if (t < 0) t = m.indexOf(' '); if (t < 0) return false;
    long a = m.substring(t + 1).toInt();
    return changeFrequency(m.substring(4, t).toInt() - 862, a >> 8, a & 0xFF);
}

void LoRaManager::printDebug(const __FlashStringHelper* message)
{
    if (debugEnabled)
    {
        Serial.println(message);
    }
}

#endif