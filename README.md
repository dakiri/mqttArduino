mqttArduino
===========

A simple proof to test the Arduino mqtt library to publish digital pin state and control relay using I2C RELAY8 shield (tested on EtherTen from Freetronics).

To test use mosquitto_pub & mosquitto_sub example : 

<pre>
mosquitto_pub -d -m '{"command":"setHigh","pin":"8"}'  -t "/devices/ethercard/action"
mosquitto_sub  -h 127.0.0.1  -t devices/ethercard/params/# 
</pre>

<b>Results :</b>

<pre>
devices/ethercard/params/inputPin 6 {state:1}
devices/ethercard/params/outputPin 1 {state:0}
</pre>

