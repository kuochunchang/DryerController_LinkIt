#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>

#include "DHT.h"
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define WIFI_AP "zaku"
#define WIFI_PASSWORD "12345678"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define SITE_URL "api.mediatek.com"

#define DEVICE_KEY "viNDQqm5aP6pp0uD"
#define DEVICE_ID "D5mzZo0s"

LWiFiClient content;
DHT dht(DHTPIN, DHTTYPE);
void setup()
{
  LTask.begin();
  LWiFi.begin();
  dht.begin();
  Serial.begin(9600);
  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    Serial.println("Waiting for connected to AP...");
    delay(1000);
  }
}

boolean disconnectedMsg = false;

void loop()
{
  if (LWiFi.status() == LWIFI_STATUS_DISCONNECTED) {
    Serial.println("Reconnected to AP...");
    while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
    {
      Serial.println("Waiting for reconnected to AP...");
      delay(1000);
    }
  }
  float t = 0.0;
  float h = 0.0;

  if (dht.readHT(&t, &h))
  {
    Serial.println("\n------------------------------");
    Serial.print("temperature = ");
    Serial.println(t);
    sendValueToMCS(t, "Temperature");

    Serial.print("humidity = ");
    Serial.println(h);
    sendValueToMCS(h, "Humidity");

  }

  delay(30000);
}

void connectToAP(){
}

void sendValueToMCS(float value, String channelId) {

  Serial.println("Connecting to WebSite");
  while (0 == content.connect(SITE_URL, 80))
  {
    LWiFi.disconnect();
    while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
    {
      Serial.println("Waiting for reconnected to AP...");
      delay(1000);
    }
    
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }

  Serial.println("send HTTP GET request");
  String action = "POST /mcs/v2/devices/";
  action += DEVICE_ID;
  action += "/datapoints.csv HTTP/1.1";
  content.println(action);

  String data = channelId + ",,";
  data += value;
  Serial.println("send Data"  + data);
  int dataLength = data.length();

  content.println("Host: api.mediatek.com");
  content.print("deviceKey: ");
  content.println(DEVICE_KEY);
  content.print("Content-Length: ");
  content.println(dataLength);
  content.println("Content-Type: text/csv");
  content.println("Connection: close");
  content.println();
  content.println(data);

  // waiting for server response
  int count = 0;
  Serial.println("waiting HTTP response:");
  while (!content.available() && count <= 50)
  {
    count += 1;
    Serial.println(count);
    delay(100);
  }

  // Make sure we are connected, and dump the response content to Serial

  while (content)
  {
    int v = content.read();
    if (v != -1)
    {
      Serial.print((char)v);
    }


  }

  if (!disconnectedMsg)
  {
    Serial.println("disconnected by server");
    disconnectedMsg = true;
  }
  delay(500);
}

