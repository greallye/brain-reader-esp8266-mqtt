#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <Brain.h>

const char *ssid = "your ssid";
const char *pass = "your wifi pass";

String attention;
String signalQuality;
String delta;
String theta;
String alpha;
String gammaLow;
String beta;
String csvData;

const char *mqtt_name = "brain";

const char *mqtt_id = "shiftr id";
const char *mqtt_password = "shiftr password";

WiFiClient net;
MQTTClient client;
Brain brain(Serial);

unsigned long lastMillis = 0;

void connect(); // <- predefine connect() for setup()

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  client.begin("broker.shiftr.io", net);
  connect();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(mqtt_name, mqtt_id, mqtt_password)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");

}

void loop() {
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
  // mqtt can only except strings so first we convert the numbers from headset to strings.
  if (brain.update()) {
    Serial.println(brain.readCSV());
    csvData =  String(brain.readCSV());
    signalQuality = String(brain.readSignalQuality());
    attention = String(brain.readAttention());
    delta = String(brain.readDelta());
    theta = String(brain.readTheta());
    alpha = String(brain.readLowAlpha());
    gammaLow = String(brain.readLowGamma());
    beta = String(brain.readLowBeta());
  }
  
  // publish each number
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/signal", signalQuality);
    client.publish("/attention", attention);
    client.publish("/delta", delta);
    client.publish("/theta", theta);
    client.publish("/alpha", alpha);
    client.publish("/beta", beta);
    client.publish("/gamma", gammaLow);
    client.publish("/csv", csvData);
  }
}



void messageReceived(String topic, String payload, char * bytes, unsigned int length) {


}
