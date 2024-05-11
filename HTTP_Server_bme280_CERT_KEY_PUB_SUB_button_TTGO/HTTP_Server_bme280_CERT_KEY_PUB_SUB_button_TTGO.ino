#include "CERT.h"
#include <Arduino.h>
#include <FastBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <string.h>

#define BOT_TOKEN "6529963600:AAGqhhalfC66JXRlbIHYvebBx-0rPZT9Dus"
#define CHAT_ID "836456379"
FastBot bot(BOT_TOKEN);

#include <esp_task_wdt.h>
#define WDT_TIMEOUT 120  //сек

const char* ssid = "dlink";
const char* password = "538stm32";
const char* mqtt_server = "mqtt.cloud.yandex.net";
const int mqtt_port = 8883;
uint8_t LED = 15;    // Номер вывода МК для подключения LED/реле включения увлажнителя
uint8_t BUTTON = 2;  // Номер вывода МК для подключения кнопки включения увлажнителя

// fsm8.ru:8880/fsm8

#define PUBLISH_TOPIC_PRES "gb_iot/Challenge/58_PNZ_FSM/Pres"
#define PUBLISH_TOPIC_TEMP "gb_iot/Challenge/58_PNZ_FSM/Temp"
#define PUBLISH_TOPIC_HUMI "gb_iot/Challenge/58_PNZ_FSM/Humi"
#define PUBLISH_TOPIC_RSSI_IN "gb_iot/Challenge/58_PNZ_FSM/WiFi.RSSI_indoor"
#define PUBLISH_TOPIC_BUTTON_STATUS "gb_iot/Challenge/58_PNZ_FSM/Button_status"
#define SUBSCRIBE_TOPIC_OUTDOOR_TEMP "gb_iot/Challenge/58_PNZ_FSM/Temp(oudoor)"  // Считываем уличную температуру
#define SUBSCRIBE_TOPIC_BUTTON_STATUS "gb_iot/Challenge/58_PNZ_FSM/Button_status"
#define SUBSCRIBE_TOPIC_RSSI_OUT "gb_iot/Challenge/58_PNZ_FSM/WiFi.RSSI_outdoor"  // Считываем мощность сигнала WiFi уличного датчика

WebServer server(8880);
WiFiClientSecure wifi_client_publ;
PubSubClient mqtt_client_publ(wifi_client_publ);
WiFiClientSecure wifi_client_subs;
PubSubClient mqtt_client_subs(wifi_client_subs);
Adafruit_BME280 bme;
TFT_eSPI tft = TFT_eSPI(135, 240);  // Invoke custom library

enum { var_size = 24 };              // размер массива
int pressure_var[var_size] = { 1 };  // Массив для хранения значений атмосферного давления
String message = "0";
//String ip_addr_of_clients = "0";
//String white_list = "0";
int Temp = 0;
int Hum = 0;
int Pres = 0;
int Temp_outdoor = 0;
unsigned long myTimer1;  // для millis
unsigned long myTimer2;  // для millis
unsigned long myTimer3;  // для millis
bool LEDstatus = LOW;
bool Moisture = LOW;
int8_t RSSI_OUT = -110;
int humi_thresh_pwroff = 50;  // Значение влажности воздуха, при котором увлажнитель автоматически выключается, по-умолчанию
int flag = 0;
String telegram_msg = "start";  // Телеграм-бот, по-умолчанию включен

void fifo(unsigned int num, int pressure_var[], int var_size)  // Записываем число в массив хранения значений атмосферного давления
{
  for (int i = var_size - 2; i >= 0; i--) {
    pressure_var[i + 1] = pressure_var[i];
  }
  pressure_var[0] = num;
}

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, SUBSCRIBE_TOPIC_OUTDOOR_TEMP) == 0) {
    payload[length] = 0;
    message = (char*)payload;
    Temp_outdoor = (int8_t)message.toInt();
    //Serial.println(Temp_outdoor);
  }

  if (strcmp(topic, SUBSCRIBE_TOPIC_BUTTON_STATUS) == 0) {
    payload[length] = 0;
    message = (char*)payload;
    LEDstatus = (bool)message.toInt();
    //Serial.println(LEDstatus);
  }

  if (strcmp(topic, SUBSCRIBE_TOPIC_RSSI_OUT) == 0) {
    payload[length] = 0;
    message = (char*)payload;
    RSSI_OUT = (int8_t)message.toInt();
    //Serial.println(RSSI_OUT);
  }
}

void wifi_mqtt_init(void) {
  wifi_client_publ.setCACert(root_ca);
  wifi_client_publ.setCertificate(dev_cert_tx);
  wifi_client_publ.setPrivateKey(key_cert_tx);
  mqtt_client_publ.setClient(wifi_client_publ);
  mqtt_client_publ.setServer(mqtt_server, mqtt_port);

  wifi_client_subs.setCACert(root_ca);
  wifi_client_subs.setCertificate(dev_cert_rx);
  wifi_client_subs.setPrivateKey(key_cert_rx);
  mqtt_client_subs.setClient(wifi_client_subs);
  mqtt_client_subs.setServer(mqtt_server, mqtt_port);
  mqtt_client_subs.setCallback(receivedCallback);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(WiFi.localIP());

  delay(500);

  while (!mqtt_client_publ.connected()) {
    Serial.print("MQTT publish connecting ");
    if (mqtt_client_publ.connect(mqtt_server)) {
      Serial.println("OK");
    } else {
      Serial.print("failed, status code = ");
      Serial.print(mqtt_client_publ.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      break;
    }
  }

  while (!mqtt_client_subs.connected()) {
    Serial.print("MQTT suscribe connecting ");
    if (mqtt_client_subs.connect(mqtt_server)) {
      Serial.println("OK");
      mqtt_client_subs.subscribe(SUBSCRIBE_TOPIC_OUTDOOR_TEMP);
      mqtt_client_subs.subscribe(SUBSCRIBE_TOPIC_BUTTON_STATUS);
      mqtt_client_subs.subscribe(SUBSCRIBE_TOPIC_RSSI_OUT);
    } else {
      Serial.print("failed, status code = ");
      Serial.print(mqtt_client_subs.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      break;
    }
  }

  server.on("/fsm8", OnConnect);
  server.on("/ledon", ledon);
  server.on("/ledoff", ledoff);
  //
  server.on("/hand", hand_moist);
  server.on("/auto", auto_moist);
  //
  server.onNotFound(NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void buttontick() {
  //Serial.println("Interrupt");
  flag = 1;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  bme.begin(0x76);  // Инициализация датчика BME280
  wifi_mqtt_init();
  tft.init();
  tft.fillScreen(TFT_BLACK);
  digitalWrite(TFT_BL, LOW);
  tft.writecommand(TFT_DISPOFF);
  tft.writecommand(TFT_SLPIN);

  digitalWrite(LED, LOW);

  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, buttontick, FALLING);

  esp_task_wdt_init(WDT_TIMEOUT, true);  //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);                //add current thread to WDT watch

  bot.setChatID(CHAT_ID);
  bot.attach(newMsg);
}

void newMsg(FB_msg& msg) {
  Serial.println(msg.text);  // выводим текст сообщения
  telegram_msg = msg.text;
}

void loop() {

  bot.tick();
  esp_task_wdt_reset();
  server.handleClient();
  mqtt_client_subs.loop();

  //vTaskDelay(500 / portTICK_PERIOD_MS);

  ///// Включение/выключение увлажнителя воздуха
  // кнопкой в вэбе
  if (LEDstatus) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
  // кнопкой на esp32
  if (!digitalRead(BUTTON)) {
    //digitalWrite(LED, HIGH);
    digitalWrite(LED, !digitalRead(LED));
    LEDstatus = digitalRead(LED);
    delay(2000);
  }

  ///// Автовыключение увлажнителя воздуха при достижении определенного критического порога
  if (LEDstatus && (Hum >= humi_thresh_pwroff)) {
    digitalWrite(LED, LOW);
    LEDstatus = LOW;
  }

  ///// Авторабота увлажнителя воздуха
  if (Moisture && (Hum >= humi_thresh_pwroff)) {
    digitalWrite(LED, LOW);
    LEDstatus = LOW;
  }
  if (Moisture && (Hum <= (humi_thresh_pwroff - (int)5))) {
    digitalWrite(LED, HIGH);
    LEDstatus = HIGH;
  }

  if (flag) {

    //tft.drawString(message, tft.width() / 2, tft.height() / 2 + 5); // x y

    digitalWrite(TFT_BL, HIGH);
    tft.writecommand(TFT_DISPON);
    tft.writecommand(TFT_SLPOUT);

    tft.setRotation(1);
    tft.setTextSize(1);
    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(TFT_GREEN);

    //tft.drawString("mqtt_client_subs.state", 0, 0);
    //tft.drawString(mqtt_client_subs.state() == 0 ? "ok" : "not_ok", 200, 0);
    //tft.drawString("mqtt_client_publ.state", 0, 10);
    //tft.drawString(mqtt_client_publ.state() == 0 ? "ok" : "not_ok", 200, 10);

    tft.drawString("Telegram", 0, 0);
    tft.drawString(telegram_msg == "start" ? "start" : "stop", 200, 0);

    tft.drawString("mqtt_client_subs_publ.state", 0, 10);
    tft.drawString(mqtt_client_subs.state() == 0 ? "ok" : "not_ok", 180, 10);
    tft.drawString(mqtt_client_publ.state() == 0 ? "ok" : "not_ok", 200, 10);

    tft.drawString("WiFi.RSSI_indoor", 0, 20);
    message = "0";
    message = String(WiFi.RSSI(), DEC);
    tft.drawString(message, 200, 20);
    if (WiFi.RSSI() >= -70) {
      tft.drawString("excellent", 120, 20);
    }
    if (WiFi.RSSI() <= -71 && WiFi.RSSI() >= -85) {
      tft.drawString("good", 120, 20);
    }
    if (WiFi.RSSI() <= -86 && WiFi.RSSI() >= -100) {
      tft.drawString("fair", 120, 20);
    }
    if (WiFi.RSSI() <= -101 && WiFi.RSSI() > -110) {
      tft.drawString("poor", 120, 20);
    }
    if (WiFi.RSSI() <= -110) {
      tft.drawString("no_signal", 120, 20);
    }

    tft.drawString("WiFi.RSSI_outdoor", 0, 30);
    message = "0";
    message = String(RSSI_OUT, DEC);
    tft.drawString(message, 200, 30);
    if (RSSI_OUT >= -70) {
      tft.drawString("excellent", 120, 30);
    }
    if (RSSI_OUT <= -71 && RSSI_OUT >= -85) {
      tft.drawString("good", 120, 30);
    }
    if (RSSI_OUT <= -86 && RSSI_OUT >= -100) {
      tft.drawString("fair", 120, 30);
    }
    if (RSSI_OUT <= -101 && RSSI_OUT > -110) {
      tft.drawString("poor", 120, 30);
    }
    if (RSSI_OUT <= -110) {
      tft.drawString("no_signal", 120, 30);
    }


    tft.drawString("WiFi.status", 0, 40);
    tft.drawString(WiFi.status() == 3 ? "ok" : "not_ok", 200, 40);

    tft.drawString("IP adress", 0, 50);
    tft.drawString(WiFi.localIP().toString().c_str(), 150, 50);

    //Serial.println(WiFi.localIP().toString().c_str());
    //Serial.println(mqtt_client_subs.state() == 0 ? "ok" : "not_ok");
    //Serial.println(mqtt_client_publ.state() == 0 ? "ok" : "not_ok");
    //Serial.println(WiFi.status() == 3 ? "ok" : "not_ok");

    tft.drawString("Temperature", 0, 70);
    message = "0";
    message = String(Temp, DEC);
    tft.drawString(message, 200, 70);

    tft.drawString("Humidity", 0, 80);
    message = "0";
    message = String(Hum, DEC);
    tft.drawString(message, 200, 80);

    tft.drawString("Pressure", 0, 90);
    message = "0";
    message = String(Pres, DEC);
    tft.drawString(message, 200, 90);

    tft.drawString("Temperature_outdoor", 0, 100);
    message = "0";
    message = String(Temp_outdoor, DEC);
    tft.drawString(message, 200, 100);

    tft.drawString("Humidifier_status", 0, 110);
    tft.drawString(LEDstatus == 1 ? "on" : "off", 200, 110);

    tft.drawString("humi_thresh_pwroff", 0, 120);
    message = "0";
    message = String(humi_thresh_pwroff, DEC);
    tft.drawString(message, 200, 120);

    //#define MQTT_CONNECTION_TIMEOUT -4
    //#define MQTT_CONNECTION_LOST -3
    //#define MQTT_CONNECT_FAILED -2
    //#define MQTT_DISCONNECTED -1
    //#define MQTT_CONNECTED 0
    //#define MQTT_CONNECT_BAD_PROTOCOL 1
    //#define MQTT_CONNECT_BAD_CLIENT_ID 2
    //#define MQTT_CONNECT_UNAVAILABLE 3
    //#define MQTT_CONNECT_BAD_CREDENTIALS 4
    //#define MQTT_CONNECT_UNAUTHORIZED 5
    //WL_NO_SHIELD = 255,
    //WL_IDLE_STATUS = 0,
    //WL_NO_SSID_AVAIL = 1,
    //WL_SCAN_COMPLETED = 2,
    //WL_CONNECTED = 3,
    //WL_CONNECT_FAILED = 4,
    //WL_CONNECTION_LOST = 5,
    //WL_DISCONNECTED = 6

    delay(6000);

    tft.fillScreen(TFT_BLACK);
    digitalWrite(TFT_BL, LOW);
    tft.writecommand(TFT_DISPOFF);
    tft.writecommand(TFT_SLPIN);

    flag = 0;
  }

  ///// Обработчик для предупреждения о снижении температуры ниже 1 град
  if (millis() - myTimer3 >= 600000)  // 10 минут
  {
    myTimer3 = millis();

    if (telegram_msg == "start" && Temp_outdoor < 1) {
      bot.sendMessage("Warning! Temperature_outdoor < 1 degree");
    }
  }


  ///// Обработчик для записи значений атм давл в массив каждый час
  if (millis() - myTimer1 >= 3600000)  // 3600000
  {
    myTimer1 = millis();

    unsigned int tmp = (bme.readPressure() / 100.0F) * 0.750064;
    fifo(tmp, pressure_var, var_size);  // Считываем показания с датчика и записываем в массив значения атм давления, 1 раз в 1 час
  }

  ///// Обработчик для записи показаний датчика bme280 в mqtt-брокер Yandexа
  if (millis() - myTimer2 >= 10000) {
    myTimer2 = millis();

    Temp = (bme.readTemperature());
    Hum = (bme.readHumidity());
    Pres = ((bme.readPressure() / 100.0F) * 0.750064);

    char buff_msg[32];
    for (int i = 0; i < 32; i++) {
      buff_msg[i] = 0;  // Обнуляем массив
    }
    sprintf(buff_msg, "%d", Pres);
    mqtt_client_publ.publish(PUBLISH_TOPIC_PRES, buff_msg);

    for (int i = 0; i < 32; i++) {
      buff_msg[i] = 0;  // Обнуляем массив
    }
    sprintf(buff_msg, "%d", Temp);
    mqtt_client_publ.publish(PUBLISH_TOPIC_TEMP, buff_msg);

    for (int i = 0; i < 32; i++) {
      buff_msg[i] = 0;  // Обнуляем массив
    }
    sprintf(buff_msg, "%d", Hum);
    mqtt_client_publ.publish(PUBLISH_TOPIC_HUMI, buff_msg);

    for (int i = 0; i < 32; i++) {
      buff_msg[i] = 0;  // Обнуляем массив
    }
    sprintf(buff_msg, "%d", LEDstatus);
    mqtt_client_publ.publish(PUBLISH_TOPIC_BUTTON_STATUS, buff_msg);

    for (int i = 0; i < 32; i++) {
      buff_msg[i] = 0;  // Обнуляем массив
    }
    sprintf(buff_msg, "%d", WiFi.RSSI());
    mqtt_client_publ.publish(PUBLISH_TOPIC_RSSI_IN, buff_msg);
  }


  // Проверка на дисконнект
  while (!mqtt_client_publ.connected()) {
    Serial.println("mqtt_client_publ_reconnect");
    if (mqtt_client_publ.connect(mqtt_server)) {
      Serial.println("OK");
    } else {
      Serial.print("failed, status code = ");
      Serial.print(mqtt_client_publ.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      break;
    }
  }
  while (!mqtt_client_subs.connected()) {
    Serial.println("mqtt_client_subs_reconnect");
    if (mqtt_client_subs.connect(mqtt_server)) {
      Serial.println("OK");
      mqtt_client_subs.subscribe(SUBSCRIBE_TOPIC_OUTDOOR_TEMP);
      mqtt_client_subs.subscribe(SUBSCRIBE_TOPIC_BUTTON_STATUS);
      mqtt_client_subs.subscribe(SUBSCRIBE_TOPIC_RSSI_OUT);
    } else {
      Serial.print("failed, status code = ");
      Serial.print(mqtt_client_subs.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      break;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("wifi_reconnect");
    //wifi_mqtt_init();

    WiFi.begin(ssid, password);
    for (int i = 0; i < 10; i++)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("wifi_reconnected");
        break;
      }
      else
      {
        Serial.print(".");
        delay(500);
      }
    }
  }
}

void OnConnect() {
  for (int i = 0; i < server.args(); i++) {
    humi_thresh_pwroff = (int8_t)server.arg(i).toInt();
  }
  server.send(200, "text/html", SendHTML(LEDstatus, Moisture));
}
void ledon() {
  LEDstatus = HIGH;
  Serial.println("GPIO Status: ON");
  server.send(200, "text/html", SendHTML(true, Moisture));
}
void ledoff() {
  LEDstatus = LOW;
  Serial.println("GPIO Status: OFF");
  server.send(200, "text/html", SendHTML(false, Moisture));
}

//
void hand_moist() {
  Moisture = LOW;
  Serial.println("hand");
  server.send(200, "text/html", SendHTML(LEDstatus, false));
}
void auto_moist() {
  LEDstatus = LOW;
  Moisture = HIGH;
  Serial.println("auto");
  server.send(200, "text/html", SendHTML(false, true));
}
//

void NotFound() {
  server.send(404, "text/plain", "Well, what do you need here? :-) Wrong address!");
}

String SendHTML(uint8_t ledstat, uint8_t moiststat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<a href='/fsm8'>Обновить!</a>\n";
  ptr += "<title>Meteostation</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color:#3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #34495e;}\n";
  ptr += ".button-off {background-color: #3498db ;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";

  //ip_addr_of_clients = server.client().remoteIP().toString();
  //Serial.println(ip_addr_of_clients);
  //white_list = "192.168.0.100";
  //if (!ip_addr_of_clients.equals(white_list))
  //{
  //ptr += "<script>\n";
  //ptr += "<!--hide\n";
  //ptr += "var password;\n";
  //ptr += "var pass1 = '1';\n";
  //ptr += "password = prompt('Please enter your password to view this page!','');\n";
  //ptr += "if (password == pass1){key = 0; password = 0;}else{password = prompt('Please enter your password to view this page!','');}\n";
  //ptr += "//-->\n";
  //ptr += "</script>\n";
  //}

  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Weather conditions </h1>\n";

  Temp = (bme.readTemperature());
  Hum = (bme.readHumidity());
  Pres = ((bme.readPressure() / 100.0F) * 0.750064);

  message = String(Temp, DEC);
  ptr += "<p>Temperature,&nbsp ℃:</p>";
  ptr += message;

  message = String(Hum, DEC);
  ptr += "<p>Humidity,&nbsp %:</p>";
  ptr += message;

  message = String(Pres, DEC);
  ptr += "<p>Pressure,&nbsp mm Hg:</p>";
  ptr += message;

  message = String(Temp_outdoor, DEC);
  ptr += "<p>Temperature_outdoor,&nbsp ℃:</p>";
  ptr += message;

  ptr += "<p>Status of Telegram</p>";
  ptr += telegram_msg;

  if (ledstat) {
    ptr += "<p>Состояние увлажнителя воздуха: Вкл.</p><a class=\"button button-off\" href=\"/ledoff\">Откл.</a>\n";
  } else {
    ptr += "<p>Состояние увлажнителя воздуха: Откл.</p><a class=\"button button-on\" href=\"/ledon\">Вкл.</a>\n";
  }

  //
  if (moiststat) {
    ptr += "<p>Режим работы увлажнителя воздуха: Авто.</p><a class=\"button button-off\" href=\"/hand\">Ручн.</a>\n";
  } else {
    ptr += "<p>Режим работы увлажнителя воздуха: Ручн.</p><a class=\"button button-on\" href=\"/auto\">Авто.</a>\n";
  }
  //

  ptr += "<p>Поменять порог отключения увлажнителя, % </p>";
  ptr += "<form action='/fsm8'>\n";
  ptr += "<input type='number' name='hum' required  min='30' max='65'>\n";
  ptr += "<input type='submit'>\n";
  ptr += "</form>\n";
  ptr += "<p></p>";

  ptr += "<p>Порог отключения увлажнителя,&nbsp %:</p>";
  ptr += humi_thresh_pwroff;
  ptr += "<p></p>";

  ptr += "<a href='http://fsm8.ru:3000/d/aad63335-3e7a-4a63-837f-7c9be7c8cb4f/new-dashboard?orgId=1&refresh=5s&from=1693599923498&to=1693686323498'target='_blank'>Статистика из Grafana</a>\n";

  ptr += "<script src='https://www.google.com/jsapi'></script>\n";
  ptr += "<script>\n";
  ptr += "google.load('visualization', '1', {packages:['corechart']});\n";
  ptr += "google.setOnLoadCallback(drawChart);\n";

  // Формируем строку для построения графика!
  ptr += "function drawChart() {var data = google.visualization.arrayToDataTable([['Время', 'Атм. давл'],";
  for (int i = 0; i < var_size; i++)  // вывод атм давл на график
  {
    ptr += "['', ";
    message = String(pressure_var[i], DEC);
    ptr += message;
    ptr += "],";
  }
  ptr += "]);var options = {title: 'Атм давление (<-Сейчас --- 24 часа назад->)',hAxis: {title: 'Время'},vAxis: {title: 'мм. рт. ст'}};var chart = new google.visualization.ColumnChart(document.getElementById('oil'));chart.draw(data, options);}\n";
  //ptr += "function drawChart() {var data = google.visualization.arrayToDataTable([['Время', 'Атм. давл'],['24 ч назад', 700],['', 710],['', 720],['', 730],['', 740],['сейчас', 750]]);var options = {title: 'Атм давление',hAxis: {title: 'Часы'},vAxis: {title: 'мм. рт. ст'}};var chart = new google.visualization.ColumnChart(document.getElementById('oil'));chart.draw(data, options);}\n";
  ptr += "</script>\n";
  //ptr += "<div id='oil' style='width: 500px; height: 400px; '></div>\n";
  ptr += "<div id='oil' margin-left: auto; margin-right: auto></div>\n";
  ptr += "<p>Нормальное атм. давл. = 746 мм.рт.ст.</p>";

  ptr += "</body>\n";
  ptr += "</html>\n";

  return ptr;
}
