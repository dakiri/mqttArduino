#include "Wire.h"
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>


#define I2C_ADDR  0x20  // 0x20 is the address with all jumpers removed
// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 2, 214 };
byte ip[]     = { 192, 168, 2, 219 };

// Pin to read
int inputPins[] = {5,6,7};
// Store the pin value to detect change
int inputPinsStates[]={LOW,LOW,LOW};

// mqtt topic & message
char topic[15];
char message[10];
      
      
unsigned long start ;
unsigned long end ;
unsigned long delta ;

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);



void callback(char* topic, byte* payload, unsigned int length) {
  sendValueToLatch(64);
  delay(200);
  sendValueToLatch(0);
  Serial.println("Receive message");
}


void setup()
{
  Serial.begin( 115200 );
  Serial.println("Starting up");

  Wire.begin(); // Wake up I2C bus

  // Set I/O bank A to outputs
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // Set all of bank A to outputs
  Wire.endTransmission();
  
  Ethernet.begin(mac, ip);
  if (client.connect("arduinoClient")) {
    client.subscribe("inTopic");
  }

}

void loop()
{
  start = micros();
  client.loop();
  for (int pinIndex = 0; pinIndex < 3; pinIndex++) { 
    
    int pinNumber=inputPins[pinIndex];
    int state=digitalRead(pinNumber);

    if (state!=inputPinsStates[pinIndex]){
      inputPinsStates[pinIndex]=state;

      sprintf(topic,"outTopic/pin%2d",pinNumber);
      sprintf(message,"{state:%1d}",state);
      client.publish(topic,message);
    }
  }


}

void sendValueToLatch(int latchValue)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x12);        // Select GPIOA
  Wire.write(latchValue);  // Send value to bank A
  Wire.endTransmission();
}
