// Custom one-way communication with Bluetooth HM-10
// Transmitting data in the form of a struct to receiver
//
// Writes (hardcoded) data to the software UART
//  and prints the transmitted packet


// Library to make a Software UART
#include <SoftwareSerial.h>

#define TX 2
#define RX 3

SoftwareSerial BTSerial(RX, TX); // (RX, TX)

#define BAUDRATE 9600

// Struct to hold the data we want to transmit
struct Packet {
  byte a;
  int b;
  float c;
  int d;
} pkt; // Instantiate a Packet struct

void setup() {
  // Start Serial Monitor for feedback
  Serial.begin(BAUDRATE);

  // HM-10 virtual UART
  BTSerial.begin(BAUDRATE);
}

void loop() {
  // Transmit data via bluetooth
  bluetooth_transmit();

  // Necessary forced delay, if we transmit too fast (no delay)
  //  the error rate increases
  delay(1);
}

// Function responsible for transmitting data over bluetooth
void bluetooth_transmit() {
  // Update data to be transmitted
  pkt.a = 0;
  pkt.b = 255;
  pkt.c = 888.888;
  pkt.d = -100;

  // Write packet data to the bluetooth - and transmit
  BTSerial.write((byte *) & pkt,sizeof(Packet));

  // Print packet (debug)
  print_packet();
}

// Function to print packet data (debug)
void print_packet() {
  Serial.print("TX: (a,b,c,d)=(");
  Serial.print(pkt.a); Serial.print(",");
  Serial.print(pkt.b); Serial.print(",");
  Serial.print(pkt.c); Serial.print(",");
  Serial.print(pkt.d);
  Serial.println(")");
}
