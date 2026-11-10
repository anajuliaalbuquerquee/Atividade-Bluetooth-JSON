/*
 * Ex 1 - Etapa 3 - ESP 2 (ESCRAVO): recebe JSON do DHT22 e exibe no LCD
 *
 * Recebe {"sensor":"DHT22","temperatura":25.3,"umidade":60.1} via Bluetooth,
 * interpreta com ArduinoJson e atualiza o LCD somente quando houver alteração.
 *
 * Circuito:
 *   LCD 16x2 I2C (0x27): VCC → 5V (VIN), GND → GND, SDA → GPIO 21, SCL → GPIO 22
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>
#include <LiquidCrystal_I2C.h>

const char* NOME_ESTE = "ESP2_BT";

BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27, 16, 2);

String buffer = "";
float ultimaTemp = NAN;
float ultimaUmid = NAN;

void mostrarNoLcd(float temperatura, float umidade) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatura, 1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Umid: ");
  lcd.print(umidade, 1);
  lcd.print("%");
}

void processarJson(const String& json) {
  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, json);

  if (erro) {
    Serial.print("JSON invalido: ");
    Serial.println(erro.c_str());
    return;
  }

  const char* sensor = doc["sensor"] | "?";
  float temperatura = doc["temperatura"];
  float umidade = doc["umidade"];

  Serial.print("Sensor: ");
  Serial.print(sensor);
  Serial.print(" | Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.print(" C | Umidade: ");
  Serial.print(umidade, 1);
  Serial.println(" %");

  bool mudou = (temperatura != ultimaTemp) || (umidade != ultimaUmid);
  if (mudou) {
    mostrarNoLcd(temperatura, umidade);
    ultimaTemp = temperatura;
    ultimaUmid = umidade;
    Serial.println("  -> LCD atualizado");
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.print("Aguardando BT...");
  SerialBT.begin(NOME_ESTE);
  Serial.println("ESP 2 pronto. Aguardando conexao do ESP 1...");
}

void loop() {
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') {
      buffer.trim();
      Serial.print("Recebido: ");
      Serial.println(buffer);
      processarJson(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }
}
