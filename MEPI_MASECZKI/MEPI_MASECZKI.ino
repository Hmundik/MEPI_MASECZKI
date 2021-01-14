#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define PIN_CZUJNIK_1 26
#define PIN_CZUJNIK_2 25
#define PIN_PRZYCISK 17
#define PIN_BUZZER 16

#define TIMEMAS 10

const char* ssid = "TP-LINK_nette";
const char* password = "aw3se4dr5";

WebServer server(8081);

int timer_mas_1 = 0;
int timer_mas_2 = 0;
char mas_ost_1 = 0;
char mas_ost_2 = 0;

void handleRoot() {
  String message = "";
  message += String(timer_mas_1);
  message += "\n";
  message += String(timer_mas_2);
  message += "\n";
  server.send(200, "text/plain", message);
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
  pinMode(PIN_BUZZER,OUTPUT);
  digitalWrite(PIN_BUZZER,HIGH);
  
  
  Serial.begin(112500);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
 
  //// Wait for connection
  Serial.println("Connectuig");
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
  xTaskCreate(task1,"task1", 10000, NULL,1,NULL);
  xTaskCreate(task2,"task2", 10000, NULL,2,NULL);
  xTaskCreate(task3,"task3", 10000, NULL,3,NULL);
}

void loop() {}

void task1( void * parameter)
{
  while(1){
    if(digitalRead(PIN_CZUJNIK_1)==1)
    {
      if((mas_ost_1==0)&&(timer_mas_1>TIMEMAS))
      {
        while(digitalRead(PIN_CZUJNIK_1)==1)
        {
          digitalWrite(PIN_BUZZER,LOW);
          vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        digitalWrite(PIN_BUZZER,HIGH);       
      }
      timer_mas_1++;
      mas_ost_1 = 1;
    } else
    {
      mas_ost_1 = 0;
    }
    if(digitalRead(PIN_CZUJNIK_2)==1)
    {
      if((mas_ost_2==0)&&(timer_mas_2>TIMEMAS))
      {
        while(digitalRead(PIN_CZUJNIK_2)==1)
        {
          digitalWrite(PIN_BUZZER,LOW);
          vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        digitalWrite(PIN_BUZZER,HIGH);       
      }
      timer_mas_2++;
      mas_ost_2 = 1;
    } else
    {
      mas_ost_2 = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task2(void * parameter)
{
  while(1)
  {
    while(digitalRead(PIN_PRZYCISK)==0) {vTaskDelay(10 / portTICK_PERIOD_MS);};
    while((digitalRead(PIN_CZUJNIK_1)==0)&&(digitalRead(PIN_CZUJNIK_2)==0)){vTaskDelay(10 / portTICK_PERIOD_MS);};
    if (digitalRead(PIN_CZUJNIK_1)==1)
    {
      timer_mas_1=0;
    }
    if(digitalRead(PIN_CZUJNIK_2)==1)
    {
      timer_mas_2=0;
    }
    while((digitalRead(PIN_CZUJNIK_1)==1)||(digitalRead(PIN_CZUJNIK_2)==1)){vTaskDelay(10 / portTICK_PERIOD_MS);};
  }
}

void task3(void * parameter)
{
  while(1)
  {
    server.handleClient();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}