/*
 * Ex 1 - Etapa 2 - ESP 2 (ESCRAVO): recebe e exibe no LCD só quando houver alteração
 *
 * Recebe "T=xx.x;U=yy.y" via Bluetooth. Compara com os últimos valores
 * mostrados; só reescreve o LCD se temperatura ou umidade mudaram.
 * (A Serial mostra tudo o que chega, para depuração.)
 *
 * Circuito:
 *   LCD 16x2 I2C (endereço 0x27): VCC → 5V (VIN), GND → GND,
 *                                 SDA → GPIO 21, SCL → GPIO 22
 */

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <LiquidCrystal_I2C.h>

const char* NOME_ESTE = "ESP2_BT";

BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // se não aparecer nada, tente 0x3F

String buffer = "";
float ultimaTemp = NAN;
float ultimaUmid = NAN;

// Extrai T e U de "T=25.3;U=60.1". Retorna false se o formato for inválido.
bool interpretar(const String& msg, float& temperatura, float& umidade) {
  int posT = msg.indexOf("T=");
  int posPV = msg.indexOf(';');
  int posU = msg.indexOf("U=");
  if (posT < 0 || posPV < 0 || posU < 0) return false;
  temperatura = msg.substring(posT + 2, posPV).toFloat();
  umidade = msg.substring(posU + 2).toFloat();
  return true;
}

void mostrarNoLcd(float temperatura, float umidade) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatura, 1);
  lcd.print((char)223);  // símbolo °
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Umid: ");
  lcd.print(umidade, 1);
  lcd.print("%");
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
    if (c != '\n') {
      buffer += c;
      continue;
    }

    buffer.trim();
    Serial.print("Recebido: ");
    Serial.println(buffer);

    float temperatura, umidade;
    if (interpretar(buffer, temperatura, umidade)) {
      bool mudou = (temperatura != ultimaTemp) || (umidade != ultimaUmid);
      if (mudou) {
        mostrarNoLcd(temperatura, umidade);
        ultimaTemp = temperatura;
        ultimaUmid = umidade;
        Serial.println("  -> LCD atualizado (houve alteracao)");
      } else {
        Serial.println("  -> sem alteracao, LCD mantido");
      }
    }
    buffer = "";
  }
}
