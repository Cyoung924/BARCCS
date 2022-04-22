// Libraries
#include <SoftwareSerial.h>

//Pin 3 on Arduino receives, pin 2 transmits
#define RX 3
#define TX 2
#define BAUDRATE 9600
#define ARRAYSIZE 5

//Defines BLE object as BTSerial
SoftwareSerial BTSerial (RX,TX);

int reset_pin = 13;
byte phone_id_byte = 98;
byte collar_id_byte = 4;
int uik_array[ARRAYSIZE] = {};
int at_delay = 1000;
unsigned long milli_delay = 5000;

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
  while (!Serial) {
    ;
  }
  BTSerial.begin(BAUDRATE);

  pinMode(reset_pin, OUTPUT);

  //Initialize w/ AT commands to complete before phone pairs
  BTSerial.print("AT+BAUD0"); delay(at_delay); //set HM-10 baud rate to 9600
  BTSerial.print("AT+POWE3"); delay(at_delay); //max out transmission power
  BTSerial.print("AT+NAMEBARCCS"); delay(at_delay); //set name to predetermined
  BTSerial.print("AT+SHOW1"); delay(at_delay); //default to show name for discovery
  BTSerial.print("AT+ROLE0"); delay(at_delay); //set peripheral so primed for smartphone
  //BTSerial.println("AT+PIO11"); delay(10); //disable status LED blink

  String response = atCommandResponse("AT+ADDR?", milli_delay);
  String own_mac_addr = response.substring(8, 20);

}

//
void loop() {
  //Discover MAC addresses
  String mac_addr_str = deviceDiscovery();

  //initiate connection
  int addr_length = mac_addr_str.length();
  if (addr_length > 4){
    Serial.println("Connecting...");
    deviceConnect(mac_addr_str);
  }

  peripheralMode();
  while(BTSerial.available() > 0) {
    BTSerial.read();
  }
  short int random_delay = random(100, 3000);
  delay(random_delay);

  receive();
  centralMode();
}

//Function to transmit UIK packet when after paired and called
void collarTransmit(unsigned int own_uik) {
//  int own_uik = array[0];
  String collar_s = String(collar_id_byte);
  int collar_i = collar_s.toInt();
  bool collar_test = checkPrefix(own_uik, collar_i);

//  Serial.print("Packet transmitted to collar: (");
//  Serial.print(pkt_rx.a); Serial.print(",");
//  Serial.print(pkt_rx.b); Serial.print(",");
//  Serial.print(pkt_rx.c); Serial.println(")");
  
  //Transmit own UIK a few times to avoid faulty packet transmission
  if (collar_test) {
    for(int i=0; i<10; i++) {
      BTSerial.write(own_uik);
      Serial.print("UIK transmitted to collar: ");
      Serial.println(own_uik);
      delay(10); //small delay to avoid data collision
    }
  }
}

//
void phoneTransmit(int array[], int size) {
  //Loop through list containing received collar UIK's, writes own UIK first
  for(int i=0; i<size; i++) {
    int transmit_uik = array[i];
    //Transmits in bytes, phone will need to translate
    BTSerial.write(transmit_uik);
  }
  //Delete all received UIK's (fills the array with 0's)
  for(int j=0; j<size; j++) {
    array[j] = 0;
  }
}

void receive() {
  //Counter initialization
  static byte count = 10;

  digitalWrite(reset_pin, HIGH);

  //Check to see if data can be read from buffer
  if(BTSerial.available() >= sizeof(Packet)) {
    //Read data in size of UIK packet
    BTSerial.readBytes((byte *) & pkt_rx,sizeof(Packet));

    //Print received packet
    Serial.print("Received packet: (");
    Serial.print(pkt_rx.a); Serial.print(",");
    Serial.print(pkt_rx.b); Serial.print(",");
    Serial.print(pkt_rx.c); Serial.println(")");

    byte id_byte = pkt_rx.a;

    //Set own UIK for TX w/o phone
    pkt_tx.a = collar_id_byte;
    pkt_tx.b = 6;
    pkt_tx.c = 9;

    unsigned int own_uik = concatenate(pkt_tx.a, pkt_tx.b, pkt_tx.c);

    //Check to see if data received is from phone
    if(id_byte == phone_id_byte){
      //Change packet tx struct to UIK sent from phone
      pkt_tx.a = collar_id_byte;
      pkt_tx.b = pkt_rx.b;
      pkt_tx.c = pkt_rx.c;

      Serial.print("Packet to transmit: (");
      Serial.print(pkt_tx.a); Serial.print(",");
      Serial.print(pkt_tx.b); Serial.print(",");
      Serial.print(pkt_tx.c); Serial.println(")");

      //Get rid of and make static in implementation demo
      int own_uik = concatenate(pkt_tx.a, pkt_tx.b, pkt_tx.c);

      //make function void, and just modify existing array
      modify_uik_array(uik_array, own_uik, 0);

      //Transmit modified uik array
      phoneTransmit(uik_array, ARRAYSIZE);

      //Reinitialize array with own uik after phoneTransmit() clears
      modify_uik_array(uik_array, own_uik, 0);
    }

    //Check to see if data is UIK of BARCCS collar device
    if(id_byte == collar_id_byte){
      //Begin timer
      unsigned long start_time = millis();

      while (millis() - start_time < 5000) {
        Serial.print("Packet received from collar: (");
        Serial.print(pkt_rx.a); Serial.print(",");
        Serial.print(pkt_rx.b); Serial.print(",");
        Serial.print(pkt_rx.c); Serial.println(")");

        //Concatenate received packet
        unsigned int found_uik = concatenate(pkt_rx.a, pkt_rx.b, pkt_rx.c);

        //Append received packet to array of x length, skipping
        //over the 0th position to not overwrite own_uik
        for(int i=1; i<ARRAYSIZE; i++) {
          bool uik_exists;
//          do {
//            uik_exists = false;
//            for(int j=1; j<i; j++) {
//              if(found_uik == uik_array[j]) {
//                uik_exists = true;
//                break;
//              }
//            }
//          } while(uik_exists);
          modify_uik_array(uik_array, found_uik, i);
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
      BTSerial.print("AT"); delay(at_delay);
    }
    //Try transmitting a few times for initiation
    collarTransmit(uik_array);
  }
  //If data is unavailable
  else {
    //Move on to next MAC Address
    BTSerial.print("AT"); delay(at_delay);
    digitalWrite(reset_pin, LOW);
    return;
  }
}

void modify_uik_array(int array[], int uik, int position) {
  bool phone_device = checkPrefix(uik, phone_id_byte);
  bool collar_device = checkPrefix(uik, collar_id_byte);

  if(phone_device) {
    array[0] = uik;
  }
  if(collar_device) {
    array[position] = uik;
  }
}

bool checkPrefix(int uik, int device_id) {
    // Convert numbers into strings
    String s1 = String(uik);
    String s2 = String(device_id);

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

unsigned int concatenate(byte a, byte b, byte c) {
  String s1 = String(a);
  String s2 = String(b);
  String s3 = String(c);

  String uik = String(s1 + s2 + s3);

  unsigned int uik_i = uik.toInt();

  return uik_i;
}

//Configure HM-10 for peripheral mode
void peripheralMode(){
    Serial.println("Switching to Peripheral Mode...");
    BTSerial.print("AT"); delay(at_delay);
    BTSerial.print("AT+ROLE0"); delay(at_delay);
}

//Configure HM-10 for central mode
void centralMode(){
    Serial.println("Switching to Central Mode...");
    BTSerial.print("AT"); delay(at_delay);
    //add delays between AT calls and other configs?
    BTSerial.print("AT+ROLE1"); delay(at_delay);
    BTSerial.print("AT+IMME1"); delay(at_delay);
    BTSerial.print("AT+RESET"); delay(at_delay);
}

//Device discovery scan
String deviceDiscovery(){
  String mac_addr_str = "MAC:";
  //Role-switching
  String role_response = atCommandResponse("AT+ROLE?", milli_delay);
  int role = role_response[7];
  //Discover can only happen in central mode
  if (role == 49) {
    //Discover devices and store MAC addresses
    String disc_response = atCommandResponse("AT+DISC?", milli_delay);
    //parse and store the response
    String name_str = "OK+NAME:BARCCS";
    int first_index = disc_response.indexOf(name_str);
    //If there is at least one BARCCS device in the discovery
    if (first_index != -1){
      //For as many times as there are BARCCS devices in the discovery
      for (int search_index = first_index; search_index != 0; search_index++) {
        int name_index = disc_response.indexOf(name_str, search_index);
        int mac_addr_index = name_index - 12;
        String mac_addr = disc_response.substring(mac_addr_index, name_index);
        //Append mac_addr to delimitted string
        mac_addr_str += mac_addr;
        mac_addr_str += ",";

        search_index = name_index;
      }
    }
    //If no BARCCS devices in discovery, move on w/ empty mac_addr_str
  }
  /*If in peripheral, hold in peripheral for some time to create window for a
  connection. Then attempt to receive data before switching modes to enable disc */
  /* else{
    short int random_delay = random(100, 3000);
    delay(random_delay);
    receive();
    centralMode();
  } */
  return mac_addr_str;
}

//Connect to device from discovery
void deviceConnect(String mac_addr_str){
  int mac_addr_count = ((mac_addr_str.length() - 4) % 13);
  for(int i = 1; i < (mac_addr_count + 1); i++){

    int search_index = 4 + (i * 13);
    String mac_addr = mac_addr_str.substring((search_index - 13), (search_index - 1));
    //String mac_addr = "0";
    String con_comm = "AT+CON" + mac_addr;
    const char * con_comm_ch = con_comm.c_str();

    Serial.print("Trying to connect to MAC address: ");
    Serial.println(mac_addr);

    //BTSerial.print("AT"); delay(at_delay);
    //BTSerial.print(con_comm); delay(10000);
    delay(10000);
    Serial.println("Connected.");
    String conn_response = atCommandResponse(con_comm_ch, milli_delay);
    Serial.println();
    receive();
  }
}

String atCommandResponse(const char *toSend, unsigned long milliseconds) {
  unsigned long startTime = millis();

  while(BTSerial.available() > 0) {
          BTSerial.read();
  }

  String result;
  Serial.print("Sending: ");
  Serial.println(toSend);
  BTSerial.print(toSend); delay(at_delay);

  Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    if (BTSerial.available()) {
      char c = BTSerial.read();
      Serial.write(c);
      result += c;  // append to the result string
      if ('\n' == c)
        break;
    }
  }
Serial.println();  // new line after timeout.
return result;
}