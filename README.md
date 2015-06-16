# ToyotaLib
OBDI ARDUINO Toyota library


Usage Example:

/*
 * PIN 7 Connected to VF1
 */
#include "PinChangeInterrupt.h"
#include "Toyota.h"

#define serialDebug true

#define TOYOTA_PIN 7

boolean ECUconnected = false;

Toyota Toyota(TOYOTA_PIN, LOW, HIGH);

void setup() {
	
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(TOYOTA_PIN), myChange, CHANGE);
	
	if (serialDebug) {
		Serial.begin(115200);
		Serial.println("System Started");
	}
	
}

void myChange() {
	Toyota.change();
}

void loop() {
	
	Toyota.status();
	
	if ( Toyota.isConnected ) {
		if (serialDebug) {
			Serial.print("???: ");
			Serial.print(Toyota.readData(0));
			Serial.println(" Unknown");
			
			Serial.print("INJ: ");
			Serial.print(Toyota.readData(1));
			Serial.println(" Injector pulse width");
			
			Serial.print("IGN: ");
			Serial.print(Toyota.readData(2));
			Serial.println(" Ignition timing angle");
			
			Serial.print("IAC: ");
			Serial.print(Toyota.readData(3));
			Serial.println(" Idle Air Control");
			
			Serial.print("RPN: ");
			Serial.print(Toyota.readData(4));
			Serial.println(" Engine speed");
			
			Serial.print("MAP: ");
			Serial.print(Toyota.readData(5));
			Serial.println(" Manifold Absolute Pressure");
			
			Serial.print("ECT: ");
			Serial.print(Toyota.readData(6));
			Serial.println(" Engine Coolant Temperature");
			
			Serial.print("TPS: ");
			Serial.print(Toyota.readData(7));
			Serial.println(" Throttle Position Sensor");
			
			Serial.print("SPD: ");
			Serial.print(Toyota.readData(8));
			Serial.println(" Speed");
			
			Serial.print("???: ");
			Serial.print(Toyota.readData(9));
			Serial.println(" UNKNOWN");
			
			Serial.print("???: ");
			Serial.print(Toyota.readData(10));
			Serial.println(" UNKNOWN");
			
			Serial.print("FLG: ");
			Serial.print(Toyota.readData(11));
			Serial.println(" map start type");
			
			Serial.print("FLG: ");
			Serial.print(Toyota.readData(12));
			Serial.println(" map diagnostics");
			
		}
	} else if (ECUconnected) {
		ECUconnected = false;
		if (serialDebug) Serial.println("Lost Connection");
	}
} 
















