/*
 * Coded by Alberto bebbo Capponi <bebbo@bebbo.it>
 * Based on http://forum.arduino.cc/index.php?topic=237539.0
 */

#ifndef Toyota_h
#define Toyota_h

// Include the standard types
#include <Arduino.h>


// Define the Toyota class
class Toyota {
	public:
	
	// Constructor
	Toyota(int PIN, boolean HIGHstatus, boolean LOWstatus);
	void status();
	void change();
	float readData(int);
	boolean isConnected = false;
	char* int2bin(unsigned int);
	
	private:
	
		int           _ECU_PIN;
		boolean       _HIGH = LOW;
		boolean       _LOW = HIGH;
		uint8_t       _CarID;
		uint8_t       _numBytes;
		uint16_t      _failBit = 0;
		unsigned long _OBDLastSuccessPacket;
		boolean       _OBDConnected = false;
		uint8_t       _data[24];
		unsigned long _last;
		float _ect2temp(float);
};


#endif //Toyota_h