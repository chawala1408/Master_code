#include "MicMMS.h"

#define Version_coding "1.0.0"
/*------------------- Information Program -------------------*/
//  MicMMS version 1.0.0  (Version code)
/*----------------------------------------------------------*/

/*------------------- Data list -------------------*/
// MicMMS aaa(WiFi_name, Password, Mqtt_server, Mqtt_port,/Department/Process/,Machine_number, SlaveID, SerialPort,IP_Address,Gate_way,Subnet_mask,Version_coding);
/*-------------------------------------------------*/
MicMMS aaa("MIC_Iot", "Micdev@2024", "192.168.0.128", 1883, "/mic/test/", "a001", 1, Serial1, "192.168.0.100", "192.168.0.1", "255.255.255.0", "1.0.0");

void setup() {
  aaa.init();
  aaa.start();
}
  
void loop() {
  aaa.run();
}
