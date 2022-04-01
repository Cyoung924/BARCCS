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
void collarTransmit() {
  //Update packet with UIK data assigned from phone

  //Write and transmit UIK
}

//
void phoneTransmit() {
  //Loop through list containing received collar UIK's
    //Write and transmit received UIK(s)

  //Delete all received UIK's
}

//Likely will need formula to avoid data collision
void receive() {
  //Check to see if data can be read
    //Read data in size of UIK packet
  
    //Check to see if data received is from phone
      //Store packet device memory as variable to pass to collarTransmit()
      //so that UIK can be generated
      //Call phoneTransmit()
  
    //Check to see if data is UIK of BARCCS device
      //If so, begin x sec timer before forced disconnect
      //Store UIK packet in device memory (as str in list) only if that UIK
      //is not already stored in list (list comprehension?)

    //Flush serial buffer

    //Transmit own UIK

    //Check timer
      //Force disconnect with GPIO

  //Try transmitting a few times for initiation
}

//For debugging
void printPacket() {
  //Print received packet to serial
}