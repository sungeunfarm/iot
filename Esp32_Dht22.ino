#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"
DHTesp dht;

// Please input the SSID and password of WiFi
const char* ssid     = "....";
const char* password = "....";

const char* mqtt_server = "....";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
void setup(){
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
  dht.setup(4,DHTesp::DHT22);  // DHT 셋업
  pinMode(2,OUTPUT);
}

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address : ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic,byte* message,unsigned int length){
  Serial.print("Message arrived on toic: ");
  Serial.print(topic);
  Serial.print(".Message: ");
  String messageTemp;

  for(int i=0; i<length;i++){
    Serial.print((char)message[i]);
    messageTemp +=(char)message[i];
  }
  Serial.println();

  if(String(topic) == "esp32/output"){
    Serial.print("Changing output to");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(2,HIGH);
      }
      else if(messageTemp == "off"){
        Serial.println("off");
        digitalWrite(2,LOW);
      }
  }
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connecting...");
    if(client.connect("ESP32Client")){
      Serial.println("connected");
      client.subscribe("esp32/output");
    }else{
      Serial.print("failed,rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop(){
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  long now = millis();
  if(now - lastMsg>3000){
    lastMsg = now;
    float humi = dht.getHumidity();
    float temp = dht.getTemperature();
    static char msg_temp[7];
    static char msg_humi[7];
    dtostrf(temp,6,2,msg_temp);
    dtostrf(humi,6,2,msg_humi);
    client.publish("dht/temp",msg_temp);
    client.publish("dht/humi",msg_humi);
  }
}