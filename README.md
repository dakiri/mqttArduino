mqttArduino
===========

A simple proof to test the Arduino mqtt library to publish digital pin state and control relay using I2C RELAY8 shield.

Subscribe to the topic inTopic and change output 7 of relay8 to up for 200ms.

To test use mosquitto_pub & mosquitto_sub :

mosquitto_pub -d -m '{"test":"0"}'  -t "inTopic"

mosquitto_sub  -h 127.0.0.1  -t outTopic/# -v

Results like :

outTopic/pin 5 {state:0}
outTopic/pin 6 {state:0}
outTopic/pin 7 {state:0}
outTopic/pin 5 {state:1}
outTopic/pin 6 {state:1}
outTopic/pin 7 {state:1}

