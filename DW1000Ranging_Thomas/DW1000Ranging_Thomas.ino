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

uint8_t veryShortAddress = 5;
uint8_t numNodes = 5;
bool debug = 0;

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
  DW1000Ranging.useRangeFilter(true);
  
  //we start the module as a tag using the predefined address
  //DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
  DW1000Ranging.startAsTagCustom(veryShortAddress, DW1000.MODE_LONGDATA_RANGE_ACCURACY, numNodes);
}

void loop() {
  DW1000Ranging.loop();
}

void newRange() {

  DW1000Device* lastNode = DW1000Ranging.getDistantDevice();
  uint8_t* messageFrom = lastNode->getByteShortAddress();

  if(debug == 0)
  {
    sendFloat2(*messageFrom, lastNode->getRange());
  }
  else
  {
  //-----------Debugging
    Serial.print("Range to: "); Serial.print(*messageFrom); Serial.print(" is: ");Serial.println(lastNode->getRange());
  }  
  //Serial.print("Range to: "); Serial.print(*messageFrom); Serial.print(" is: ");Serial.println(lastNode->getRange());
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
*  This function was added by me to send the floats in the correct format
*/
void sendFloat2(uint8_t fromMessage, float outfloat){
  byte floatbyte[FLOAT_SIZE];
  memcpy(floatbyte,&outfloat,FLOAT_SIZE);

  Serial.write(START_MARKER);
  Serial.write(fromMessage);
  Serial.write(floatbyte,4);
}


