/*************************
 * Projeto IOT com ESP32
 * Escola SENAI de Informática
 * Curso de Apredizagem industrial
 * Integrador de soluções em nuvem
 * Data: 01/08/2024
 *
 * Autor: Thiago Augusto de Oliveira
 *
 * Descrição: Projeto de automação utilizando ESP32
 * com conexão WiFi e MQTT.
 *
 * versão: 0.9
 *************************/

#include <Arduino.h>
#include <DHTesp.h>
#include <ArduinoJson.h>
#include "iot.h"
#include "saidas.h"
#include "entradas.h"
#include "tempo.h"
#include "atuadores.h"

// Definição dos tópicos de publicação
#define mqtt_pub_topic2 "projetoProfessor/botao_boot"
#define mqtt_pub_topic3 "projetoSimon/exercicio"

// Variáveis globais
unsigned long tempo_anterior = 0;
const unsigned long intervalo = 5000;

void setup()
{

  Serial.begin(115200);
  setup_wifi();
  setup_time();
  inicializa_entradas();
  inicializa_saidas();
  inicializa_mqtt();
  inicializa_servos();
}

void loop()
{
  atualiza_time();
  atualiza_saidas();
  atualiza_botoes();
  atualiza_mqtt();

  JsonDocument doc;
  String json;
  bool menssagemEmFila = false;

  if (millis() - tempo_anterior >= intervalo)
  {
    tempo_anterior - millis();
    doc["timeStamp"] = timeStamp();

    serializeJson(doc, json);
    publica_mqtt(mqtt_pub_topic3, json);
    menssagemEmFila = true;
  }

  if (botao_boot_pressionado())
  {
    LedBuiltInState =! LedBuiltInState;
    doc["LedState"] = LedBuiltInState;  // Manda a informação quando o botao for apertado no MQTT
    doc["BotaoState"] = true; // Manda a informação quando o botao for apertado no MQTT
    doc["timeStamp"] = timeStamp();  // Manda a informação quando o botao for apertado no MQTT
    serializeJson(doc, json);
    publica_mqtt(mqtt_pub_topic3, json);
    menssagemEmFila = true;
  }
  if (botao_boot_solto())
  {
    doc["LedState"] = LedBuiltInState;  // Manda a informação quando o botao for solto no MQTT
    doc["BotaoState"] = false; // Manda a informação quando o botao for solto no MQTT
    doc["timeStamp"] = timeStamp();  // Manda a informação quando o botao for solto no MQTT
    serializeJson(doc, json);
    publica_mqtt(mqtt_pub_topic3, json);
    menssagemEmFila = true;
  }

  if (menssagemEmFila)
  {
    serializeJson(doc, json);
    publica_mqtt(mqtt_pub_topic2, json);
    menssagemEmFila = false;
  }
}