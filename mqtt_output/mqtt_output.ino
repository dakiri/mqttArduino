/*


192.168.2.26:8085?topic=inTopic/t/action&message={"motor":"mymessage"}&retained=0&qos=0
192.168.2.26:8085?topic=/devices/ethercard/action&message={"command":"setHigh","pin":"1"}&retained=0&qos=0


mosquitto_pub -d -m '{"command":"setLow","pin":"1"}' -t "/devices/ethercard/action"
mosquitto_sub -h 127.0.0.1 -t devices/ethercard/params/#
*/

#include "Wire.h"
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <stdlib.h>

#include <aJSON.h>

#define I2C_ADDR 0x20 // relay8 I2C address
// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 2, 214 };
byte ip[] = { 192, 168, 2, 219 };

// States of all outputs
int outputStates;
int outputPin;

// Pins to read
int inputPins[] = {5,6,7};
#define numberOfInput (sizeof(inputPins)/sizeof(int))
// Store the pin value to detect change
int inputPinsStates[]={LOW,LOW,LOW};

// where input PIN states are publish
char* pubTopic="devices/ethercard/params/inputPin%2d";
// where output PIN states are publish
char* outputTopic="devices/ethercard/params/outputPin%2d";

// Use by mqtt client for publishing (size is max sizeOf pubTopic/outputTopic)
char topic[37];
char message[10];
      
EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);


// New message receive
void callback(char* topic, byte* payload, unsigned int length) {
  
  String strPayload= String((char*)payload).substring(0,length);
  char* string =(char*)payload;
  
  //Serial.print("Receive message");
  //Serial.println(strPayload);
  
  aJsonObject* jsonObject = aJson.parse(string);
  aJsonObject* jsonRequest = aJson.getObjectItem(jsonObject , "command");

  String commandTxt =String((char*)jsonRequest->valuestring);

  if (commandTxt.equals("setHigh") || commandTxt.equals("setLow")){
    
    //Serial.println("Receiving request changing state");
    jsonRequest = aJson.getObjectItem(jsonObject , "pin");

    outputPin=atol(jsonRequest->valuestring);
    
    if (outputPin>0) {
          
      if (commandTxt.equals("setHigh")) {
        set(outputPin,1);
      }
      else {
        set(outputPin,0);
      }
      sendValueToLatch(outputStates);
    }
  }
}

void set(int outputPin,byte value) {
  if (value) {
    outputStates= (byte)outputStates | (1 << outputPin-1);
  }
  else {
    outputStates = (byte)outputStates & ~(1 << outputPin-1);
  }
  publishState(outputPin,(int)value,outputTopic);
}

void setup() {
  Serial.begin( 115200 );
  Serial.println("Starting up");

  Wire.begin(); // Wake up I2C bus

  // Set I/O bank A to outputs
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // Set all of bank A to outputs
  Wire.endTransmission();
  
  Ethernet.begin(mac, ip);
  connect();
    
  for (int pinIndex = 0; pinIndex < numberOfInput; pinIndex++) {
  pinMode(pinIndex, INPUT);
  }
}

void connect() {
  if (client.connect("arduinoClient")) {
    client.subscribe("/devices/ethercard/action");
  }
}

void loop()
{
  if (!client.connected()) {
    connect();
  }
  
  client.loop();
    
  // need to be optimize with bitwise
  for (int pinIndex = 0; pinIndex < numberOfInput; pinIndex++) {
    int pinNumber=inputPins[pinIndex];
    int state=digitalRead(pinNumber);

    if (state!=inputPinsStates[pinIndex]){
      inputPinsStates[pinIndex]=state;
      publishState(pinNumber,state,pubTopic);
    }
  }
}

void publishState(int pin,int state,char* theTopic) {
  sprintf(topic,theTopic,pin);
  sprintf(message,"{state:%1d}",state);
  client.publish(topic,message);
}

void sendValueToLatch(int latchValue)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x12); // Select GPIOA
  Wire.write(latchValue); // Send value to bank A
  Wire.endTransmission();
}
