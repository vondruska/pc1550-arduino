#include <XBee.h>
#include <PC1550.h>

// milliseconds to wait for a status response
#define STATUS_RESPONSE_TIMEOUT 500

#define READY_LIGHT 0
#define ZONE_1 1
#define ZONE_2 2
#define ZONE_3 3
#define ZONE_4 4
#define ZONE_5 5
#define ZONE_6 6
#define TROUBLE_LIGHT 7
#define ARDUINO_STATUS 8

enum arduino_status {
  unknown,
  booting,
  looping
};


PC1550 alarm = PC1550();

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx = Rx16Response();
TxStatusResponse txStatus = TxStatusResponse();
ModemStatusResponse msr = ModemStatusResponse();
uint8_t payload[] = { '0','0','0','0','0','0','0', '0', '1'};
int16_t cordXbeeAddress = 0x2222;
Tx16Request tx = Tx16Request(cordXbeeAddress, payload, sizeof(payload));

unsigned long previousMillis = 0; // last time update
long interval = 2000; // interval at which to do something (milliseconds)
bool startedUp = false;

void setup() {
  
   Serial.begin(57600);
   xbee.begin(Serial);

}

void loop() {
  if(!startedUp) {
    // this sends the message outbound about the startup
    sendMessage();
    startedUp = true;
  }
  
  unsigned long currentMillis = millis();
  //process a full transmisison cycle with the PC1550 controller  
  alarm.processTransmissionCycle();

  //print the state of the keypad and and PGM output to the Serial console
  if (alarm.keypadStateChanged()) {
     setState();
  }

}

void setState() {
  payload[READY_LIGHT] = alarm.ReadyLight() ? '1' : '0';
  payload[ZONE_1] = alarm.Zone1Light() ? '1' : '0';
  payload[ZONE_2] = alarm.Zone2Light() ? '1' : '0';
  payload[ZONE_3] = alarm.Zone3Light() ? '1' : '0';
  payload[ZONE_4] = alarm.Zone4Light() ? '1' : '0';
  payload[ZONE_5] = alarm.Zone5Light() ? '1' : '0';
  payload[ZONE_6] = alarm.Zone6Light() ? '1' : '0';
  payload[TROUBLE_LIGHT] = alarm.TroubleLight() ? '1' : '0';
  payload[ARDUINO_STATUS] = '2';
  sendMessage();
}


//void printState(){
//      Serial.print("\n|");
//
//      
//      alarm.ReadyLight() ? Serial.print("R") : Serial.print(" ");
//      alarm.ArmedLight() ? Serial.print("A") : Serial.print(" ");
//      alarm.MemoryLight() ? Serial.print("M") : Serial.print(" ");
//      alarm.BypassLight() ? Serial.print("B") : Serial.print(" ");
//      alarm.TroubleLight() ? Serial.print("T|") : Serial.print(" |");
//      alarm.Zone1Light() ? Serial.print("1") : Serial.print(" ");
//      alarm.Zone2Light() ? Serial.print("2") : Serial.print(" ");
//      alarm.Zone3Light() ? Serial.print("3") : Serial.print(" ");
//      alarm.Zone4Light() ? Serial.print("4") : Serial.print(" ");
//      alarm.Zone5Light() ? Serial.print("5") : Serial.print(" ");
//      alarm.Zone6Light() ? Serial.print("6|") : Serial.print(" |");
//      alarm.Beep() ? Serial.print("B| ") : Serial.print(" | ");
//
//      alarm.fireButtonTripped()  ? Serial.print("F"):Serial.print(" ");
//      alarm.auxButtonTripped()   ? Serial.print("A"):Serial.print(" ");
//      alarm.panicButtonTripped() ? Serial.print("P|"):Serial.print(" |");
//      alarm.systemArmed() ? Serial.print("Armed   |") : Serial.print("Disarmed|");
//      alarm.Zone1Tripped() ? Serial.print("1") : Serial.print(" ");
//      alarm.Zone2Tripped() ? Serial.print("2") : Serial.print(" ");
//      alarm.Zone3Tripped() ? Serial.print("3") : Serial.print(" ");
//      alarm.Zone4Tripped() ? Serial.print("4") : Serial.print(" ");
//      alarm.Zone5Tripped() ? Serial.print("5") : Serial.print(" ");
//      alarm.Zone6Tripped() ? Serial.print("6") : Serial.print(" ");
//      alarm.AlarmTripped() ? Serial.print("|A|") : Serial.print("| |");
//
//      
//}

void sendMessage() {
  xbee.send(tx);
  
    // after sending a tx request, we expect a status response
    // wait up to half second for the status response
    if (xbee.readPacket(STATUS_RESPONSE_TIMEOUT)) {
        // got a response!

        // should be a znet tx status              
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
         xbee.getResponse().getZBTxStatusResponse(txStatus);
        
         // get the delivery status, the fifth byte
           if (txStatus.getStatus() == SUCCESS) {
              
           } else {
             // offline, we assume
             // TODO print delivery status
//             lcd.println("XBee offline");
           }
        }      
    } else {
      // local XBee did not provide a timely TX Status Response -- should not happen
//       lcd.println("Request timeout");
    } 
}
