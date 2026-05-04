#ifndef CONFIG_H
#define CONFIG_H
/*------------------- Information Program -------------------*/
//  MicMMS version 2.0.3  (Version code)
/*----------------------------------------------------------*/

#if CONFIG_IDF_TARGET_ESP32S2
  #define Pinled1 42
  #define Pinled2 41

#elif CONFIG_IDF_TARGET_ESP32S3
  #define Pinled1 1
  #define Pinled2 2

#else
  #define Pinled1 1
  #define Pinled2 2
#endif
#define rsRx 18                  // Pin for Serial RS232/RS485 UART Rx 18
#define rsTx 17                  // Pin for Serial RS232/RS485 UART Tx 17
#define SaveDisconnectTime 1000  // Time in ms for save disconnection

#if CONFIG_IDF_TARGET_ESP32
#define ESP32  1
#elif CONFIG_IDF_TARGET_ESP32S2
#define ESP32  2
#elif CONFIG_IDF_TARGET_ESP32S3
#define ESP32  3
#elif CONFIG_IDF_TARGET_ESP32C6
#define ESP32  4
#else
#define ESP32  5
#endif

/*--------- Topics to Publish MQTT Broker ---------*/
char* topic_pub_1 = "data";  //data/nht/gd/ic02r
char* topic_pub_2 = "status";
char* topic_pub_3 = "alarm";
char* topic_esp_health = "esp_health";
char* topic_broke_modbus = "mqtt";

/*--------- Timer config ---------*/
const uint16_t itr_modbus = 100;  // ms   0.1s
const uint16_t itr_fnc_1 = 1000;  // ms   1s
const uint16_t itr_fnc_2 = 1000;  // ms
const uint16_t itr_fnc_3 = 1000;  // ms
const uint16_t itr_esp = 20000;   // ms    20s
const uint16_t itr_network = 5000;   // ms  5s
const uint16_t itr_bro_mod = 5000;  // ms  5s

/*--------- Variable config ---------*/
const uint8_t num_got_data = 150;
uint16_t got_data[num_got_data];
float init_heap;
uint16_t bkr_connect, modb_check;
uint16_t query_check1, query_check2;
uint16_t query_temp1, query_temp2, tigger_1 = 1;
int total_data, Add_convert = 65536;
String prv_status, status;
String prv_alarm, alarm_;
String Id_ttl;
unsigned long long prv_time = 0;
unsigned long long prv_time_1 = 0;
unsigned long long prv_time_2 = 0;
uint16_t heap_cnt1, heap_cnt2, heap_cnt3;
float ct_fn1, ct_fn2, ct_fn3, ct_read;
uint16_t ct_read_cnt, ct_fn1_cnt, ct_fn2_cnt, ct_fn3_cnt;
/*--------- Number Time config CPU ---------*/
const uint16_t ct_read_ = 400;             //400 microsec
const unsigned int ct_fn1_ = 100000;      //100  ms 
const unsigned int ct_fn2_ = 100000;     //100 ms
const unsigned int ct_fn3_ = 100000;    //100 ms

String def_tb[][5] = {
  // name||address||type||value||prv_value
  // type for separate detail of data
  { "mc_run", "1", "1", "", "" },                     //mc_run
  { "mc_stop", "2", "1", "", "" },                    //mc_stop
  { "mc_alarm", "3", "1", "", "" },                   //mc_alarm
  { "mc_waitpart", "4", "1", "", "" },                //mc_waitpart
  { "mc_fullWork", "5", "1", "", "" },                //mc_fullWork
  { "H-1 LOADING ERROR", "21", "2", "", "" },         //Data alarm list
  { "H-1 ARM FWD ERROR", "22", "2", "", "" },
  { "H-1 TOOL ENGAGE IN ERROR", "23", "2", "", "" },
  { "H-1 TOOL ENGAGE OUT ERROR", "24", "2", "", "" },
  { "H-1 ARM REV ERROR", "25", "2", "", "" },
  { "TR-1 LOADER ERROR", "26", "2", "", "" },
  { "TR-1 STEP CHECK ERROR", "27", "2", "", "" },
  { "TR-1 PUSHER ERROR", "28", "2", "", "" },
  { "H-1 ARM CENTER ERROR", "29", "2", "", "" },
  { "TR-1 NO PART", "30", "2", "", "" },
  { "H-1 FLOATLESS SW ERROR", "31", "2", "", "" },
  { "H-2 LOADING ERROR", "32", "2", "", "" },
  { "H-2 ARM FWD ERROR", "33", "2", "", "" },
  { "H-2 TOOL ENGAGE IN ERROR", "34", "2", "", "" },
  { "H-2 TOOL ENGAGE OUT ERROR", "35", "2", "", "" },
  { "H-2 ARM REV ERROR", "36", "2", "", "" },
  { "TR-2 LOADER ERROR", "37", "2", "", "" },
  { "TR-2 STEP CHECK ERROR", "38", "2", "", "" },
  { "TR-2 PUSHER ERROR", "39", "2", "", "" },
  { "H-2 ARM CENTER ERROR", "40", "2", "", "" },
  { "H-2 FLOATLESS SW ERROR", "41", "2", "", "" },
  { "H-1 NO PART", "42", "2", "", "" },
  { "H-2 NO PART", "43", "2", "", "" },
  { "H-1 FULL PART", "44", "2", "", "" },
  { "H-2 FULL PART", "45", "2", "", "" },
  { "SIDE DRESS FOR ERROR", "46", "2", "", "" },
  { "SIDE DRESS REV ERROR", "47", "2", "", "" },
  { "GRINDER GAUGE ERROR", "48", "2", "", "" },
  { "WORN WHEEL", "49", "2", "", "" },
  { "LOADING ERROR", "50", "2", "", "" },
  { "DOOR OPEN", "51", "2", "", "" },
  { "FULL WORK", "52", "2", "", "" },
  { "CHUTE EMPTY", "53", "2", "", "" },
  { "TRANFER LOADER ERROR", "54", "2", "", "" },
  { "AFTER DRESS STOP", "55", "2", "", "" },
  { "SPINOUT", "56", "2", "", "" },
  { "G.WHEEL MOTOR OVER LOAD", "57", "2", "", "" },
  { "SPARKOUT HOLD MODE", "58", "2", "", "" },
  { "RADIAL DRESS ERROR", "59", "2", "", "" },
  { "NEXT M/C CHUTE FULL", "60", "2", "", "" },
  { "LINE UP PUSHER ERROR", "61", "2", "", "" },
  { "DOOR OPEN STOP", "62", "2", "", "" },
  { "TRANFER LOADER NO WORK", "63", "2", "", "" },
  { "WORN WHEEL NOTICE", "64", "2", "", "" },
  { "SERVO AMP BATTERY ALARM", "65", "2", "", "" },
  { "GE-ON", "66", "2", "", "" },
  { "GE NOT READY", "67", "2", "", "" },
  { "R/W BIG", "68", "2", "", "" },
  { "G.W TABLE ORIGIN REQUEST", "69", "2", "", "" },
  { "PICK UP ORIGIN REQUEST", "70", "2", "", "" },
  { "LINE UP ORIGIN REQUEST", "71", "2", "", "" },
  { "GE NOT ON", "72", "2", "", "" },
  { "MAGENT CHUCK ON/OFF", "73", "2", "", "" },
  { "ROTARY DRESSER RUN ERROR", "74", "2", "", "" },
  { "DRESSER ERROR", "75", "2", "", "" },
  { "I.D SMALL", "76", "2", "", "" },
  { "I.D LARGE", "77", "2", "", "" },
  { "GRINDER FULL WORK", "78", "2", "", "" },
  { "GRINDER CHUTE EMPTY", "79", "2", "", "" },
  { "A/F ADJ. YIELD STOP", "80", "2", "", "" },
  { "SORTING FULL WORK COUNTER", "81", "2", "", "" },
  { "SORTING NO WORK", "82", "2", "", "" },
  { "REPEAT COUNTER", "83", "2", "", "" },
  { "TRANSFER LOADER ERROR", "84", "2", "", "" },
  { "ID SMALL(GE)", "85", "2", "", "" },
  { "GE CRUSH", "86", "2", "", "" },
  { "DPM. ERROR", "87", "2", "", "" },
  { "TOTAL TAPER ADJ.LIMIT ERROR", "88", "2", "", "" },
  { "GAUGE ERROR (NO SIGNAL)", "89", "2", "", "" },
  { "OK1 TRAP SHUTTER ERROR", "90", "2", "", "" },
  { "OK2 TRAP SHUTTER ERROR", "91", "2", "", "" },
  { "-NG TRAP SHUTTER ERROR", "92", "2", "", "" },
  { "+NG TRAP SHUTTER ERROR", "93", "2", "", "" },
  { "SORTING NO WORK STOP", "94", "2", "", "" },
  { "GRINDING CYCLE TIME OVER", "95", "2", "", "" },
  { "RESET BY LOADING", "96", "2", "", "" },
  { "DAY START", "97", "2", "", "" },
  { "id_num", "110", "5", "", "" },                     //String ID_num
  { "id_num", "111", "5", "", "" },
  { "id_num", "112", "5", "", "" },
  { "id_num", "113", "5", "", "" },
  { "id_num", "114", "5", "", "" },
  { "avgct", "115", "3", "", "" },         //Production data
  { "eachct", "116", "3", "", "" },
  { "yieldrt", "117", "3", "", "" },
  { "idl", "118", "3", "", "" },
  { "ng_p", "119", "4", "", "" },
  { "ng_p1", "120", "4", "", "" },
  { "ng_n", "121", "4", "", "" },
  { "ng_n1", "122", "4", "", "" },
  { "tng", "123", "4", "", "" },
  { "tng1", "124", "4", "", "" },
  { "prod_total", "125", "4", "", "" },
  { "prod_total1", "126", "4", "", "" },
  { "utilization", "127", "3", "", "" },
  { "utl_total", "128", "3", "", "" },
  { "prod_s1", "129", "4", "", "" },
  { "prod_s11", "130", "4", "", "" },
  { "prod_s2", "131", "4", "", "" },
  { "prod_s21", "132", "4", "", "" },
  { "prod_s3", "133", "4", "", "" },
  { "prod_s31", "134", "4", "", "" },
  { "cth1", "135", "3", "", "" },
  { "cth2", "136", "3", "", "" },
  { "idh1", "137", "3", "", "" },
  { "idh2", "138", "3", "", "" },

};

#endif
