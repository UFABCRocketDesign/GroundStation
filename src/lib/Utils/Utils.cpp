#include "Utils.h"

Utils::Utils(int sdPin, int rPin, int gPin, int bPin) {
  sdCsPin = sdPin;
  ledR = rPin;
  ledG = gPin;
  ledB = bPin;
  lastMillis = 0;
  maxSaved = -INFINITY;

#if (LED_RGB)
  if (ledR != -1) {
    pinMode(ledR, OUTPUT);
  }
if (ledG != -1) {
    pinMode(ledG, OUTPUT);
  }
if (ledB != -1) {
    pinMode(ledB, OUTPUT);
  }
#endif
}

#if (LORA)
void Utils::beginLoRa(unsigned long baud) {
  Serial2.begin(baud);
}

void Utils::sendLoRa(const String& msg) {
  Serial2.println(msg);
}

bool Utils::availableLoRa() {
  return Serial2.available();
}

String Utils::readLoRaLine() {
  if (Serial2.available()) return Serial2.readStringUntil('\n');
  return "";
}

bool Utils::interval(unsigned long ms) {
  unsigned long now = millis();
  if (now - lastMillis >= ms) {
    lastMillis = now;
    return true;
  }
  return false;
}
#endif

#if (SD_CARD)
bool Utils::beginSD(const String& baseName) {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, sdCsPin);
  if (!SD.begin(sdCsPin, SPI)) {
#else
  if (!SD.begin(sdCsPin)) {
#endif
    Serial.println(F("SD err"));
    return false;
  } else {
    Serial.println(F("SD ok"));
  }

  int iSD = 0;
  unsigned long t_atual = millis();
  while (true) {
    if (millis() - t_atual > 10000) {
      strcpy(nomeSD, "err");
      break;
    }

    // calcula quantos zeros precisa
    int numZeros = 8 - baseName.length() - String(iSD).length();
    String zeros = "";
    for (int i = 0; i < numZeros; i++) zeros += "0";

    // monta o nome do arquivo
    String nome = baseName + zeros + String(iSD) + ".txt";
    nome.toCharArray(nomeSD, sizeof(nomeSD));

    if (SD.exists(nomeSD)) {
      // já existe → tenta próximo
    } else {
      File dataFile = SD.open(nomeSD, FILE_WRITE);
      if (dataFile) {
        Serial.println(String(nomeSD) + " criado.");
      } else {
        Serial.println(String(nomeSD) + "err");
        return false;
      }
      break;
    }
    iSD++;
  }
  return true;
}


bool Utils::logSD(const String& data) {
  dataFile = SD.open(nomeSD, FILE_WRITE);
  if (dataFile) {
    dataFile.println(data);
    dataFile.flush();
    dataFile.close();
    return true;
  }
  return false;
}
#endif

#if (LED_RGB)
void Utils::rgb(bool r, bool g, bool b) {
  digitalWrite(ledR, r);
  digitalWrite(ledG, g);
  digitalWrite(ledB, b);
}

void Utils::setColor(Colors color) {
  switch (color) {
    case white:   rgb(L_ON,  L_ON,  L_ON);  break;
    case magenta: rgb(L_ON, !L_ON,  L_ON);  break;
    case blue:    rgb(!L_ON, !L_ON,  L_ON); break;
    case cyan:    rgb(!L_ON,  L_ON,  L_ON); break;
    case green:   rgb(!L_ON,  L_ON, !L_ON); break;
    case yellow:  rgb(L_ON,  L_ON, !L_ON);  break;
    case red:     rgb(L_ON, !L_ON, !L_ON);  break;
    case black:   rgb(!L_ON, !L_ON, !L_ON); break;
  }
}

void Utils::rgbStart() {
  setColor(white);
  delay(100);
  setColor(magenta);
  delay(100);
  setColor(blue);
  delay(100);
  setColor(cyan);
  delay(100);
  setColor(green);
  delay(100);
  setColor(yellow);
  delay(100);
  setColor(red);
  delay(100);
  setColor(black);
  delay(100);
}
#endif

#if (GPS)
void Utils::beginGPS(unsigned long baud) {
  Serial3.begin(baud);
}

String Utils::readGPSLine() {
  if (Serial3.available()) return Serial3.readStringUntil('\n');
  return "";
}
#endif

void Utils::reset() {
#if (LED_RGB)
    rgbStart();
#else
if (ledR != -1) {
for (int i = 0; i < 2; i++) {
    digitalWrite(ledR, HIGH);
    delay(100);
    digitalWrite(ledR, LOW);
    delay(100);
  }
}
#endif

#if defined(ARDUINO_ARCH_ESP32)
  ESP.restart();
#else 
  asm volatile ("jmp 0");
#endif
}

void Utils::updateMax(float val) {
  if (val > maxSaved) maxSaved = val;
}

float Utils::getMaxValue() {
  return maxSaved;
}

void Utils::resetMaxValue() {
  maxSaved = -INFINITY;
}

void Utils::apitar(int tempo, int apitos, int buzzPin, bool buzzOn) {
    for (int i = 0; i <= apitos; i++) {
    if (buzzPin != -1) digitalWrite(buzzPin, !buzzOn);
    digitalWrite(ledR, buzzOn);
    delay(tempo);
    if (buzzPin != -1) digitalWrite(buzzPin, buzzOn);
    digitalWrite(ledR, !buzzOn);
    delay(tempo);
  }
}
