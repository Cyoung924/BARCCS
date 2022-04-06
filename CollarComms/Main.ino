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

//Struct to hold UIK packet.
  //Change to 3 bytes. lead_id, uik, end_id?
struct Packet {
  int a;
  int b;
  int c;
  int d;
} pkt_tx, pkt_rx;

//
void setup() {
  //Inititalize baud rates for both serial and HM-10
  Serial.begin(BAUDRATE);
  BTSerial.begin(BAUDRATE);
  
  //Initialize w/ AT commands to complete before phone pairs
    //AT commands to query HM-10 device info for phone
    //AT commands to set info (BARCCS name, power, etc.)
  BTSerial.println("AT+BAUD0")
  BTSerial.println("AT+POWE3")
  BTSerial.println("AT+NAMEBARCCS")
  BTSerial.println("AT+ADDR?")
  mac_addr = Serial.read()
}

//
void loop() {
  //Begin constantly receiving
  receive();
  
  //Suitable delay
  delay(50)
}

//Function to transmit UIK packet when after paired and called
void collarTransmit() {
  //Update packet with UIK data assigned from phone

  //Write and transmit UIK
}

//
void phoneTransmit(int uik) {
  //Transmit own MAC Address to phone
  BTSerial.write(mac_addr);

  //Loop through list containing received collar UIK's
    //Write and transmit received UIK(s)
  
  
  //Delete all received UIK's
}

//Likely will need formula to avoid data collision
void receive() {
  //Counter initialization  
  static byte count = 10
  
  //Check to see if data can be read from buffer
    //Read data in size of UIK packet
  if(BTSerial.available() >= sizeof(Packet)) {
    BTSerial.readBytes((byte *) & pkt_rx,sizeof(Packet));

    int id_byte = pkt_rx.a;
    
    //Check to see if data received is from phone
    if(id_byte == phone_id_byte){
      //Change packet tx struct to UIK sent from phone
      pkt_tx.a = IDENTIFIER;
      pkt_tx.b = pkt_rx.b;
      pkt_tx.c = pkt_rx.c;
      pkt_tx.d = pkt_tx.d;

      int own_uik = concatenate(pkt_tx.a, pkt_tx.b, pkt_tx.c, pkt_tx.d);

      phoneTransmit();
    }
  }
  
    //Check to see if data is UIK of BARCCS device
      //Store UIK packet in device memory (as str in list) only if that UIK
      //is not already stored in list (list comprehension?)
    if(id_byte == collar_id_byte){
      //Begin timer
      chrono.restart()

      while (chrono elapsed() < 5000) {
        //Concatenate received packet
        //Initialize array of x length with own UIK
        //Add received packet to array of x length
        //Flush serial buffer

        //Transmit own UIK
      }
      //Force disconnect with GPIO
    }

  //Try transmitting a few times for initiation
}

int concatenate(int a, int b, int c, int d) {
  String s1 = String(a);
  String s2 = String(b);
  String s3 = String(c);
  String s4 = String(d);

  String uik = String(s1 + s2 + s3 + s4);

  int uik_i = uik.toint();
  
  return uik_i;
}

//For debugging
void printPacket() {
  //Print received packet to serial
}