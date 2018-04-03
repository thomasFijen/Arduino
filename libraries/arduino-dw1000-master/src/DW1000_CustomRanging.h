/*
*		-------------------------- DW1000 CustomRanging.h --------------------------
*		Written by: Thomas Fijen 
*		Created on: 28/03/2018
*		These are custom made functions that are used to setup and preform ranging on the decawave DW1000 UWB. This must be used in 		*		conjuntion with the DW1000 libruary obtained from: https://github.com/thotro/arduino-dw1000.
*/

#include "DW1000.h"
#include "DW1000Time.h"
#include "DW1000Device.h" 
#include "DW1000Mac.h"

//Max devices we put in the networkDevices array ! Each DW1000Device is 74 Bytes in SRAM memory for now.
#define MAX_DEVICES 4

//sketch type (anchor or tag)
#define TAG 0
#define ANCHOR 1

//debug mode
#ifndef DEBUG
#define DEBUG false
#endif


class DW1000_CustomRangingClass {
public:
	static void    initAnchors();
	static void	   startAsAnchorCustom(uint8_t veryShortAddress, const byte mode[]);
	static void    startAsTagCustom(uint8_t veryShortAddress, const byte mode[], uint8_t numNodes);
	
	static void    generalStart();
	
protected:
	// addresses of current DW1000
	static byte _longAddress[LEN_EUI];
	static byte _veryShortAddress;
	static uint8_t _numNodes;
	// self node
	static DW1000Device _selfNode;	

private:
	//other devices in the network
	static DW1000Device _networkDevices[MAX_DEVICES];
	static volatile uint8_t _networkDevicesNumber;
	static int16_t      _lastDistantDevice;
	static byte         _currentAddress[8];
	static byte         _currentShortAddress[2];
	
	//sketch type (tag or anchor)
	static int16_t          _type; //0 for tag and 1 for anchor
	// ranging counter (per second)
	static uint16_t     _successRangingCount;
	static uint32_t    _rangingCountPeriod;
	// message sent/received state
	static volatile boolean _sentAck;
	static volatile boolean _receivedAck;
	
	//methods
	static void handleSent();
	static void receiver();
	static void handleReceived();

};

extern DW1000_CustomRangingClass DW1000_CustomRanging;
