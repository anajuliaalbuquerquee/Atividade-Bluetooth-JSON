/*
 * Ex 2 - ESP 2 (ESCRAVO): pede NOME, IDADE e HABILITADO no Monitor Serial
 *        e envia como JSON via Bluetooth para o ESP 1
 *
 * Fluxo:
 *   1. Pergunta "Nome:" e espera o usuário digitar + Enter.
 *   2. Pergunta "Idade:".
 *   3. Pergunta "Habilitado? (s/n):".
 *   4. Monta {"nome":"Ana","idade":20,"habilitado":true} e envia via BT.
 *   5. Volta ao passo 1.
 *
 * Monitor Serial: 115200, com terminação de linha "Nova linha" (LF).
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>

const char* NOME_ESTE = "ESP2_BT";

BluetoothSerial SerialBT;

// Mostra a pergunta e bloqueia até o usuário digitar uma linha
String perguntar(const char* pergunta) {
  Serial.print(pergunta);
  Serial.print(' ');
  while (!Serial.available()) {
    delay(10);
  }
  String resposta = Serial.readStringUntil('\n');
  resposta.trim();
  Serial.println(resposta);  // eco do que foi digitado
  return resposta;
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  SerialBT.begin(NOME_ESTE);
  Serial.println("ESP 2 pronto. Aguardando conexao do ESP 1...");
}

void loop() {
  if (!SerialBT.connected()) {
    delay(500);
    return;
  }

  Serial.println("\n=== Cadastro de usuario ===");
  String nome = perguntar("Nome:");
  int idade = perguntar("Idade:").toInt();
  String resp = perguntar("Habilitado? (s/n):");
  resp.toLowerCase();
  bool habilitado = (resp == "s" || resp == "sim");

  JsonDocument doc;
  doc["nome"] = nome;
  doc["idade"] = idade;
  doc["habilitado"] = habilitado;

  String json;
  serializeJson(doc, json);
  SerialBT.println(json);

  Serial.print("Enviado via BT: ");
  Serial.println(json);
}
