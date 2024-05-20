#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>


#define DHTPIN D2  //pin where the dht11 is connected
#define ss 15
#define rst 16

#define BAND 433E6

int readingID = 0;

int counter = 0;
String LoRaMessage = "";

float t = 0;
float h = 0;


DHT dht(DHTPIN, DHT11);

void startLoRA() {
  LoRa.setPins(ss, rst);  //setup LoRa transceiver module

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    readingID++;
    Serial.println("Starting LoRa failed!");
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
}

void startDHT() {
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

void getReadings() {

    h = dht.readHumidity();
    t = dht.readTemperature();
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));
    sendReadings();
}

void sendReadings() {

  LoRaMessage = String(readingID) + "/" + String(t) + "&" + String(h);
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();

  Serial.print("Sending packet: ");
  Serial.println(readingID);
  readingID++;
  Serial.println(LoRaMessage);
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  dht.begin();
  startDHT();
  startLoRA();
}

void loop() {
  getReadings();
}
