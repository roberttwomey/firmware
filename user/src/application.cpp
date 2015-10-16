#include "application.h"
#include "OSC/OSCMessage.h"
#include "OSC/OSCBundle.h"

//----- OUTPUTS
int ledPin = D7;


//----- REGISTERED OSC COMMANDS
char OscCmd_led[5] = "/led";							// 4 characters  + 1 for the "null-terminated string" -> '\0'

// for testing
char OscCmd_TestSendMsg[13] = "/sendTestMsg";			// 12 characters + 1 for the "null-terminated string" -> '\0'
char OscCmd_TestSendBndl[14] = "/sendTestBndl";			// 13 characters + 1 for the "null-terminated string" -> '\0'
char OscCmd_SwitchToMessages[16] = "/manageMessages";	// 15 characters + 1 for the "null-terminated string" -> '\0'
char OscCmd_SwitchToBundles[15] = "/manageBundles";		// 14 characters + 1 for the "null-terminated string" -> '\0'

// pin readings
char OscCmd_SetReadMask[13] = "/setReadMask"; // 13 characters + 1 null terminator
char OscCmd_ReadPins[10] = "/readPins"; // 9 characters + 1 null terminator


//----- IP ADRESSES
// IPAddress computerIPAddress = IPAddress(10, 1, 10, 78);	// put the IP address of your computer here
// IPAddress computerIPAddress = IPAddress(192, 168, 0, 10);	// IP address of house event recorder

IPAddress computerIPAddress = IPAddress(192, 168, 0, 2);	// IP address of house event recorder


IPAddress coreIPAddress;
OSCMessage coreIPMessage("/coreip");


//----- PORTS
#define LOCALPORT  8888		// to send data to the Spark Core (from the computer)
#define REMOTEPORT 9999		// to send data to the computer (from here)


//----- MANAGING OSC MESSAGES OR OSC BUNDLES
int manageMessages = 0;
int manageBundles = 0;

//----- SMART HOME SENSORS
#define NUM_INPUT_PINS 4
int digitalInPins[NUM_INPUT_PINS] = { D0, A1, A2, A3}; // { A0, A1, A2, A3};

int pinValues[NUM_INPUT_PINS] = {HIGH, HIGH, HIGH, HIGH};
int lastPinValues[NUM_INPUT_PINS] = {HIGH, HIGH, HIGH, HIGH};

		
//----- UDP + overloading the inappropriate UDP functions of the Spark Core (REQUIRED !)
// class myUDP : public UDP {
// private :
// 	uint8_t myBuffer[512];
// 	int offset = 0;
// public :
// 	virtual int beginPacket(IPAddress ip, uint16_t port){
// 		offset = 0;
// 		return UDP::beginPacket(ip, port);
// 	};
// 	virtual int endPacket(){
// 		return UDP::write(myBuffer, offset);
// 	};
// 	virtual size_t write(uint8_t buffer) {
// 		write(&buffer, 1);
// 		return 1;
// 	}
// 	virtual size_t write(const uint8_t *buffer, size_t size) {
// 		memcpy(&myBuffer[offset], buffer, size);
// 		offset += size;
// 		return size;
// 	}
// };
//
// myUDP Udp;

UDP Udp;

//=========================================================================================
//=========================================================================================
void setup()
{
	// Initialize A0 - A7 as digital input with pullup. 
	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);
	pinMode(A3, INPUT_PULLUP);
	pinMode(A4, INPUT_PULLUP);
	pinMode(A5, INPUT_PULLUP);
	pinMode(A6, INPUT_PULLUP);
	pinMode(A7, INPUT_PULLUP);
	
	pinMode(D0, INPUT_PULLUP);
	
	// Initialize D7 as LED output
	pinMode(ledPin, OUTPUT); 
	
	// Start UDP
	Udp.begin(LOCALPORT);

	// Get the IP address of the Spark Core and send it as an OSC Message
	coreIPAddress = WiFi.localIP();
	coreIPMessage.add(coreIPAddress[0]).add(coreIPAddress[1]).add(coreIPAddress[2]).add(coreIPAddress[3]);

	Udp.beginPacket(computerIPAddress, REMOTEPORT);
	coreIPMessage.send(Udp);
	Udp.endPacket();

	// First manage OSCMessages and not OSCBundles
	manageMessages = 1;
}


//=========================================================================================
//===== TEST receiving a request to take OSC Bundles into account
//=========================================================================================
void switchToBundles(OSCMessage &mess) { manageMessages = 0; manageBundles = 1;}


//=========================================================================================
//===== TEST receiving a request to take OSC Messages into account
//=========================================================================================
void switchToMessages(OSCMessage &mess) { manageBundles = 0; manageMessages = 1; }


//=========================================================================================
//===== TEST receiving an OSC Message made of the address "/led" and containing 2 ints : led number , led status (0=LOW , 1=HIGH)
//=========================================================================================
void setLEDStatus(OSCMessage &mess)
{
	if (mess.size() == 2 && mess.isInt(0) && mess.isInt(1)) {
		int thisLed = mess.getInt(0);
		int thisStatus = mess.getInt(1);

		// if (thisLed == 0 && thisStatus == 0) {digitalWrite(ledPin,LOW);}
		// else if (thisLed == 0 && thisStatus == 1) {digitalWrite(ledPin,HIGH);}
		// else if (thisLed == 1 && thisStatus == 0) {digitalWrite(led2,LOW);}
		// else if (thisLed == 1 && thisStatus == 1) {digitalWrite(led2,HIGH);}

		// if (thisLed == 0 && thisStatus == 0) {digitalWrite(ledPin,LOW);}
		// else if (thisLed == 0 && thisStatus == 1) {digitalWrite(ledPin,HIGH);}


		//	else {
		//		return;			// ERRORS : inappropriate led number or led status
		//	}
	}
	//	else {
	//		return;			// ERRORS : bad message size (!= 2) or the arguments are not ints
	//	}
}


//=========================================================================================
//===== TEST sending an OSCMessage (called when the Spark Core receives "/sendTestMsg" as an OSC Message)
//=========================================================================================
void sendOSCTestMsg(OSCMessage &mess)
{
	OSCMessage testMsg_toSend("/testmessage");
	testMsg_toSend.add((float)2.6).add(150).add("hahaha").add(-1000);

	Udp.beginPacket(computerIPAddress, REMOTEPORT);
	testMsg_toSend.send(Udp); // send the bytes
	Udp.endPacket();
	testMsg_toSend.empty(); // empty the message to free room for a new one
}

//=========================================================================================
//===== TEST sending an OSCBundle (called when the Spark Core receives "/sendTestBndl" as an OSC Message)
//=========================================================================================
void sendOSCTestBndl(OSCMessage &mess)
{
	OSCBundle testBndl_toSend;

	//OSCBundle's "add" returns the OSCMessage so the message's "add" can be composed together
	testBndl_toSend.add("/testbundle/msg1").add((float)5.6).add(250).add("hohoho").add(-2000);
	testBndl_toSend.add("/testbundle/msg2").add(124).add("hehehe").add((float)1.6).add(-150);

	testBndl_toSend.setTimetag((uint64_t)millis());
	// NB : millis() is "the number of milliseconds since the processor started up", not an appropriate NTP timetag !
	// setTimetag(oscTime()) DOESN'T WORK ON THE SPARK CORE (Oscuino has to rewrite the OSCTiming class)

	Udp.beginPacket(computerIPAddress, REMOTEPORT);
	testBndl_toSend.send(Udp); // send the bytes
	Udp.endPacket();
	testBndl_toSend.empty(); // empty the bundle to free room for a new one
}

//=========================================================================================
//===== house machine stuff
//=========================================================================================


void sendOSCPinsBndl(OSCMessage &mess)
{
	
	OSCBundle testBndl_toSend;

	//OSCBundle's "add" returns the OSCMessage so the message's "add" can be composed together
	testBndl_toSend.add("/digital/a0").add(digitalRead(A0));
	testBndl_toSend.add("/digital/a1").add(digitalRead(A1));
	testBndl_toSend.add("/digital/a2").add(digitalRead(A2));
	testBndl_toSend.add("/digital/a3").add(digitalRead(A3));
	testBndl_toSend.add("/digital/a4").add(digitalRead(A4));
	testBndl_toSend.add("/digital/a5").add(digitalRead(A5));
	testBndl_toSend.add("/digital/a6").add(digitalRead(A6));
	testBndl_toSend.add("/digital/a7").add(digitalRead(A7));

	testBndl_toSend.setTimetag((uint64_t)millis());
	
	Udp.beginPacket(computerIPAddress, REMOTEPORT);
	testBndl_toSend.send(Udp); // send the bytes
	Udp.endPacket();
	testBndl_toSend.empty(); // empty the bundle to free room for a new one
	
	// OSCMessage testMsg_toSend("/digital");
	// testMsg_toSend.add((float)2.6).add(150).add("hahaha").add(-1000);

}

void sendOSCPinsMsg(OSCMessage &mess) {
	
	OSCMessage testMsg_toSend("/digitalpins");

	// from https://www.arduino.cc/en/Tutorial/BitMask
	for (int i=0; i < NUM_INPUT_PINS; i++) {
		// A0; i <= A7; i++) {
		int thisPin = digitalInPins[i];
		// read this pin
		if(digitalRead(thisPin) == HIGH) {
	      testMsg_toSend.add(1);
			  //digitalWrite(transmit,HIGH); // send 1
	    }
	    else{ //if bitwise and resolves to false
	      testMsg_toSend.add(0);
		  // digitalWrite(transmit,LOW); // send 0
	    }
	}
	
	// }
	// for(byte mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
	//     if (data & mask){ // if bitwise AND resolves to true
	//       testMsg_toSend.add(1);
	// 		  //digitalWrite(transmit,HIGH); // send 1
	//     }
	//     else{ //if bitwise and resolves to false
	//       testMsg_toSend.add(0);
	// 	  // digitalWrite(transmit,LOW); // send 0
	//     }
	// }
	
	Udp.beginPacket(computerIPAddress, REMOTEPORT);
	testMsg_toSend.send(Udp); // send the bytes
	Udp.endPacket();
	testMsg_toSend.empty(); // empty the message to free room for a new one
}

void sendOSCPins() {
	
	OSCMessage testMsg_toSend("/digitalpins");

	// from https://www.arduino.cc/en/Tutorial/BitMask
	for (int i=A0; i <= A7; i++) {
		// read this pin
		if(digitalRead(i) == HIGH) {
	      testMsg_toSend.add(1);
			  //digitalWrite(transmit,HIGH); // send 1
	    }
	    else{ //if bitwise and resolves to false
	      testMsg_toSend.add(0);
		  // digitalWrite(transmit,LOW); // send 0
	    }
	}
	
	// }
	// for(byte mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
	//     if (data & mask){ // if bitwise AND resolves to true
	//       testMsg_toSend.add(1);
	// 		  //digitalWrite(transmit,HIGH); // send 1
	//     }
	//     else{ //if bitwise and resolves to false
	//       testMsg_toSend.add(0);
	// 	  // digitalWrite(transmit,LOW); // send 0
	//     }
	// }
	
	Udp.beginPacket(computerIPAddress, REMOTEPORT);
	testMsg_toSend.send(Udp); // send the bytes
	Udp.endPacket();
	testMsg_toSend.empty(); // empty the message to free room for a new one
}

//=========================================================================================
void loop()
{
	bool bPinsChanged = false;
	
	// update sensors
	for(int i=0; i < NUM_INPUT_PINS; i++) {
		// store last value
		lastPinValues[i] = pinValues[i];
		
		// read new values
		pinValues[i] = digitalRead(digitalInPins[i]);
		
		// flag if changed
		if(pinValues[i] != lastPinValues[i])
			bPinsChanged=true;
		
		// if(i == 0 && pinValues[i] == HIGH) {
		// 	digitalWrite(ledPin, HIGH);
		// } else {
		// 	digitalWrite(ledPin, LOW);
		// }
		//
	}
	
	
	// sending messages
	if(bPinsChanged == true) {
		OSCMessage testMsg_outgoing;
		sendOSCPinsMsg(testMsg_outgoing);

		digitalWrite(ledPin, (pinValues[0]==LOW));
		// sendOSCPins();
	}

	// need a delay
	delay(1);
	
  //   float ap0 = analogRead(A0);
  //
  // // --- Send Message
  //
  // 	OSCMessage message("/A0/");
  // 	message.add(ap0);
  // 	sendMessage(message);
	

	// receiving messages
	if (manageMessages == 1 && manageBundles == 0) {

		//===== TEST : receiving and dispatching an OSC Message

		OSCMessage testMsg_Received;

		int bytesToRead = Udp.parsePacket();	// how many bytes are available via UDP
		if (bytesToRead > 0) {
			while(bytesToRead--) {
				testMsg_Received.fill(Udp.read());	// filling the OSCMessage with the incoming data
			}
			if(!testMsg_Received.hasError()) { // if the address corresponds to a command, we dispatch it to the corresponding function
				testMsg_Received.dispatch(OscCmd_led , setLEDStatus);
				testMsg_Received.dispatch(OscCmd_TestSendMsg , sendOSCTestMsg);
				testMsg_Received.dispatch(OscCmd_TestSendBndl , sendOSCTestBndl);
				testMsg_Received.dispatch(OscCmd_SwitchToBundles , switchToBundles);
				testMsg_Received.dispatch(OscCmd_ReadPins , sendOSCPinsMsg);
				// testMsg_Received.dispatch(OscCmd_ReadPins , sendOSCPinsBndl);
			}
			//	else {
			//		return;			// ERRORS : errors in the OSCMessage
			//	}
		}
	}

	else if (manageBundles == 1 && manageMessages == 0) {

		//===== TEST : receiving and dispatching an OSC Bundle (same code as with the OSCMessage, but with an OSCBundle)

		OSCBundle testBndl_Received;

		int bytesToRead = Udp.parsePacket();	// how many bytes are available via UDP
		if (bytesToRead > 0) {
			while(bytesToRead--) {
				testBndl_Received.fill(Udp.read());	// filling the OSCBundle with the incoming data
			}
			if(!testBndl_Received.hasError()) { // if the address corresponds to a command, we dispatch it to the corresponding function
				testBndl_Received.dispatch(OscCmd_led , setLEDStatus);
				testBndl_Received.dispatch(OscCmd_TestSendMsg , sendOSCTestMsg);
				testBndl_Received.dispatch(OscCmd_TestSendBndl , sendOSCTestBndl);
				testBndl_Received.dispatch(OscCmd_SwitchToMessages , switchToMessages);
			}
			//	else {
			//		return;			// ERRORS : errors in the OSCBundle
			//	}
		}
	}
}
