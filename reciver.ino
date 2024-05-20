#include <SPI.h>
#include <LoRa.h>
#include <ESP8266WiFi.h>


// Replace with your network credentials
String apiKey = "7WAHIMVOYWHB9J5R";  // Enter your Write API key from ThingSpeak
const char* ssid = "Conference Romm front side";         // replace with your wifi ssid and wpa2 key
const char* password = "SEC_EEE_SPECIAL";
const char* server = "api.thingspeak.com";

WiFiClient client;

#define ss 15
#define rst 16
#define dio0 4

#define BAND 433E6

int rssi;
String loRaMessage;
String t;
String h;
String readingID;
const unsigned long eventInterval = 60000L;
unsigned long previousTime = 0;

String processor(const String& var) {
  //Serial.println(var);
  if (var == "TEMPERATURE") {
    return t;
  } else if (var == "HUMIDITY") {
    return h;
  } else if (var == "RRSI") {
    return String(rssi);
  }
  return String();
}

void setup() {

  Serial.begin(115200);
  int counter;

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);  //setup LoRa transceiver module

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(2000);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!");
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void loop() {
  unsigned long currentTime = millis();

  if (currentTime - previousTime >= eventInterval) {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      Serial.print("Lora packet received: ");
      while (LoRa.available())  // Read packet
      {
        String LoRaData = LoRa.readString();
        Serial.print(LoRaData);


        int pos1 = LoRaData.indexOf('/');
        int pos2 = LoRaData.indexOf('&');
        readingID = LoRaData.substring(0, pos1);              // Get readingID
        t = LoRaData.substring(pos1 + 1, pos2);               // Get temperature
        h = LoRaData.substring(pos2 + 1, LoRaData.length());  // Get humidity
      }
      rssi = LoRa.packetRssi();  // Get RSSI
      Serial.print(" with RSSI ");
      Serial.println(rssi);
    }

    if (client.connect(server, 80))  // "184.106.153.149" or api.thingspeak.com
    {
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(t);
      postStr += "&field2=";
      postStr += String(h);

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);
    }
    previousTime = currentTime;
  }
}
