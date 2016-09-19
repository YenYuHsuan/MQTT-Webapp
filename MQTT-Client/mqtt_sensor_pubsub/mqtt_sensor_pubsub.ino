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
int VRaw; //This will store our raw ADC data
int IRaw;
float VFinal; //This will store the converted data
float IFinal;


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
float Volt_mapping(float input, float in_min, float in_max, float out_min, float out_max){
  return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void AttoPilot_IV_sensor(){
  //Measurement
  VRaw = analogRead(A0);
  IRaw = analogRead(A1);

  //Conversion
  // VFinal = VRaw/49.44; //45 Amp board
  //Max=8.62 min=8.31
  float Battery_VMax=8.62, Battery_Vmin=8.31;
  
  VFinal = VRaw/13.6; //90 Amp board
  Serial.print("VFinal=");
  Serial.print(VFinal);
  Serial.println("");
  float VFinalmap =constrain(Volt_mapping(VFinal , Battery_Vmin, Battery_VMax , 0 , 100), 0, 100); //90 Amp board
  //VFinal = VRaw/12.99; //180 Amp board  
  Serial.print("VFinalmap=");
  Serial.print(VFinalmap);
  Serial.println("");
  //IFinal = IRaw/14.9; //45 Amp board
  IFinal = IRaw; //90 Amp board
  //IFinal = IRaw/3.7; //180 Amp board
  
  String Vstr=String(VFinalmap);
  char Vchrary[8];
  Vstr.toCharArray(Vchrary,8);
  Serial.print("Vstr=");
  Serial.print(Vstr);
  Serial.println("");
  Serial.print("Vchrary=");
  Serial.print(Vchrary);
  Serial.println("");
  client.publish("Volts",Vchrary);
  
  Vstr=String(VFinalmap);
  Vstr.toCharArray(Vchrary,8);
  
  Serial.print(VFinal);
  Serial.println("   Volts");
  Serial.print(IFinal);
  Serial.println("   Amps");
  Serial.println("");
  Serial.println("");
  delay(2000);
}

/*
S   -> A5
mid -> GND
-   -> 5V
*/

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
  AttoPilot_IV_sensor();
  client.loop();
}
