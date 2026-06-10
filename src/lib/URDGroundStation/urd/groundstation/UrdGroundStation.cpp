#include "UrdGroundStation.h"

#if URD_GROUNDSTATION_ENABLE


UrdGroundStation::UrdGroundStation(unsigned long baudRate)
    : UrdBase(baudRate)
{
    gsStarted = false;

    printEnabled = false;
    debugEnabled = false;

#if LORA_MANAGER
    loraManager = nullptr;

    loraForcedRequested = false;
    loraChangeRequested = false;

    loraWaitingVals = false;
    loraWaitingFcConfirmation = false;
    loraWaitingFcFinalConfirmation = false;

    loraChangeLastTime = 0;

    loraChangeCommand = "";

    loraChangeChanText = "";
    loraChangeAddressHex = "";

    loraChangeChan = 0;
    loraChangeAddh = 0;
    loraChangeAddl = 0;

    previousChan = LORA_CHAN;
    previousAddh = LORA_ADDH;
    previousAddl = LORA_ADDL;
    currentChan = LORA_CHAN;
    currentAddh = LORA_ADDH;
    currentAddl = LORA_ADDL;
#endif

#if SD_CARD
    sdManager = nullptr;
#endif

#if URD_GPS_ENABLE
    gpsSerial = nullptr;
    gpsManager = nullptr;

    gpsAvailable = false;
    gpsReceiving = false;
    gpsEverReceivedData = false;

    gpsLatitude = "~";
    gpsLongitude = "~";

    lastGpsFixTime = 0;
    lastGpsCharTime = 0;

    gpsFixTimeoutMs = 3000UL;
    gpsNoDataTimeoutMs = 5000UL;
#endif
}

void UrdGroundStation::setPrint(bool enable)
{
    printEnabled = enable;
}

void UrdGroundStation::setDebug(bool enable)
{
    debugEnabled = enable;
}

void UrdGroundStation::printInfo(const __FlashStringHelper* message)
{
    if (printEnabled)
    {
        Serial.print(F("[GS] "));
        Serial.println(message);
    }
}

void UrdGroundStation::debugInfo(const __FlashStringHelper* message)
{
    if (debugEnabled)
    {
        Serial.print(F("[DEBUG][GS] "));
        Serial.println(message);
    }
}

void UrdGroundStation::debugInfo(const String& message)
{
    if (debugEnabled)
    {
        Serial.print(F("[DEBUG][GS] "));
        Serial.println(message);
    }
}

void UrdGroundStation::startHandshake()
{
    appStarted = false;
    readyReceived = false;
    gsStarted = false;

    unsigned long timer = millis();

    Serial.println(F("Starting URD Ground Station..."));
#if URD_BUZZER_ENABLE
    playDot();
    playDot();
    playDot();
#endif

    while (!appStarted)
    {
#if URD_BUZZER_ENABLE
        if (buzzerEnabled && !readyReceived && ((millis() - timer) >= 5000UL))
        {
            playDot();
            timer = millis();
        }
#endif

#if URD_GPS_ENABLE
        updateGps();
#endif

        String message;

        if (readAppMessage(message))
        {
            if (processReady(message)) { continue; }
            if (processReset(message)) { continue; }
            if (processGpsCoords(message)) { continue; }

            processUnknownMessage(message);
        }

        delay(10);
    }

#if URD_LED_ENABLE
    ledOn();
#endif
}

#if LORA_MANAGER

void UrdGroundStation::initLoRaManager(LoRaManager& manager)
{
    loraManager = &manager;
}

bool UrdGroundStation::processLoraChangeFrequency(const String& message)
{
    if (loraManager == nullptr)
    {
        debugInfo(F("LoRaManager is null."));
        return false;
    }

    // =======================================================
    // APP -> GS:
    // MUDAR_FREQUENCIA (NORMAL CHANGE)
    // =======================================================
    if (message == UrdProtocol::LORA_CHANGE_REQUEST_APP)
    {
        loraForcedRequested = false;
        loraChangeRequested = true;

        loraWaitingVals = true;
        loraWaitingFcConfirmation = false;
        loraWaitingFcFinalConfirmation = false;

        loraChangeLastTime = millis();

        loraChangeCommand = "";
        loraChangeChanText = "";
        loraChangeAddressHex = "";

        loraChangeChan = 0;
        loraChangeAddh = 0;
        loraChangeAddl = 0;

        printInfo(F("LoRa config change requested by app."));
        debugInfo(F("Waiting for VALS:CHANXX_A1B2."));

        return true;
    }

    // =======================================================
    // APP -> GS:
    // MUDAR_AGORA (FORCED CHANGE)
    // =======================================================
    if (message == UrdProtocol::LORA_FORCED_CHANGE_REQUEST_APP)
    {
        loraForcedRequested = true;
        loraChangeRequested = true;

        loraWaitingVals = true;
        loraWaitingFcConfirmation = false;
        loraWaitingFcFinalConfirmation = false;

        loraChangeLastTime = millis();

        loraChangeCommand = "";
        loraChangeChanText = "";
        loraChangeAddressHex = "";

        loraChangeChan = 0;
        loraChangeAddh = 0;
        loraChangeAddl = 0;

        printInfo(F("LoRa FORCED config change requested by app."));
        debugInfo(F("Waiting for VALS:CHANXX_A1B2."));

        return true;
    }

    // =======================================================
    // APP -> GS:
    // VALS:CHANC3_A1B2
    //
    // In forced mode: aplly config immediately and respond to app.
    // In normal mode: forward to FC and wait for confirmation.
    // 
    // GS -> FC:
    // MUD4R_FR3Q_PFV\tCHANC3_A1B2
    // =======================================================
    if (message.startsWith(UrdProtocol::LORA_VALUE_PREFIX))
    {
        if (!loraChangeRequested)
        {
            debugInfo(F("Received VALS without pending LoRa request."));
            Serial.println(UrdProtocol::LORA_CHANGE_ERROR_NOT_REQUESTED_APP);
            return true;
        }

        if (!loraWaitingVals)
        {
            debugInfo(F("Received VALS but GS was not waiting for VALS."));
            Serial.println(UrdProtocol::LORA_CHANGE_ERROR_UNEXPECTED_VALS_APP);
            return true;
        }

        String payload = message.substring(strlen(UrdProtocol::LORA_VALUE_PREFIX));
        payload.trim();
        payload.toUpperCase();

        String chanText;
        String addressText;

        if (!splitLoraChangeCommand(payload, chanText, addressText))
        {
            debugInfo(F("Invalid LoRa config format. Expected CHANXX_A1B2."));
            Serial.println(UrdProtocol::LORA_CHANGE_ERROR_INVALID_FORMAT_APP);

            resetLoraChangeState();
            return true;
        }

        if (!isValidLoraChanText(chanText))
        {
            debugInfo(F("Invalid CHAN value."));
            Serial.println(UrdProtocol::LORA_CHANGE_ERROR_INVALID_CHAN_APP);

            resetLoraChangeState();
            return true;
        }

        if (!isValidLoraAddressText(addressText))
        {
            debugInfo(F("Invalid ADDH/ADDL address value."));
            Serial.println(UrdProtocol::LORA_CHANGE_ERROR_INVALID_ADDR_APP);

            resetLoraChangeState();
            return true;
        }

        if (!decodeLoraChangeCommand(payload))
        {
            debugInfo(F("Failed to decode LoRa config command."));
            Serial.println(UrdProtocol::LORA_CHANGE_ERROR_INVALID_FORMAT_APP);

            resetLoraChangeState();
            return true;
        }

        debugInfo(String("LoRa command decoded: ") + loraChangeCommand);
        debugInfo(String("CHAN DEC: ") + loraChangeChanText);
        debugInfo(String("ADDR HEX: ") + loraChangeAddressHex);

        loraChangeLastTime = millis();

        // ===================================================
        // FORCED CHANGE:
        // ===================================================
        if (loraForcedRequested)
        {
            if (applyDecodedLoraConfig())
            {
                Serial.println(UrdProtocol::LORA_FORCED_CHANGE_REQUEST_CONFIRMATION_APP);
            }
            else
            {
                Serial.println(UrdProtocol::LORA_CHANGE_ERROR_FORCE_FAILED_APP);
            }

            resetLoraChangeState();
            return true;
        }

        // ===================================================
        // NORMAL CHANGE:
        // ===================================================
        Serial.println(UrdProtocol::LORA_CHANGE_REQUEST_CONFIRMATION_APP);

        String flightRequest = String(UrdProtocol::LORA_CHANGE_REQUEST_FLIGHT) +
                               loraChangeCommand + "#";

        loraManager->sendLine(flightRequest);

        debugInfo(String("Sent to FC: ") + flightRequest);

        loraWaitingVals = false;
        loraWaitingFcConfirmation = true;
        loraWaitingFcFinalConfirmation = false;

        loraChangeLastTime = millis();

        return true;
    }

    // =======================================================
    // FC -> GS:
    // CTZ_FR3Q\tCH4NC3_A1B2
    //
    // Both boards change frequency
    //
    // GS -> FC: 1SSO_MSM
    // GS troca configuração
    // GS -> FC: MUD0U_MSM
    // =======================================================
    if (message.startsWith(UrdProtocol::LORA_CHANGE_REQUEST_CONFIRMATION_FLIGHT))
    {
        if (!loraChangeRequested || !loraWaitingFcConfirmation)
        {
            debugInfo(F("Received FC confirmation out of expected state."));
            Serial.println(UrdProtocol::LORA_CHANGE_ERROR_FC_CONFIRM_UNEXPECTED_APP);

            resetLoraChangeState();
            return true;
        }

        String fcPayload = message.substring(strlen(UrdProtocol::LORA_CHANGE_REQUEST_CONFIRMATION_FLIGHT));
        fcPayload.trim();
        fcPayload.toUpperCase();

        String expectedPayload = buildFcConfirmationPayload();

        if (fcPayload != expectedPayload)
        {
            debugInfo(String("FC confirmation mismatch. Received: ") + fcPayload);
            debugInfo(String("Expected: ") + expectedPayload);

            Serial.println(UrdProtocol::LORA_CHANGE_ERROR_FC_CONFIRM_MISMATCH_APP);

            resetLoraChangeState();
            return true;
        }

        debugInfo(F("FC confirmed LoRa config request."));

        loraManager->sendLine(UrdProtocol::LORA_CHANGE_REQUEST_FINAL_CONFIRMATION_FLIGHT);

        debugInfo(F("Sent 1SSO_MSM to FC."));

        delay(100);

        if (!applyDecodedLoraConfig())
        {
            debugInfo(F("Ground Station failed to apply decoded LoRa config."));

            Serial.println(UrdProtocol::LORA_CHANGE_DONE_ERROR_ON_GS_APP);

            resetLoraChangeState();
            return true;
        }

        debugInfo(F("Ground Station changed LoRa config."));

        delay(100);

        loraManager->sendLine(UrdProtocol::LORA_CHANGE_FINAL_DONE_CONFIRMATION_GS_FLIGHT);

        debugInfo(F("Sent MUD0U_MSM to FC on new config."));

        loraWaitingVals = false;
        loraWaitingFcConfirmation = false;
        loraWaitingFcFinalConfirmation = true;

        loraChangeLastTime = millis();

        return true;
    }

    // =======================================================
    // FC -> GS:
    // JUR0_JUR4D1NH0
    //
    // GS -> FC:
    // B04
    //
    // GS -> APP:
    // MUDAR_CERTO
    // =======================================================
    if (message == UrdProtocol::LORA_CHANGE_FINAL_DONE_CONFIRMATION_FC_FLIGHT)
    {
        if (!loraChangeRequested || !loraWaitingFcFinalConfirmation)
        {
            debugInfo(F("Received JUR0_JUR4D1NH0 out of expected state."));
            Serial.println(UrdProtocol::LORA_CHANGE_ERROR_FC_CONFIRM_UNEXPECTED_APP);

            resetLoraChangeState();
            return true;
        }

        debugInfo(F("FC confirmed final LoRa communication."));

        loraManager->sendLine(UrdProtocol::LORA_CHANGE_FINAL_DONE_COMMUNICATION_FC_FLIGHT);

        debugInfo(F("Sent B04 to FC."));

        Serial.println(UrdProtocol::LORA_CHANGE_DONE_APP);

        resetLoraChangeState();
        return true;
    }

    // =======================================================
    // FC -> GS:
    // MUD4R_ERRO
    // =======================================================
    if (message == UrdProtocol::LORA_CHANGE_DONE_ERROR_FLIGHT)
    {
        debugInfo(F("Flight computer reported LoRa change error."));

        Serial.println(UrdProtocol::LORA_CHANGE_ERROR_FC_REPORTED_APP);

        resetLoraChangeState();
        return true;
    }

    return false;
}

bool UrdGroundStation::isHexaText(const String& text)
{
    if (text.length() == 0)
    {
        return false;
    }

    for (unsigned int i = 0; i < text.length(); i++)
    {
        char c = text[i];

        if (!((c >= '0' && c <= '9') ||
              (c >= 'A' && c <= 'F') ||
              (c >= 'a' && c <= 'f')))
        {
            return false;
        }
    }

    return true;
}

bool UrdGroundStation::splitLoraChangeCommand(
    const String& command,
    String& chanText,
    String& addressText
)
{
    String msg = command;
    msg.trim();
    msg.toUpperCase();

    // Formato esperado:
    // CHAN41_A1B2

    if (!msg.startsWith("CHAN"))
    {
        return false;
    }

    int separatorIndex = msg.indexOf('_');

    if (separatorIndex < 0)
    {
        return false;
    }

    chanText = msg.substring(4, separatorIndex);
    addressText = msg.substring(separatorIndex + 1);

    chanText.trim();
    addressText.trim();

    if (chanText.length() < 1 || chanText.length() > 3)
    {
        return false;
    }

    if (addressText.length() != 4)
    {
        return false;
    }

    return true;
}

bool UrdGroundStation::isValidLoraChanText(const String& chanText)
{
    String text = chanText;
    text.trim();

    if (!isDecimalText(text))
    {
        return false;
    }

    int value = text.toInt();

    if (value < 0 || value > 69)
    {
        return false;
    }

    return true;
}

bool UrdGroundStation::isValidLoraAddressText(const String& addressText)
{
    String text = addressText;
    text.trim();
    text.toUpperCase();

    if (text.startsWith("0X"))
    {
        text = text.substring(2);
    }

    if (text.length() != 4)
    {
        return false;
    }

    if (!isHexaText(text))
    {
        return false;
    }

    unsigned long value = strtoul(text.c_str(), nullptr, 16);

    if (value > 0xFFFFUL)
    {
        return false;
    }

    return true;
}

bool UrdGroundStation::decodeLoraChangeCommand(const String& command)
{
    String chanText;
    String addressText;

    if (!splitLoraChangeCommand(command, chanText, addressText))
    {
        return false;
    }

    if (!isValidLoraChanText(chanText))
    {
        return false;
    }

    if (!isValidLoraAddressText(addressText))
    {
        return false;
    }

    addressText.toUpperCase();

    int chanValue = chanText.toInt();
    unsigned long addressValue = strtoul(addressText.c_str(), nullptr, 16);

    loraChangeChan = static_cast<uint8_t>(chanValue);
    loraChangeAddh = static_cast<uint8_t>((addressValue >> 8) & 0xFF);
    loraChangeAddl = static_cast<uint8_t>(addressValue & 0xFF);

    loraChangeChanText = chanText;
    loraChangeAddressHex = addressText;

    loraChangeCommand = String("CH4N") + loraChangeChanText + "_" + loraChangeAddressHex;

    return true;
}

bool UrdGroundStation::isDecimalText(const String& text) {
    if (text.length() == 0) return false;

    for (size_t i = 0; i < text.length(); i++) {
        if (!isDigit(text[i])) {
            return false;
        }
    }

    return true;
}

String UrdGroundStation::buildFcConfirmationPayload() const
{
    return String("CH4N") + loraChangeChanText + "_" + loraChangeAddressHex + "#";
}

bool UrdGroundStation::applyDecodedLoraConfig()
{
    if (loraManager == nullptr)
    {
        return false;
    }

    previousChan = currentChan;
    previousAddh = currentAddh;
    previousAddl = currentAddl;

    if (debugEnabled) {
        Serial.println(String("[LORA DEBUG] Applying LoRa config: CHAN=") + loraChangeChan +
                       String(", ADDH DEC=") + loraChangeAddh +
                       String(", ADDL DEC=") + loraChangeAddl);
    }

    bool success = loraManager->changeFrequency(
        loraChangeChan,
        loraChangeAddh,
        loraChangeAddl
    );

    if (success)
    {
        currentChan = loraChangeChan;
        currentAddh = loraChangeAddh;
        currentAddl = loraChangeAddl;
    }

    return success;
}

void UrdGroundStation::checkLoraChangeTimeout()
{
    if (!loraChangeRequested)
    {
        return;
    }

    if ((millis() - loraChangeLastTime) < UrdProtocol::LORA_CHANGE_TIMEOUT_MS)
    {
        return;
    }

    if (loraWaitingVals)
    {
        Serial.println(UrdProtocol::LORA_CHANGE_TIMEOUT_VALS_APP);
    }
    else if (loraWaitingFcConfirmation)
    {
        Serial.println(UrdProtocol::LORA_CHANGE_TIMEOUT_FC_CONFIRM_APP);
    }
    else if (loraWaitingFcFinalConfirmation)
    {
        Serial.println(UrdProtocol::LORA_CHANGE_TIMEOUT_FINAL_APP);

        debugInfo(F("Reverting Ground Station to previous frequency because FC handshake timed out."));
        loraManager->changeFrequency(previousChan, previousAddh, previousAddl);
        currentChan = previousChan;
        currentAddh = previousAddh;
        currentAddl = previousAddl;
    }
    else
    {
        Serial.println(UrdProtocol::LORA_CHANGE_TIMEOUT_GENERAL_APP);
    }

    debugInfo(F("LoRa change timeout. Resetting LoRa change state."));

    resetLoraChangeState();
}

void UrdGroundStation::resetLoraChangeState()
{
    loraForcedRequested = false;
    loraChangeRequested = false;

    loraWaitingVals = false;
    loraWaitingFcConfirmation = false;
    loraWaitingFcFinalConfirmation = false;

    loraChangeLastTime = 0;

    loraChangeCommand = "";

    loraChangeChanText = "";
    loraChangeAddressHex = "";

    loraChangeChan = 0;
    loraChangeAddh = 0;
    loraChangeAddl = 0;
}

#endif

#if SD_CARD

void UrdGroundStation::initSDManager(SDManager& manager)
{
    sdManager = &manager;
}

#endif

#if URD_GPS_ENABLE

void UrdGroundStation::initGps(
    Stream& gpsStream,
    TinyGPSPlus& gps,
    unsigned long fixTimeoutMs,
    unsigned long noDataTimeoutMs
)
{
    gpsSerial = &gpsStream;
    gpsManager = &gps;

    gpsFixTimeoutMs = fixTimeoutMs;
    gpsNoDataTimeoutMs = noDataTimeoutMs;

    clearGpsCoordinates();

    gpsReceiving = false;
    gpsEverReceivedData = false;
    lastGpsCharTime = 0;
    lastGpsFixTime = 0;
}

void UrdGroundStation::setGpsCoordinates(const String& latitude, const String& longitude)
{
    gpsLatitude = latitude;
    gpsLongitude = longitude;
    gpsAvailable = true;
    lastGpsFixTime = millis();
}

void UrdGroundStation::clearGpsCoordinates()
{
    gpsLatitude = "~";
    gpsLongitude = "~";
    gpsAvailable = false;
}

bool UrdGroundStation::updateGps()
{
    if (gpsSerial == nullptr || gpsManager == nullptr)
    {
        gpsReceiving = false;
        gpsAvailable = false;
        return false;
    }

    bool receivedCharNow = false;

    while (gpsSerial->available() > 0)
    {
        char c = static_cast<char>(gpsSerial->read());

        gpsManager->encode(c);

        receivedCharNow = true;
        gpsEverReceivedData = true;
        lastGpsCharTime = millis();
    }

    if (receivedCharNow)
    {
        gpsReceiving = true;
    }
    else if (gpsNoDataTimeoutMs > 0 && lastGpsCharTime > 0)
    {
        gpsReceiving = ((millis() - lastGpsCharTime) <= gpsNoDataTimeoutMs);
    }
    else
    {
        gpsReceiving = false;
    }

    bool locationValid = gpsManager->location.isValid();
    bool locationFresh = false;

    if (locationValid)
    {
        unsigned long age = gpsManager->location.age();
        locationFresh = (gpsFixTimeoutMs == 0UL || age <= gpsFixTimeoutMs);
    }

    if (gpsManager->location.isUpdated() && locationValid)
    {
        gpsLatitude = String(gpsManager->location.lat(), 6);
        gpsLongitude = String(gpsManager->location.lng(), 6);
        lastGpsFixTime = millis();
    }

    gpsAvailable = (gpsReceiving && locationValid && locationFresh);

    if (!gpsAvailable)
    {
        gpsLatitude = "~";
        gpsLongitude = "~";
    }

    return gpsAvailable;
}

bool UrdGroundStation::hasGpsFix() const
{
    return gpsAvailable;
}

bool UrdGroundStation::isGpsReceiving() const
{
    return gpsReceiving;
}

bool UrdGroundStation::hasGpsEverReceivedData() const
{
    return gpsEverReceivedData;
}

String UrdGroundStation::getGpsLatitude() const
{
    return gpsLatitude;
}

String UrdGroundStation::getGpsLongitude() const
{
    return gpsLongitude;
}

unsigned long UrdGroundStation::getLastGpsFixTime() const
{
    return lastGpsFixTime;
}

unsigned long UrdGroundStation::getLastGpsCharTime() const
{
    return lastGpsCharTime;
}

unsigned long UrdGroundStation::getGpsCharsProcessed() const
{
    if (gpsManager == nullptr)
    {
        return 0;
    }

    return gpsManager->charsProcessed();
}

unsigned long UrdGroundStation::getGpsSentencesWithFix() const
{
    if (gpsManager == nullptr)
    {
        return 0;
    }

    return gpsManager->sentencesWithFix();
}

unsigned long UrdGroundStation::getGpsFailedChecksum() const
{
    if (gpsManager == nullptr)
    {
        return 0;
    }

    return gpsManager->failedChecksum();
}

unsigned long UrdGroundStation::getGpsLocationAge() const
{
    if (gpsManager == nullptr || !gpsManager->location.isValid())
    {
        return ULONG_MAX;
    }

    return gpsManager->location.age();
}

uint32_t UrdGroundStation::getGpsSatellites() const
{
    if (gpsManager == nullptr || !gpsManager->satellites.isValid())
    {
        return 0;
    }

    return gpsManager->satellites.value();
}

float UrdGroundStation::getGpsHdop() const
{
    if (gpsManager == nullptr || !gpsManager->hdop.isValid())
    {
        return -1.0f;
    }

    return gpsManager->hdop.hdop();
}

void UrdGroundStation::printGpsStatus(Stream& out)
{
    out.print(F("GPS receiving: "));
    out.print(gpsReceiving ? F("YES") : F("NO"));

    out.print(F(" | fix: "));
    out.print(gpsAvailable ? F("YES") : F("NO"));

    out.print(F(" | lat: "));
    out.print(gpsLatitude);

    out.print(F(" | lon: "));
    out.print(gpsLongitude);

    out.print(F(" | sats: "));
    out.print(getGpsSatellites());

    out.print(F(" | hdop: "));
    out.print(getGpsHdop(), 1);

    out.print(F(" | chars: "));
    out.print(getGpsCharsProcessed());

    out.print(F(" | fix sentences: "));
    out.print(getGpsSentencesWithFix());

    out.print(F(" | checksum fail: "));
    out.println(getGpsFailedChecksum());
}

#endif

bool UrdGroundStation::processGpsCoords(const String& message)
{
    if (message != UrdProtocol::GPS_COORDS)
    {
        return false;
    }

#if URD_GPS_ENABLE
    updateGps();

    if (hasGpsFix())
    {
        Serial.print(gpsLatitude);
        Serial.print('\t');
        Serial.println(gpsLongitude);
    }
    else
#endif
    {
        Serial.println(UrdProtocol::NO_GPS_COORDS);
    }

    if (!gsStarted)
    {
        gsStarted = true;
        markAppStarted();
        Serial.println(F("---Ground Station UFABC Rocket Design---"));
    }

#if URD_BUZZER_ENABLE
    if (buzzerEnabled)
    {
        playDash();
    }
#endif

    return true;
}

void UrdGroundStation::processAppSerialMessages()
{
    String message;

    while (readAppMessage(message))
    {
        message.trim();

        if (message.length() == 0)
        {
            continue;
        }

        if (processReady(message))
        {
            continue;
        }

        if (processReset(message))
        {
            continue;
        }

        if (processGpsCoords(message))
        {
            continue;
        }

#if LORA_MANAGER
        if (processLoraChangeFrequency(message))
        {
            continue;
        }
#endif

        if (debugEnabled)
        {
            Serial.print(F("[DEBUG][GS] Unknown app message: "));
            Serial.println(message);
        }
    }
}

void UrdGroundStation::processLoraMessages()
{
#if LORA_MANAGER
    if (loraManager == nullptr)
    {
        return;
    }

    String packet;

    while (loraManager->readLine(packet))
    {
        packet.trim();

        if (packet.length() == 0)
        {
            continue;
        }

        if (processLoraChangeFrequency(packet))
        {
            continue;
        }

        processLoraPacket(packet);
    }
#endif
}

void UrdGroundStation::update()
{
#if URD_GPS_ENABLE
    updateGps();
#endif

    if (!appStarted)
    {
        return;
    }

#if LORA_MANAGER
    checkLoraChangeTimeout();
#endif

    processAppSerialMessages();

    processLoraMessages();
}


void UrdGroundStation::processLoraPacket(const String& packet)
{
    if (packet.length() == 0)
    {
        return;
    }

#if URD_BUZZER_ENABLE
    if (buzzerEnabled)
    {
        playDot();
    }
#endif

    Serial.println(packet);

#if SD_CARD
    if (sdManager != nullptr && sdManager->isReady())
    {
        if (sdManager->writeLine(packet))
        {
#if URD_BUZZER_ENABLE
            if (buzzerEnabled)
            {
                playDot();
            }
#endif
        }
        else
        {
            debugInfo(F("Failed to save LoRa packet to SD."));
        }
    }
#endif
}

bool UrdGroundStation::isGsStarted() const
{
    return gsStarted;
}

#endif
