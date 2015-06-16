/*
 * Coded by Alberto bebbo Capponi <bebbo@bebbo.it>
 * Based on http://forum.arduino.cc/index.php?topic=237539.0
 */
 
#include <Arduino.h>
#include <Toyota.h>

// Constructor
Toyota::Toyota(int PIN, boolean HIGHstatus, boolean LOWstatus) {
	
	_ECU_PIN = PIN;
	_HIGH    = HIGHstatus;
	_LOW     = LOWstatus;
	
	pinMode(_ECU_PIN, INPUT_PULLUP);
	
}

void Toyota::status() {
	
	// if found bytes
	if (_numBytes > 0)  {
		_last = millis(); // set last success
		isConnected = true; // set connected to true
		_numBytes = 0; // reset the counter.
	} else if (_last + 3500 < millis() && isConnected) {
		isConnected = false;
	}
	
	if (_failBit > 0) {
		_failBit = 0; 
		//if (((ToyotaFailBit - 4) % 11) == 0) Serial.print(" (StartBit)");
		//else if (((ToyotaFailBit - 4) % 11) > 8) Serial.print(" (StopBit)");
	}
}

void Toyota::change() {
	static uint8_t _ID, _buffer[24];
	static boolean _inPacket = false;
	static unsigned long _startMS;
	static uint16_t _BitCount;
	
	int _state = digitalRead(_ECU_PIN);
	if (_inPacket == false) {
    	if (_state == _HIGH) {
			_startMS = millis();
		} else {
			if ((millis() - _startMS) > (15 * 8)) {
				_startMS = millis();
				_inPacket = true;
				_BitCount = 0;
			}
		}
	} else {
		uint16_t _bits = ((millis() - _startMS) + 1) / 8;  // The +1 is to cope with slight time errors
		_startMS = millis();
		while (_bits > 0) {
			if (_BitCount < 4) {
				if (_BitCount == 0) _ID = 0;
				_ID >>= 1;
				if (_state == _LOW) _ID |= 0x08;
			} else {
				uint16_t _bitpos = (_BitCount - 4) % 11;
				uint16_t _bytepos = (_BitCount - 4) / 11;
				if (_bitpos == 0) {
					// Start bit, should be LOW
					if (_state != _HIGH) {
						_failBit = _BitCount;
						_inPacket = false;
						break;
					}
				} else if (_bitpos < 9) {
					_buffer[_bytepos] >>= 1;
					if (_state == _LOW) _buffer[_bytepos] |= 0x80;
				} else {
					// Stop _bits, should be HIGH
					if (_state != _LOW) {
						_failBit = _BitCount;
						_inPacket = false;
						break;
					}
					if ( (_bitpos == 10) && ((_bits > 1) || (_bytepos == (24 - 1))) ) {
						_numBytes = 0;
						_CarID = _ID;
						for (int i=0; i<=_bytepos; i++) {
							_data[i] = _buffer[i];
						}
						_numBytes = _bytepos + 1;
						if (_bits >= 16)  {
							// Stop _bits of last byte were 1's so detect preamble for next packet
							_BitCount = 0;
						} else {
							_failBit = _BitCount;
							_inPacket = false;
						}
						break;
					}
				}
			}
			++_BitCount;
			--_bits;
		}
	}
}

float Toyota::readData(int OBDid) {
	float returnValue;
	switch (OBDid) {
		case 0:// UNKNOWN
			returnValue = _data[OBDid]; 
			break;
		case 1: //  Injector pulse width (INJ) - in milisec
			returnValue = _data[OBDid]/10;
			break;
		case 2: // Ignition timing angle (IGN) - degree- BTDC
			returnValue = _data[OBDid]-90;
			break;
		case 3: //Idle Air Control (IAC) - Step # X = 125 = open 100%
			returnValue = _data[OBDid]/125*100;
			break;
		case 4: //Engine speed (RPM)
			returnValue = _data[OBDid]*25;
			break; 
		case 5: //Manifold Absolute Pressure (MAP) - kPa Abs
			returnValue = _data[OBDid];
			break;  
		case 6: // Engine Coolant Temperature (ECT)
			returnValue = _ect2temp(_data[OBDid]);
			break;
		case 7: // Throttle Position Sensor (TPS) - DEGREE
			returnValue = _data[OBDid]/2;
			break;       
		case 8: // Speed (SPD) - km/h
			returnValue = _data[OBDid];
			break; 
		case 9:// UNKNOWN
			returnValue = _data[OBDid]; 
			break;
		case 10:// UNKNOWN
			returnValue = _data[OBDid]; 
			break;   
		case 11:// FLAG #1
			returnValue = _data[OBDid];
			break;
		case 12:// FLAG # 2
			returnValue = _data[OBDid];
			break;   
		default: // DEFAULT CASE (in no match to number)
			// send "error" value
			returnValue =  9999.99;
	} // end switch
   
   return returnValue;
}




// integer to string conversion
char* Toyota::int2bin(unsigned int x) {
	static char buffer[17];
	for (int i=0; i<16; i++) buffer[15-i] = '0' + ((x & (1 << i)) > 0);
	buffer[16] ='\0';
	return buffer;
}

// convert ect reating to celcius
float Toyota::_ect2temp(float ect) {
	float res;
	if (ect >= 244)      res = (ect - 244) * 10.0 + 132.0;
	else if (ect >= 238) res = (ect - 238) * 4.0 + 103.0;
	else if (ect >= 228) res = (ect - 228) * 2.1 + 80.0;
	else if (ect >= 210) res = (ect - 210) * 1.11 + 60.0;
	else if (ect >= 180) res = (ect - 180) * 0.666 + 40.0;
	else if (ect >= 135) res = (ect - 135) * 0.444 + 20.0;
	else if (ect >= 82)  res = (ect - 82) * 0.377 + 0.0;
	else if (ect >= 39)  res = (ect - 39) * 0.465 + (-20.0);
	else if (ect >= 15)  res = (ect - 15) * 0.833 + (-40.0);
	else  res = (ect * 2.0) + (-70.0);
	return res;
}