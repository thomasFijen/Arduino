#include <ConnectedRanging.h>

uint8_t numNodes = 2;
uint8_t veryShortAddress = 1;


void setup() {
  Serial.begin(57600);
  ConnectedRanging.init(veryShortAddress,numNodes);
  ConnectedRanging.setSelfState(0.0,0.0,0.0);
  ConnectedRanging.attachNewRange(newRange);

}

void loop() {
  ConnectedRanging.loop();  
}

void newRange(){
  DW1000Node* lastNode = ConnectedRanging.getDistantNode();
  uint8_t messageFrom = lastNode->getVeryShortAddress();
  State* remoteState = lastNode->getState();  

  /*
  SerialCoder.sendFloat(veryShortAddress,messageFrom,R,remoteState->r);
  SerialCoder.sendFloat(veryShortAddress,messageFrom,VX,remoteState->vx);
  SerialCoder.sendFloat(veryShortAddress,messageFrom,VY,remoteState->vy);
  SerialCoder.sendFloat(veryShortAddress,messageFrom,Z,remoteState->z);  
  */
  
  Serial.print(F(" Range to ")); Serial.print(messageFrom);Serial.print(F(" is: ")); Serial.print(remoteState->r);
  Serial.print(F(" m, state vx, vy, z is: ")); Serial.print(remoteState->vx);Serial.print(F(", "));
  Serial.print(remoteState->vy);Serial.print(F(", "));  Serial.print(remoteState->z);
  Serial.print(F(" m, update frequency is: "));Serial.print(lastNode->getRangeFrequency()); Serial.println(F(" Hz"));
  
}
