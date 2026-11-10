/*
 * Ex 1 - Etapa 2 - ESP 1 (MESTRE): envia a cada 2 s OU quando houver alteração
 *
 * Lê o DHT22 a cada 1 s. Envia "T=xx.x;U=yy.y" para o ESP 2:
 *   - imediatamente, se temperatura ou umidade mudaram desde o último envio; ou
 *   - a cada 2 s, mesmo sem mudança (envio periódico).
 *
 * Circuito:
 *   DHT22: VCC → 3V3, DATA → GPIO 4, GND → GND
 */

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <DHT.h>

const int PINO_DHT = 4;
#define TIPO_DHT DHT22

const char* NOME_ESTE = "ESP1_BT";
const char* NOME_DESTINO = "ESP2_BT";

const unsigned long INTERVALO_LEITURA = 1000;  // ms
const unsigned long INTERVALO_ENVIO = 2000;    // ms (envio obrigatório)

BluetoothSerial SerialBT;
DHT dht(PINO_DHT, TIPO_DHT);

unsigned long ultimaLeitura = 0;
unsigned long ultimoEnvio = 0;
float ultimaTemp = NAN;
float ultimaUmid = NAN;

void conectarBluetooth() {
  Serial.print("Conectando em ");
  Serial.println(NOME_DESTINO);
  while (!SerialBT.connect(NOME_DESTINO)) {
    Serial.println("Falha ao conectar. Tentando novamente...");
    delay(1000);
  }
  Serial.println("Bluetooth conectado!");
}

void enviar(float temperatura, float umidade, const char* motivo) {
  String mensagem = "T=" + String(temperatura, 1) + ";U=" + String(umidade, 1);
  SerialBT.println(mensagem);
  ultimoEnvio = millis();
  ultimaTemp = temperatura;
  ultimaUmid = umidade;

  Serial.print("Enviado (");
  Serial.print(motivo);
  Serial.print("): ");
  Serial.println(mensagem);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  SerialBT.begin(NOME_ESTE, true);
  conectarBluetooth();
}

void loop() {
  if (!SerialBT.connected()) {
    conectarBluetooth();
  }

  unsigned long agora = millis();

  if (agora - ultimaLeitura >= INTERVALO_LEITURA) {
    ultimaLeitura = agora;

    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();
    if (isnan(temperatura) || isnan(umidade)) {
      Serial.println("Falha ao ler o DHT22!");
      return;
    }

    bool mudou = (temperatura != ultimaTemp) || (umidade != ultimaUmid);
    bool passouTempo = (agora - ultimoEnvio >= INTERVALO_ENVIO);

    if (mudou) {
      enviar(temperatura, umidade, "alteracao");
    } else if (passouTempo) {
      enviar(temperatura, umidade, "periodico 2s");
    }
  }
}
