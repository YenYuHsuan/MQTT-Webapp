/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <BridgeClient.h> 
#include <math.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 12);
IPAddress server(140, 115, 70, 160);
int sensorPin = A5; // select the input pin for the potentiometer
double T=0.0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//EthernetClient ethClient;
BridgeClient yunClient;
//PubSubClient client(ethClient);
PubSubClient client(yunClient);


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("connectstatus","connected");
      // ... and resubscribe
      client.subscribe("connectstatus");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

double Thermistor(int RawADC) {     //Temp test
  double Temp;
  Temp = log(10000.0*((1024.0/RawADC-1))); 
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
   //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
   return Temp;
}

void TempRead(){
 int readVal=analogRead(sensorPin);
 double temp =  Thermistor(readVal);
 if(T!=temp){
  T=temp;
  String Tstr=String(temp);
  char Tchrary[8];
  Tstr.toCharArray(Tchrary,8);
  
  client.publish("Temp",Tchrary);
 // client.subscribe("outTopic");
 }
 Serial.println(temp);  // display tempature
 
 //Serial.println(readVal);  // display tempature
 
 delay(2000);  
}

void setup()
{
  Serial.begin(115200);
  Bridge.begin();
  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  TempRead();
  client.loop();
}
