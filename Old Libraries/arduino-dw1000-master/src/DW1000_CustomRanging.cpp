/*
*		-------------------------- DW1000 CustomRanging.cpp --------------------------
*		Written by: Thomas Fijen 
*		Created on: 28/03/2018
*		These are custom made functions that are used to setup and preform ranging on the decawave DW1000 UWB. This must be used in conjuntion with the DW1000 libruary obtained from: https://github.com/thotro/arduino-dw1000.
*/

#include "DW1000_CustomRanging.h"
#include "DW1000Ranging.h"
#include "DW1000Device.h"


DW1000_CustomRangingClass DW1000_CustomRanging;

//other devices we are going to communicate with which are on our network:
DW1000Device DW1000_CustomRangingClass::_networkDevices[MAX_DEVICES];
byte         DW1000_CustomRangingClass::_currentAddress[8];
byte         DW1000_CustomRangingClass::_currentShortAddress[2];
int16_t      DW1000_CustomRangingClass::_lastDistantDevice    = 0; // TODO short, 8bit?
uint8_t 	 DW1000_CustomRangingClass::_numNodes = 0;

//module type (anchor or tag)
int16_t      DW1000_CustomRangingClass::_type; // TODO enum??

// addresses of current DW1000
byte DW1000_CustomRangingClass::_longAddress[LEN_EUI];
byte DW1000_CustomRangingClass::_veryShortAddress;
DW1000Device DW1000_CustomRangingClass::_selfNode;

// ranging counter (per second)
uint16_t  DW1000_CustomRangingClass::_successRangingCount = 0;
uint32_t  DW1000_CustomRangingClass::_rangingCountPeriod  = 0;

// message sent/received state
volatile boolean DW1000_CustomRangingClass::_sentAck     = false;
volatile boolean DW1000_CustomRangingClass::_receivedAck = false;




/*  -----------------------------------------------------------------------------------------------------------------------
*	This is a custom function that I added. It allows one to specify a veryshortadress (Which is the same as the anchor ID).
* 	This function also automatically defines the _networkDevices array. 
*/
void DW1000_CustomRangingClass::startAsAnchorCustom(uint8_t veryShortAddress, const byte mode[]) {
	//save the address
	for(int i=0;i<LEN_EUI;i++){
		_longAddress[i] = veryShortAddress;
	}
	_veryShortAddress = _longAddress[0];
	//write the address on the DW1000 chip
	DW1000.setEUI(_longAddress);
	Serial.print("device address: ");Serial.print(_longAddress[1]);Serial.println(_longAddress[0]);
	
	Serial.print(F("very short device address: ")); Serial.println(_veryShortAddress);
	
	
	_currentShortAddress[0] = _veryShortAddress;
	_currentShortAddress[1] = _veryShortAddress;
	
	
	//we configure the network for mac filtering

	DW1000.newConfiguration();
	DW1000.setDefaults();
	DW1000.setDeviceAddress((uint16_t)_veryShortAddress);
	DW1000.setNetworkId(0xDECA);
	DW1000.enableMode(mode);
	DW1000.commitConfiguration();
		
	//general start:
	generalStart();
	
	//defined type as anchor
	_type = ANCHOR;
	
	Serial.println("### ANCHOR ###");
	
}

/*  -----------------------------------------------------------------------------------------------------------------------
*	This is a custom function that I added. It allows one to specify a veryshortadress (Which is the same as the tag ID).
* 	This function also automatically defines the _networkDevices array. 
*/
void DW1000_CustomRangingClass::startAsTagCustom(uint8_t veryShortAddress, const byte mode[], uint8_t numNodes) {
	//save the address
	for(int i=0;i<LEN_EUI;i++){
		_longAddress[i] = veryShortAddress;
	}
	_veryShortAddress = _longAddress[0];
//	_currentAddress = _longAddress		//This statement needs to be tested, if it works then repace _longAddress with _currentAdd
	//write the address on the DW1000 chip
	DW1000.setEUI(_longAddress);
	Serial.print("device address: ");Serial.print(_longAddress[1]);Serial.println(_longAddress[0]);
	Serial.print(F("very short device address: ")); Serial.println(_veryShortAddress);
	

	_currentShortAddress[0] = _veryShortAddress;
	_currentShortAddress[1] = _veryShortAddress;
	
	//we configur the network for mac filtering
	DW1000.newConfiguration();
	DW1000.setDefaults();
	DW1000.setDeviceAddress((uint16_t)_veryShortAddress);
	DW1000.setNetworkId(0xDECA);
	DW1000.enableMode(mode);
	DW1000.commitConfiguration();
	
	generalStart();
	//defined type as tag
	_type = TAG;
	
	//Initialising the network for the anchors
	_numNodes = numNodes;
	initAnchors();
	
	// Define the _networkDevices
	
	Serial.println("### TAG ###");
}

/*-----------------------------------------------------------------------------
*	This is a function that I copied over from stevens code (ConnectedRanging.cpp).
*	It assigns the short adresses to the anchors in the tags memory
*/
// initialization function
void DW1000_CustomRangingClass::initAnchors(){
	byte address[LEN_EUI];
	byte shortaddress[2];
	uint8_t index = 0;
	uint8_t beaconID = 1;
	
	for (int i=0;i<_numNodes;i++){
		for (int j=0;j<LEN_EUI;j++){
			address[j]=beaconID;
		}
		for (int j=0;j<2;j++){
			shortaddress[j]=beaconID;
		}
		DW1000Device temp = DW1000Device(address,shortaddress);

		if(beaconID==_veryShortAddress){
			_selfNode = temp;
		}
		else if(beaconID!=_veryShortAddress){
			_networkDevices[index] = temp;
			index++;
		}
		beaconID++;

	}
}

// ------------------------------------- Modules taken from DW1000Ranging.cpp ------------------------------------

void DW1000_CustomRangingClass::generalStart() {
	// attach callback for (successfully) sent and received messages
	DW1000.attachSentHandler(handleSent);
	DW1000.attachReceivedHandler(handleReceived);
	// anchor starts in receiving mode, awaiting a ranging poll message
	
	
	if(DEBUG) {
		// DEBUG monitoring
		Serial.println("DW1000-arduino");
		// initialize the driver
		
		
		Serial.println("configuration..");
		// DEBUG chip info and registers pretty printed
		char msg[90];
		DW1000.getPrintableDeviceIdentifier(msg);
		Serial.print("Device ID: ");
		Serial.println(msg);
		DW1000.getPrintableExtendedUniqueIdentifier(msg);
		Serial.print("Unique ID: ");
		Serial.print(msg);
		char string[6];
		sprintf(string, "%02X:%02X", _currentShortAddress[0], _currentShortAddress[1]);
		Serial.print(" short: ");
		Serial.println(string);
		
		DW1000.getPrintableNetworkIdAndShortAddress(msg);
		Serial.print("Network ID & Device Address: ");
		Serial.println(msg);
		DW1000.getPrintableDeviceMode(msg);
		Serial.print("Device mode: ");
		Serial.println(msg);
	}
	
	
	// anchor starts in receiving mode, awaiting a ranging poll message
	receiver();
	// for first time ranging frequency computation
	_rangingCountPeriod = millis();
}


/* ###########################################################################
 * #### Private methods and Handlers for transmit & Receive reply ############
 * ######################################################################### */


void DW1000_CustomRangingClass::handleSent() {
	// status change on sent success
	_sentAck = true;
}
void DW1000_CustomRangingClass::handleReceived() {
	// status change on received success
	_receivedAck = true;
}

void DW1000_CustomRangingClass::receiver() {
	DW1000.newReceive();
	DW1000.setDefaults();
	// so we don't need to restart the receiver manually
	DW1000.receivePermanently(true);
	DW1000.startReceive();
}


