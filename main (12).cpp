/*
 * Ex 2 - ESP 1 (MESTRE): recebe o JSON do ESP 2 via Bluetooth e mostra na Serial
 *
 * Recebe {"nome":"Ana","idade":20,"habilitado":true} e imprime:
 *   Usuario: Ana
 *   Idade: 20
 *   Habilitado
 *
 * Papel Bluetooth: MESTRE — conecta no "ESP2_BT". (A conexão SPP é
 * bidirecional, então o ESP 2 pode enviar mesmo sendo o escravo.)
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>

const char* NOME_ESTE = "ESP1_BT";
const char* NOME_DESTINO = "ESP2_BT";

BluetoothSerial SerialBT;
String buffer = "";

void conectarBluetooth() {
  Serial.print("Conectando em ");
  Serial.println(NOME_DESTINO);
  while (!SerialBT.connect(NOME_DESTINO)) {
    Serial.println("Falha ao conectar. Tentando novamente...");
    delay(1000);
  }
  Serial.println("Bluetooth conectado! Aguardando dados do ESP 2...");
}

void processarJson(const String& json) {
  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, json);
  if (erro) {
    Serial.print("JSON invalido: ");
    Serial.println(erro.c_str());
    return;
  }

  const char* nome = doc["nome"] | "";
  int idade = doc["idade"] | 0;
  bool habilitado = doc["habilitado"] | false;

  Serial.println("---------------------------");
  Serial.print("Usuario: ");
  Serial.println(nome);
  Serial.print("Idade: ");
  Serial.println(idade);
  Serial.println(habilitado ? "Habilitado" : "Nao Habilitado");
  Serial.println("---------------------------");
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin(NOME_ESTE, true);
  conectarBluetooth();
}

void loop() {
  if (!SerialBT.connected()) {
    conectarBluetooth();
  }

  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') {
      buffer.trim();
      processarJson(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }
}
