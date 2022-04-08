// Libraries
#include <SoftwareSerial.h>
#include <Chrono.h>

//Pin 3 on Arduino receives, pin 2 transmits
#define RX 3
#define TX 2
#define BAUDRATE 9600

//Defines BLE object as BTSerial
SoftwareSerial BTSerial (RX,TX);

Chrono chrono;

int array_size = 5;
int reset_pin = 13;
byte phone_id_byte = 123;
byte collar_id_byte = 234;

//Struct to hold UIK packet.
struct Packet {
	byte a;
  	byte b;
  	byte c;
} pkt_tx, pkt_rx;

//
void setup() {
  	//Inititalize baud rates for both serial and HM-10
  	Serial.begin(BAUDRATE);
  	BTSerial.begin(BAUDRATE);

	int uik_array[array_size] = {};
	pinMode(reset_pin, OUTPUT);
  
  	//Initialize w/ AT commands to complete before phone pairs
    	//AT commands to query HM-10 device info for phone
    	//AT commands to set info (BARCCS name, power, etc.)
  	BTSerial.println("AT+BAUD0")
  	BTSerial.println("AT+POWE3")
  	BTSerial.println("AT+NAMEBARCCS")
  	BTSerial.println("AT+PIO11") //disable status LED blink
	BTSerial.println("AT+ADDR?")
  	string mac_addr = BTSerial.read()
}

//
void loop() {
  	//Begin constantly receiving
  	receive();
  
  	//Suitable delay
  	delay(50)
}

//Function to transmit UIK packet when after paired and called
void collarTransmit(int own_uik) {
  	//Transmit own UIK a few times to avoid faulty packet transmission
  	for(i=0;i<10;i++) {
    	BTSerial.write(own_uik);
    	delay(5) //small delay to avoid data collision
  	}
}

//
void phoneTransmit(string mac_addr, int uik_array) {
  	//Transmit own MAC Address to phone
  	BTSerial.write(mac_addr);

  	//Loop through list containing received collar UIK's
	for(i=0; i<5; i++) {
		int transmit_uik = uik_array[i];
		BTSerial.write(transmit_uik);
	}
  
  	//Delete all received UIK's
}

void receive() {
  	//Counter initialization  
  	static byte count = 10;

	digitalWrite(reset_pin, HIGH);
  
  	//Check to see if data can be read from buffer
    //Read data in size of UIK packet
  	if(BTSerial.available() >= sizeof(Packet)) {
    	BTSerial.readBytes((byte *) & pkt_rx,sizeof(Packet));

    	byte id_byte = pkt_rx.a;
    
    	//Check to see if data received is from phone
    	if(id_byte == phone_id_byte){
      		//Change packet tx struct to UIK sent from phone
      		pkt_tx.a = collar_id_byte;
      		pkt_tx.b = pkt_rx.b;
      		pkt_tx.c = pkt_rx.c;

      		int own_uik = concatenate(pkt_tx.a, pkt_tx.b, pkt_tx.c);
			uik_array = write_uik_array(own_uik);

      		phoneTransmit();
    	}
  
    	//Check to see if data is UIK of BARCCS device
    	if(id_byte == collar_id_byte){
      		//Begin timer
      		chrono.restart();

      		while (chrono elapsed() < 5000) {
        		//Initialize array of x length with own UIK. x determines
        		//how many uik's can be stored before phone reconnection
        		int uik_array[array_size] = {own_uik};
        		//Concatenate received packet
        		int found_uik = concatenate(pkt_rx.a, pkt_rx.b, pkt_rx.c);
        
        		//Append received packet to array of x length
        		for(int i=1; i<array_size; i++) {
          			bool uik_exists;
          			do {
            			uik_exists = false;
            			for(int j=1; j<i; j++) {
              				if(found_uik == uik_array[j]) {
                				uik_exists = true;
                				break;
              				}
            			}
          			} while(uik_exists);
          			uik_array[i] = found_uik;
          		}
        		//Flush serial buffer
        		while(BTSerial.available() > 0) {
          			BTSerial.read();
          		}
        		//Transmit UIK to connected Collar
        		collarTransmit(own_uik);
      		}
      		//Force disconnect with GPIO
			digitalWrite(reset_pin, LOW);
    	}
		//Try transmitting a few times for initiation
		collarTransmit(own_uik);
	}
	else{
		//Perform AT commands
	}
}

int write_uik_array(int uik) {
	bool phone_device = checkPrefix(uik, phone_id_byte);
	bool collar_device = checkPrefix(uik, collar_id_byte);

	if(phone_device) {
		
	}
	if(collar_device) {
		
	}
}

bool checkprefix(int uik, int device_id) {
    // Convert numbers into strings
    string s1 = to_string(uik);
    string s2 = to_string(device_id);
	
    // Find the lengths of strings
    int n1 = s1.length();
    int n2 = s2.length();
 
    // Base Case
    if (n1 < n2) {
        return false;
    }
 
    // Traverse the strings s1 & s2
    for (int i = 0; i < n2; i++) {
        // If at any index characters are unequal return false
        if (s1[i] != s2[i]) {
            return false;
        }
    }
    return true;
}

int concatenate(byte a, byte b, byte c) {
  String s1 = String(a);
  String s2 = String(b);
  String s3 = String(c);

  String uik = String(s1 + s2 + s3);

  int uik_i = uik.toint();
  
  return uik_i;
}

//For debugging
void printPacket() {
  //Print received packet to serial
}

//interrupt BT connection
void breakBT(){
    BTSerial.println("AT"); 
    //or, set GPIO connected to pin 20 of HM-10 through level shifter low
}

//Configure HM-10 for peripheral mode
void peripheralMode(){
    BTSerial.println("AT");
    BTSerial.println("AT+ROLE0");
}

//Configure HM-10 for central mode
void centralMode(){
    BTSerial.println("AT");
    BTSerial.println("AT+ROLE1");
    BTSerial.println("AT+IMME1"); //disable auto-pairing
    BTSerial.println("AT+RESET");
}

//Device discovery scan
void deviceDiscovery(){
    //return array of devices discovered?

    BTSerial.println("AT");
    BTSerial.println("AT+DISC?");
    //parse and store the response
    //see discord for the differences in output for DSD and HMSoft
}

//Connect to device from discovery
void connectDevice(int index){
    BTSerial.println("AT");
    //this is not properly coded prob will not compile
    BTSerial.println("AT+CONN"+toChar(index));
}

//track previous device connections and attempt connection to each device on
//discovery list until a collar is identified
void findCollar(){
    
}