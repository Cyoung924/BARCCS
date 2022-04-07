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

      int own_uik = concatenate(pkt_tx.a, pkt_tx.b, pkt_tx.c);

      phoneTransmit();
    }
  }
  
    //Check to see if data is UIK of BARCCS device
    if(id_byte == collar_id_byte){
      //Begin timer
      chrono.restart()

      while (chrono elapsed() < 5000) {
        //Initialize array of x length with own UIK. x determines how many
        //uik's can be stored before phone reconnection
        int array_size = 5;
        int uik_array[array_size] = {own_uik}
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
        //Transmit UIK's
        collarTransmit(uik_array);
      }
      //Force disconnect with GPIO
    }

  //Try transmitting a few times for initiation
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