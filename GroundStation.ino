#include "src/lib/config.h"

unsigned long currentTime = 0;

#if URD_APP
unsigned long startTime = 0;
bool inicio = false;
#endif

#if BUZZER
#include "src/lib/Buzzer/buzzer.h"
BuzzerEvento buzzer(BUZZ_PIN, BUZZ_ON, 75, 50);
#endif
#include "src/lib/Utils/Utils.h"
#if LED_RGB
Utils utils(SD_CS, LED_R, LED_G, LED_B);
#else
Utils utils(SD_CS, LED_R);
#endif

#if LORA
#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial LoRa(1);  //32 tx lora, 33 rx lora
#define M0_LORA_PIN 26
#define M1_LORA_PIN 25
#define AUX_LORA_PIN 35
#else
HardwareSerial &LoRa = Serial2;
#define M0_LORA_PIN 12
#define M1_LORA_PIN 11
#define AUX_LORA_PIN 10
#endif

#else
HardwareSerial &LoRa = Serial2;
#endif

// ========================================================
// Setup
// ========================================================
void setup() {
  Serial.begin(115200);

#if LORA
  LoRa.begin(9600);
  pinMode(M0_LORA_PIN, OUTPUT);
  pinMode(M1_LORA_PIN, OUTPUT);
  pinMode(AUX_LORA_PIN, INPUT);
  digitalWrite(M0_LORA_PIN, LOW);
  digitalWrite(M1_LORA_PIN, LOW);
#endif

#if SD_CARD
  utils.beginSD(SD_NAME);
#endif

#if BUZZER
  utils.apitar(50, 3, BUZZ_PIN, BUZZ_ON);
#endif

  pinMode(LED_R, OUTPUT);
#if LED_RGB
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  utils.rgbStart();
  utils.setColor(black);
#endif


#if URD_APP
  // Handshake inicial com PC
  while (!inicio) {
    if (Serial.available()) {
      String msg = Serial.readStringUntil('\n');
      msg.trim();
      if (msg == "READY") {
#if LED_RGB
        utils.setColor(green);
#endif
        inicio = true;
        Serial.println("OK");
        startTime = millis();
        Serial.println("---Ground Station UFABC Rocket Design---");
#if LED_RGB
        utils.setColor(black);
#endif
      }
      if (msg == "RST") {
#if LED_RGB
        utils.setColor(red);
        delay(50);
        utils.setColor(black);
        delay(50);
        utils.setColor(red);
        delay(50);
        utils.setColor(black);
#if BUZZER
        utils.apitar(50, 3, BUZZ_PIN, BUZZ_ON);
#endif
#else
        utils.apitar(50, 3, -1, -1);
#endif


        utils.reset();
      }
    }
  }
#endif
}

// ========================================================
// Loop
// ========================================================
void loop() {
  currentTime = millis();

#if URD_APP
  // Comando de reset vindo do PC
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    // RST -> reset local
    if (cmd == "RST") {
#if LED_RGB
      utils.setColor(red);
      delay(50);
      utils.setColor(black);
      delay(50);
      utils.setColor(red);
      delay(50);
      utils.setColor(black);
#if BUZZER
      utils.apitar(50, 3, BUZZ_PIN, BUZZ_ON);
#else
      utils.apitar(50, 3, -1, 1);
#endif
#else
      utils.apitar(50, 3, -1, 1);
#endif
      utils.reset();
    }

#if IGN_MODE
    // ---- NOVOS COMANDOS VINDO DO PC ----
    if (cmd == "PING!") {
      // Repassa para o LoRa
      LoRa.println("PING!\n");
      Serial.println("[GS] PING enviado via LoRa");
    } else if (cmd == "ARMED!") {
      LoRa.println("ARMED!\n");
      Serial.println("[GS] ARMED enviado via LoRa");
    } else if (cmd == "DISARMED!") {
      LoRa.println("DISARMED!\n");
      Serial.println("[GS] DISARMED enviado via LoRa");
    } else if (cmd == "IGN!") {
      LoRa.println("IGN!\n");
      Serial.println("[GS] IGN enviado via LoRa");
    }
#endif  // IGN_MODE
  }

  if (!inicio) return;
#endif  // URD_APP

  // ---- RECEBE DO LORA ----
  if (LoRa.available()) {
    String linha = LoRa.readStringUntil('\n');  // lê até quebra de linha

    if (linha.length() > 0) {
#if BUZZER
      buzzer.add();
#endif
#if LED_RGB
      utils.setColor(green);
      delay(50);
      utils.setColor(black);
      delay(50);
      utils.setColor(green);
      delay(50);
      utils.setColor(black);
#if BUZZER
      utils.apitar(50, 3, BUZZ_PIN, BUZZ_ON);
#endif
#else
      utils.apitar(50, 3, -1, 1);
#endif
      Serial.println(linha);  // repassa para o PC
#if IGN_MODE
        // Se for Pong0 ou Pong1 -> repassa para o PC
      if (!linha.startsWith("Pong")) {
#if SD_CARD
      if (utils.logSD(linha)) buzzer.add();  // salva no SD usando Utils
#endif
      }
#else
    if (utils.logSD(linha)) buzzer.add();  // salva no SD usando Utils
#endif
    }
  }

#if BUZZER
  buzzer.loop(currentTime);
#endif
}
