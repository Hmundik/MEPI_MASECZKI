#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define PIN_CZUJNIK_1 26
#define PIN_CZUJNIK_2 25
#define PIN_PRZYCISK 17

const char* ssid = "........";
const char* password = "........";

WebServer server(80);

int timer_mas_1 = 0;
int timer_mas_2 = 0;

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) 
{
  pinMode(PIN_CZUJNIK_1, INPUT);
  pinMode(PIN_CZUJNIK_2, INPUT);
  pinMode(PIN_PRZYCISK, INPUT);
  
  Serial.begin(112500);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  xTaskCreate(task1,"task1", 2048, NULL,1,NULL);
  xTaskCreate(task2,"task2", 2048, NULL,2,NULL);
  xTaskCreate(task3,"task3", 2048, NULL,2,NULL);
 
  //// Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {}

void task1( void * parameter)
{
  while(1){
    if(digitalRead(PIN_CZUJNIK_1)==1)
    {
      timer_mas_1++;
    }
    if(digitalRead(PIN_CZUJNIK_2)==1)
    {
      timer_mas_2++;
    }
    Serial.print("Maseczki: ");
    Serial.print(timer_mas_1);
    Serial.print(" ");
    Serial.print(timer_mas_2);
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  
}

void task2(void * parameter)
{
  while(1)
  {
    if(digitalRead(PIN_PRZYCISK)==1)
    {
      while((digitalRead(PIN_CZUJNIK_1)==0)&&(digitalRead(PIN_CZUJNIK_2)==0)){ };
      if (digitalRead(PIN_CZUJNIK_1)==1)
      {
        timer_mas_1=0;
      }
      if(digitalRead(PIN_CZUJNIK_2)==1)
      {
        timer_mas_2=0;
      }
    }
  }
}

void task3(void * parameter)
{
  server.handleClient();
}
