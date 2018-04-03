/**
 * 
 * @todo
 *  - move strings to flash (less RAM consumption)
 *  - fix deprecated convertation form string to char* startAsTag
 *  - give example description
 */
#include <SPI.h>
#include "DW1000Ranging.h"

// connection pins
const uint8_t PIN_RST = 9; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = SS; // spi select pin

#define MAX_MESSAGE 10
//#define END_MARKER 255
#define SPECIAL_BYTE 253
#define START_MARKER 0xFE//254
#define FLOAT_SIZE 4
#define STATE_SIZE 3
//byte _tempBuffer2[MAX_MESSAGE];
//byte _dataSendCount = 0;
//byte _dataTotalSend = 0;

uint8_t veryShortAddress = 4;
uint8_t numNodes = 4;

void setup() {
  Serial.begin(57600);
  delay(1000);
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //Enable the filter to smooth the distance
  //DW1000Ranging.useRangeFilter(true);
  
  //we start the module as a tag
  //DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
  DW1000Ranging.startAsTagCustom(veryShortAddress, DW1000.MODE_LONGDATA_RANGE_ACCURACY, numNodes);
}

void loop() {
  DW1000Ranging.loop();
}

void newRange() {
  /*
  Serial.print("from: "); Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  Serial.print("\t Range: "); Serial.print(DW1000Ranging.getDistantDevice()->getRange()); Serial.print(" m");
  Serial.print("\t RX power: "); Serial.print(DW1000Ranging.getDistantDevice()->getRXPower()); Serial.println(" dBm");
*/
 // Serial.write(DW1000Ranging.getDistantDevice()->getRange());
 //SerialCoder.sendFloat2(DW1000Ranging.getDistantDevice()->getRange());

  DW1000Device* lastNode = DW1000Ranging.getDistantDevice();
  uint8_t* messageFrom = lastNode->getByteShortAddress();
  sendFloat2(*messageFrom, lastNode->getRange());
 
  //-----------Debugging
 // Serial.print("Range to: "); Serial.print(*messageFrom); Serial.print(" is: ");Serial.println(lastNode->getRange());
}

void newDevice(DW1000Device* device) {
  Serial.print("ranging init; 1 device added ! -> ");
  Serial.print(" short:");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device) {
 // Serial.print("delete inactive device: ");
 // Serial.println(device->getShortAddress(), HEX);
}

/*
*  This function was added by me to send the floats...
*/
void sendFloat2(uint8_t fromMessage, float outfloat){
  byte floatbyte[FLOAT_SIZE];
  memcpy(floatbyte,&outfloat,FLOAT_SIZE);
  uint8_t temp[2];
//  temp[0] = 0x00;
//  temp[1] = fromMessage;
  
  //encodeHighBytes(floatbyte,FLOAT_SIZE);
  Serial.write(START_MARKER);
  Serial.write(fromMessage);
  Serial.write(floatbyte,4);
  //Serial.write(_tempBuffer2,_dataTotalSend);
  //Serial.write(END_MARKER);

}

/**
 * Function that encodes the high bytes of the serial data to be sent.
 * Start and end markers are reserved values 254 and 255. In order to be able to send these values,
 * the payload values 253, 254, and 255 are encoded as 2 bytes, respectively 253 0, 253 1, and 253 2.

void encodeHighBytes(byte* sendData, uint8_t msgSize){
  _dataSendCount = msgSize;
  _dataTotalSend = 0;
  for (uint8_t i = 0; i < _dataSendCount; i++){
    if (sendData[i] >= SPECIAL_BYTE){
      _tempBuffer2[_dataTotalSend] = SPECIAL_BYTE;
      _dataTotalSend++;
      _tempBuffer2[_dataTotalSend] = sendData[i] - SPECIAL_BYTE;
    }
    else{
      _tempBuffer2[_dataTotalSend] = sendData[i];
    }
    _dataTotalSend++;
  }
} */


