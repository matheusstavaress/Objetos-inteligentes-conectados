#include <Ethernet.h>
#include "PubSubClient.h"
#include <SPI.h>

#define CLIENT_ID       "Hal"
#define PUBLISH_DELAY   3000 //conta o tempo em mil segundos
#define PUB_TOPIC "my_username/f/temperature" //Adafruit dashboard
#define PUB_TOPIC_h "my_username/f/humidity" //Adafruit dashboard
#define DHTPIN          3

#define ledPin 13 //entradas de portas com os jumpers
#define rele_1 2
#define rele_2 7
#define rele_3 8
#define rele_4 9
#define lamp 5
String ip = "";
bool statusKD = HIGH;
bool statusBD = HIGH;
bool statusGD = HIGH;
bool estadorele_1 = LOW;
bool estadorele_2 = LOW;
bool estadorele_3 = LOW;
bool estadorele_4 = LOW;
bool startsend = HIGH;
bool statuslamp = LOW;
int lichtstatus;
byte mac[] = { 0x00, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

EthernetClient ethClient;
PubSubClient mqttClient;


long previousMillis;

void setup() {
  //pinMode(4, INPUT);
  pinMode(5, INPUT);
  //pinMode(6, INPUT_PULLUP);
  pinMode(rele_1,OUTPUT);
  pinMode(rele_2,OUTPUT);
  pinMode(rele_3,OUTPUT);
  pinMode(rele_4,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(2, LOW); // ativa o pino digital 13
  digitalWrite(7, LOW); // ativa o pino digital 13
  digitalWrite(8, LOW); // ativa o pino digital 13
  digitalWrite(9, LOW); // ativa o pino digital 13


  // Configuração da comunicação serial

  Serial.begin(9600);
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  while (!Serial) {};
  Serial.println(F("MQTT Arduino"));
  Serial.println();

  Serial.print(F("Starting ethernet..."));
  if(!Ethernet.begin(mac)) Serial.println(F("failed"));
  else {
      Serial.print(F("IP: "));
      Serial.println(Ethernet.localIP());
      Serial.print(F("Subnet: "));
      Serial.println(Ethernet.subnetMask());
      Serial.print(F("Gateway: "));
      Serial.println(Ethernet.gatewayIP());
      Serial.print(F("DNS server: "));
      Serial.println(Ethernet.dnsServerIP());
      }
  Serial.println(F("Ethernet configurada via DHCP"));
  Serial.print("Endereço de IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println();

  ip = String (Ethernet.localIP()[0]);
  ip = ip + ".";
  ip = ip + String (Ethernet.localIP()[1]);
  ip = ip + ".";
  ip = ip + String (Ethernet.localIP()[2]);
  ip = ip + ".";
  ip = ip + String (Ethernet.localIP()[3]);
  //Serial.println(ip);

  // setup mqtt client
  mqttClient.setClient(ethClient);
  //mqttClient.setServer(mqttServer, 1883);
  mqttClient.setServer("broker.hivemq.com", 1883);
  // mqttClient.setServer( "raspberrypi.local",1883);
  //mqttClient.setServer("io.adafruit.com",1883);
  //mqttClient.setServer( "192.168.1.189", 1883);
  //Serial.println(F("MQTT client configured"));
  mqttClient.setCallback(callback);
  // setup DHT sensor


  Serial.println();
  Serial.println(F("Sistema operante"));
  previousMillis = millis();
  mqttClient.publish("casa/Teste/tbc/nb/ip", ip.c_str());
}

void loop() {

  statuslamp = digitalRead(5);// Leitura da LDR
  //statusGD = digitalRead(5);// Leitura da Garagem
  //statusKD = (digitalRead(6));//Leitura da Porta da Sala 
  lichtstatus = analogRead(A0);//Leitura Analogica
  estadorele_1 = digitalRead(rele_1);
  estadorele_2 = digitalRead(rele_2);
  estadorele_3 = digitalRead(rele_3);
  estadorele_4 = digitalRead(rele_4); // Estado do Rele

  // Loop para novas informações
  if (millis() - previousMillis > PUBLISH_DELAY) {
    sendData();
    previousMillis = millis();

  }

  mqttClient.loop();
}

void sendData() {

  char msgBuffer[20];


  Serial.print("A lâmpada está: ");
  Serial.println((statuslamp == LOW) ? "DESLIGADA" : "LIGADA");
  Serial.print("Rele 1: ");
  Serial.println((estadorele_1 == LOW) ? "DESLIGADO" : "LIGADO");
  Serial.print("Rele 2: ");
  Serial.println((estadorele_2 == LOW) ? "DESLIGADO" : "LIGADOO");
  Serial.print("Rele 4: ");
  Serial.println((estadorele_4 == LOW) ? "DESLIGADO" : "LIGADO");
  Serial.print("Tomada está: ");
  Serial.println((estadorele_3 == LOW) ? "DESLIGADO" : "LIGADO");
  // Definição dos topicos e publicação 
  if (mqttClient.connect(CLIENT_ID)) {

    mqttClient.publish("casa/Teste/tbc/nb/lamp", (statuslamp == LOW) ? "LIGADA" : "DESLIGADA");
    mqttClient.publish("casa/Teste/tbc/nb/garage", (statusGD == HIGH) ? "ABERTO" : "FECHADO");
    mqttClient.publish("casa/Teste/tbc/nb/bel", (statusKD == HIGH) ? "ABERTO" : "FECHADO");
    mqttClient.publish("casa/Teste/tbc/nb/l", (lamp== HIGH) ? "DESLIGADA" : "LIGADA");
    mqttClient.publish("casa/Teste/tbc/nb/relay/1", (estadorele_1 == LOW) ? "ON" : "OFF");
    mqttClient.publish("casa/Teste/tbc/nb/relay/2", (estadorele_2 == LOW) ? "ON" : "OFF");
    mqttClient.publish("casa/Teste/tbc/nb/relay/3", (estadorele_3 == LOW) ? "ON" : "OFF");
    mqttClient.publish("casa/Teste/tbc/nb/relay/4", (estadorele_4 == LOW) ? "ON" : "OFF");
    mqttClient.publish("casa/Teste/tbc/nb/ip", ip.c_str());// publish IP nr
 // Subcrição em tópicos
    mqttClient.subscribe("casa/Teste/tbc/sb");
    mqttClient.subscribe("casa/Teste/tbc/sb1");
    mqttClient.subscribe("casa/Teste/tbc/sb2");
    mqttClient.subscribe("casa/Teste/tbc/sb3");
    if (startsend) {
      // mqttClient.publish("casa/Teste/tbc/nb/relay", (estado_rele == LOW) ? "DESLIGADO" : "LIGADO");
      mqttClient.publish("casa/Teste/tbc/nb/ip", ip.c_str());
      startsend = LOW;
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char msgBuffer[20];
  Serial.print("Nova Mensagem [");
  Serial.print(topic);
  Serial.print("] ");//MQTT_BROKER
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println(payload[0]);
//-----Mensanges de cada tópico ----------
payload[length]='\0';          
    String strPayload = String((char*)payload);

if(strPayload=="a"){   
    digitalWrite(rele_1, HIGH);
}
else if (strPayload=="aa")
{
digitalWrite(rele_1, LOW);    
}

if(strPayload=="b"){   
    digitalWrite(rele_2, HIGH);
}
else if (strPayload=="bb")
{
digitalWrite(rele_2, LOW);    
}

if(strPayload=="c"){   
    digitalWrite(rele_3, HIGH);
}
else if (strPayload=="cc")
{
digitalWrite(rele_3, LOW);    
}

if(strPayload=="d"){   
    digitalWrite(rele_4, HIGH);
}
else if (strPayload=="dd")
{
digitalWrite(rele_4, LOW);    
}
}
