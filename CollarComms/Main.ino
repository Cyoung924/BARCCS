// Libraries
#include <SoftwareSerial.h>

//Struct to hold UIK packet

//
void setup() {
  //Inititalize baud rates for both serial and HM-10
  
  //Initialize w/ AT commands to complete before phone pairs
    //AT commands to query HM-10 device info for phone
    //AT commands to set info (BARCCS name, power, etc.)
}

//
void loop() {
  //Begin constantly receiving

  //Suitable delay
}

//Function to transmit UIK packet when after paired and called
void transmit() {
  //Update packet with UIK data assigned from phone

  //Write and transmit UIK
}

//
void receive() {
  //Check to see if data can be read
    //Read data in size of UIK packet
    //Check to see if data received is from phone
      //Store in device memory as variable to pass to transmit()
      //so that UIK can be generated
    //Check to see if data is UIK of BARCCS device
      //If so, begin x sec timer before forced disconnect
      //Store UIK packet in device memory (as str in list)

    //Flush serial buffer

    //Transmit own UIK

    //Check timer
      //Force disconnect

  //Try transmitting a few times
}

//For debugging
void printPacket() {
  //Print packet to serial
}