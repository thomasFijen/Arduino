#include <ConnectedRanging.h>

uint8_t numNodes = 4;
uint8_t veryShortAddress = 4;

uint8_t debug = 0;

void setup() {
  Serial.begin(57600);
  ConnectedRanging.init(veryShortAddress,numNodes);
  ConnectedRanging.setSelfState(2,2);
  ConnectedRanging.attachNewRange(newRange);

}

void loop() {
  ConnectedRanging.loop();  
}

void newRange(){
  DW1000Node* lastNode = ConnectedRanging.getDistantNode();
  uint8_t messageFrom = lastNode->getVeryShortAddress();
  State* remoteState = lastNode->getState();  
 
  if (debug == 0){
    SerialCoder.sendFloat2(R,messageFrom,remoteState->r);
    SerialCoder.sendFloat2(X,messageFrom,remoteState->x);
    SerialCoder.sendFloat2(Y,messageFrom,remoteState->y);
    SerialCoder.sendFloat2(D,messageFrom,remoteState->d);   
  }else {
    Serial.print(F(" Range to ")); Serial.print(messageFrom);Serial.print(F(" is: ")); Serial.print(remoteState->r);
    Serial.print(F(" m, state x, y is: ")); Serial.print(remoteState->x);Serial.print(F(", "));
    Serial.print(remoteState->y);
    Serial.print(F(" m, update frequency is: "));Serial.print(lastNode->getRangeFrequency()); Serial.print(F(" Hz"));   
    Serial.print(F(", Depot Flag = ")); Serial.println(remoteState->d);
  }
  
}
