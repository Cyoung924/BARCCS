// Arduino code to send messages from smartphone to Arduino
// Can enter message as string via BLExAR and will echo the message if connection successful
#include <SoftwareSerial.h>

#define RX 3
#define TX 2

// Instantiation of a Software UART
SoftwareSerial ble_device(RX, TX); // (RX, TX)

String str_ii = "";
int ii_0 = 0;

void setup() {  
  ble_device.begin(9600);
  delay(100);

  // Enter AT+ commands of interest here (BLE Address, UUIDs, Power settings, may need to be revised for HM-10)
  ble_cmd("AT+NAMEBLExAR","Device Name: "); // printout device name
  ble_cmd("AT+ADDR?","Address: "); // printout BLE address
  ble_cmd("AT+CHAR?","Char UUID: "); // printout character UUID
  ble_cmd("AT+VERR?","Version: "); // module version
  ble_cmd("AT+RESET","Reset: "); // reset module
  str_ii = "";
  delay(500);
}

void loop() {
  while (ble_device.available()){
    char in_char = ble_device.read();
    if (int(in_char)!=-1 and int(in_char)!=42){
      str_ii+=in_char;
    }
    if (in_char=='\n'){
      delay(20);
      String msg = "Msg: ";
      msg+=str_ii;
      ble_device.print(msg);
      str_ii = "";
    }
  }
}

String ble_cmd(String cmd_str,String desc_str){
  str_ii = "";
  unsigned long t1 = millis();
  ble_device.println(cmd_str);
  while (true){
    if ((millis()-t1)>2000){ // 2 sec timeout
      return "Err";
    }
    char in_char = ble_device.read();
    if (int(in_char)==-1 or int(in_char)==42){
      continue;
    }
    if (in_char=='\n'){
      return str_ii;
    }
    str_ii+=in_char;
  }
}