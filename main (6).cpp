/*
 * Ex 1 - Etapa 1 - ESP 1 (MESTRE): lê o DHT22 e envia via Bluetooth
 *
 * Lê temperatura e umidade do DHT22 e envia a linha
 *   "T=25.3;U=60.1"
 * para o ESP 2 pela conexão Bluetooth Classic (SPP).
 *
 * Papel Bluetooth: MESTRE — procura e conecta no dispositivo "ESP2_BT".
 *
 * Circuito:
 *   DHT22: VCC → 3V3, DATA → GPIO 4, GND → GND (resistor 10 kΩ entre DATA e 3V3)
 */

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <DHT.h>

const int PINO_DHT = 4;
#define TIPO_DHT DHT22

const char* NOME_ESTE = "ESP1_BT";   // nome deste ESP
const char* NOME_DESTINO = "ESP2_BT"; // nome do ESP 2 (escravo)

const unsigned long INTERVALO_ENVIO = 2000;  // ms

BluetoothSerial SerialBT;
DHT dht(PINO_DHT, TIPO_DHT);
unsigned long ultimoEnvio = 0;

void conectarBluetooth() {
  Serial.print("Conectando em ");
  Serial.print(NOME_DESTINO);
  Serial.println("...");
  while (!SerialBT.connect(NOME_DESTINO)) {
    Serial.println("Falha ao conectar. Tentando novamente...");
    delay(1000);
  }
  Serial.println("Bluetooth conectado!");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  SerialBT.begin(NOME_ESTE, true);  // true = modo mestre
  conectarBluetooth();
}

void loop() {
  if (!SerialBT.connected()) {
    conectarBluetooth();
  }

  if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {
    ultimoEnvio = millis();

    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();

    if (isnan(temperatura) || isnan(umidade)) {
      Serial.println("Falha ao ler o DHT22!");
      return;
    }

    String mensagem = "T=" + String(temperatura, 1) + ";U=" + String(umidade, 1);
    SerialBT.println(mensagem);  // '\n' marca o fim da mensagem

    Serial.print("Enviado: ");
    Serial.println(mensagem);
  }
}
