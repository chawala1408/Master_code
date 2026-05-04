#include "MicMMS.h"
/*------------------- Information Program -------------------*/
//  MicMMS version 2.0.3  (Version code)
/*----------------------------------------------------------*/

/*------------------- Data list -------------------*/
// MicMMS aaa("WiFi_name", "Password", "Mqtt_server", Mqtt_port,"/Department/Process/","Machine_number", 1, Serial1,"IP_Address","Gate_way","Subnet_mask","Version_coding");
/*-------------------------------------------------*/

MicMMS aaa("MIC-IIoT-DEVICE", "natmms22", "10.128.16.200", 1883,"/nht/gd/","ic45b", 1, Serial1,"10.128.58.155","10.128.58.1","255.255.254.0","2.0.3");

void setup() {
  aaa.init();
  aaa.start();
}
  
void loop() {
  aaa.run();
}
