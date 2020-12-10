#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

void setup_wifi();
void reconnect();

struct dataSensor
{
  float temperature = 0.0f;
  float humidity = 0.0f;
};

static char payload[256];
static dataSensor data;
StaticJsonDocument<256> doc;

#define DHTPIN 6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define TOKEN "3PCFzkMPUYIstpMrwvyE"
#define DEVICEID "804b7760-f511-11ea-bcda-a5152a32a9f6"

const char broker[] = "demo.thingsboard.io";
const char publishTopic[] = "v1/devices/me/telemetry";
const char* ssid = "Ayamayaman 1";
const char* password = "cucitangan";

WiFiClient mkr1010Client;
PubSubClient mqtt(mkr1010Client);
long lastData = 0;

void setup() {

  Serial.begin(9600);
  dht.begin();
  setup_wifi();
  mqtt.setServer(broker, 1883);
  
}

void loop() {

  if (!mqtt.connected())
  {
    reconnect();
  }

  mqtt.loop();
  

  float t = 0.0F;
  float h = 0.0F;
  
  t = dht.readTemperature();
  h = dht.readHumidity();

  if (isnan(t) || isnan(h))
  {
    Serial.println("Failed to read sensor");
  }
  
  data.temperature = t;
  data.humidity = h;
 
  doc["temperature"] = data.temperature;
  doc["humidity"] = data.humidity;
  serializeJsonPretty(doc, payload);
  long now = millis();

  if (now - lastData > 10000)
  {
    lastData = now;
    Serial.println(payload);
    mqtt.publish(publishTopic, payload);
  }
   
}

void setup_wifi(){

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while( WiFi.status() != WL_CONNECTED){

    delay(500);
    Serial.print(".");   
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
}

void reconnect(){

  while(!mqtt.connected()){
    
    Serial.print("Attempting MQTT connection ....");

    if (mqtt.connect(DEVICEID, TOKEN, NULL)) { 
      
      Serial.println("Connected to MQTT Broker");
    
    }

    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println("try again in 5 second");
      delay(5000);
    }
    
    
  }
  
}