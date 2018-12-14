# pc1550-arduino

`TODO: A better readme`

This uses the library from https://github.com/dougkpowers/pc1550-interface and XBee-Arduino.

## Uses
1. Arduino Uno
1. Xbee Shield


An Arduino Uno is connected to the main logic board of a DSC PC1550 security system. Look at the above repository for the diagram on how that is accomplished.

The Arduino is looping watching for changes in the security panel. When a change is detected or a timer "expires", the current status of the panel is read and pushed as a message across Xbee.

[Picture of the wasps nest of wires with Arduino in the middle](https://i.imgur.com/SA4ujbsh.jpg).

Another Xbee is listening for messages (likely using a Xbee USB Explorer) and can do what it wants with the messages. In my case, I have a node app running. The source code for that is at https://github.com/vondruska/pc1550-xbee-bridge.