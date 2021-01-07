#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define PERIOD 100
#define PIN_CZUJNIK_1 2
#define PIN_CZUJNIK_2 3
#define PIN_PRZYCISK 4

const char* ssid = "........";
const char* password = "........";

WebServer server(80);

uint32_t timer_glowny = 0;
int timer_mas_1 = 0;
int timer_mas_2 = 0;



void handleRoot() {
  server.send(200, "text/plain", String(timer_mas_1)+"\n"+String(timer_mas_2));
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
  
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
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

void loop(void) 
{
  if (millis() - timer_glowny >= PERIOD) 
  {
    if(digitalRead(PIN_CZUJNIK_1)==0)
    {
      timer_mas_1++;
    }
    if(digitalRead(PIN_CZUJNIK_2)==0)
    {
      timer_mas_2++;
    }
    if(digitalRead(PIN_PRZYCISK)==0)
    {
      while((digitalRead(PIN_CZUJNIK_1)==1)&&(digitalRead(PIN_CZUJNIK_2)==1))
      {
        delay(100);
      }
      if (digitalRead(PIN_CZUJNIK_1)==0)
      {
        timer_mas_1=0;
        while(digitalRead(PIN_CZUJNIK_1)==0) delay(100);
      } else
      {
        timer_mas_2=0;
        while(digitalRead(PIN_CZUJNIK_2)==0) delay(100);
      }
    }
    Serial.println(timer_mas_1);
    Serial.println(timer_mas_2);
    Serial.println();
    server.handleClient();
    do 
    {
      timer_glowny += PERIOD;
      if (timer_glowny < PERIOD) break;
    } 
    while (timer_glowny < millis() - PERIOD);
  }
}
