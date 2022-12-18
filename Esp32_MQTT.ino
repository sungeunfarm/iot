#include <WiFi.h>
#include <PubSubClient.h>

// Please input the SSID and password of WiFi
const char* ssid     = "...";
const char* password = "...";

const char* mqtt_server = ".....";
WiFiClient espClient;
PubSubClient client(espClient);

void setup(){
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
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
}