#include "CERT.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <esp_task_wdt.h>
#define WDT_TIMEOUT 300  //сек

#define ONE_WIRE_BUS 4 // Data wire is plugged into pin 4 on the Arduino
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "dlink";
const char* password = "538stm32";

const char* mqtt_server = "mqtt.cloud.yandex.net";
const int mqtt_port = 8883;

#define PUBLISH_TOPIC_TEMP_OUTDOOR "gb_iot/Challenge/58_PNZ_FSM/Temp(oudoor)"
#define PUBLISH_TOPIC_RSSI_OUT "gb_iot/Challenge/58_PNZ_FSM/WiFi.RSSI_outdoor"

WiFiClientSecure wifi_client_publ;
PubSubClient mqtt_client_publ(wifi_client_publ);

String message = "0";
unsigned long myTimer1; // для millis
int Temp_outdoor = 0;

void wifi_mqtt_init(void)
{
  wifi_client_publ.setCACert(root_ca);
  wifi_client_publ.setCertificate(dev_cert_tx);
  wifi_client_publ.setPrivateKey(key_cert_tx);
  mqtt_client_publ.setClient(wifi_client_publ);
  mqtt_client_publ.setServer(mqtt_server, mqtt_port);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println(WiFi.localIP());

  delay(500);

  while (!mqtt_client_publ.connected())
  {
    Serial.print("MQTT publish connecting ");
    if (mqtt_client_publ.connect(mqtt_server))
    {
      Serial.println("OK");
    }
    else
    {
      Serial.print("failed, status code = ");
      Serial.print(mqtt_client_publ.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

}

void setup()
{
  Serial.begin(115200);
  sensors.begin();
  wifi_mqtt_init();
  esp_task_wdt_init(WDT_TIMEOUT, true);  //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);                //add current thread to WDT watch
}

void loop()
{

  esp_task_wdt_reset();
  
  // Проверка на дисконнект
  while (!mqtt_client_publ.connected())
  {
    if (mqtt_client_publ.connect(mqtt_server)) {
      //Serial.println("OK");
    }
    else
    {
      delay(5000);
      break;
    }
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    wifi_mqtt_init();
  }


  if (millis() - myTimer1 >= 10000)
  {
    myTimer1 = millis();

    sensors.requestTemperatures(); // Send the command to get temperature readings. call sensors.requestTemperatures() to issue a global temperature
    Temp_outdoor = sensors.getTempCByIndex(0);
    //Serial.println(Temp_outdoor);

    char buff_msg[32];
    for (int i = 0; i < 32; i++)
    {
      buff_msg[i] = 0; // Обнуляем массив
    }
    sprintf(buff_msg, "%d", Temp_outdoor);
    mqtt_client_publ.publish(PUBLISH_TOPIC_TEMP_OUTDOOR, buff_msg);

    for (int i = 0; i < 32; i++) {
      buff_msg[i] = 0;  // Обнуляем массив
    }
    sprintf(buff_msg, "%d", WiFi.RSSI());
    mqtt_client_publ.publish(PUBLISH_TOPIC_RSSI_OUT, buff_msg);

  }

}
