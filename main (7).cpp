/*
 * Ex 1 - Etapa 1 - ESP 2 (ESCRAVO): recebe os dados do ESP 1 e mostra na Serial
 *
 * Papel Bluetooth: ESCRAVO — fica visível com o nome "ESP2_BT" e aguarda
 * a conexão do ESP 1.
 *
 * Circuito: só o ESP32 (Monitor Serial a 115200).
 */

#include <Arduino.h>
#include <BluetoothSerial.h>

const char* NOME_ESTE = "ESP2_BT";

BluetoothSerial SerialBT;
String buffer = "";

void setup() {
  Serial.begin(115200);
  SerialBT.begin(NOME_ESTE);  // modo escravo (padrão)
  Serial.println("ESP 2 pronto. Aguardando conexao do ESP 1...");
}

void loop() {
  // Lê caractere por caractere até encontrar '\n' (fim da mensagem)
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') {
      buffer.trim();
      Serial.print("Recebido: ");
      Serial.println(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }
}
