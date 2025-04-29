#ifndef CONFIG_H
#define CONFIG_H
/*------------------- Information Program -------------------*/
//  MicMMS version 1.0.0  (Version code)
/*----------------------------------------------------------*/

#define Pinled1 1  // LED for Detected the Publish data
#define Pinled2 2  // LED for Connection Internet
#define SaveDisconnectTime 1000 // Time im ms for save disconnection

/*--------- Topics to Publish MQTT Broker ---------*/
char* topic_pub_1 = "data";  //data/mic/test/a001
char* topic_pub_2 = "status";
char* topic_pub_3 = "alarm";
char* topic_esp_health = "esp_health";
char* topic_broke_modbus = "mqtt";
//  char* topic_sub_1 = "sub_1";

/*--------- Timer config ---------*/
const uint16_t itr_modbus = 100;    // ms   0.1s
const uint16_t itr_fnc_1 = 1000;    // ms   1s
const uint16_t itr_fnc_2 = 1000;    // ms
const uint16_t itr_fnc_3 = 1000;    // ms
const uint16_t itr_esp = 20000;     // ms  20s
const uint16_t itr_network = 5000;  // ms  5s
const uint16_t itr_bro_mod = 5000;  // ms

const uint8_t num_got_data = 180;
uint16_t got_data[num_got_data];
float init_heap;
uint16_t bkr_connect, modb_check;
uint16_t query_check1, query_check2;
uint16_t query_temp1, query_temp2, tigger_1 = 1;
int total_data, Add_convert = 65536;
String prv_status, status;
String prv_alarm, alarm_;
String Lot_ttl;
unsigned long long prv_time = 0;
unsigned long long prv_time_1 = 0;
unsigned long long prv_time_2 = 0;
uint16_t heap_cnt1, heap_cnt2, heap_cnt3;
float ct_fn1, ct_fn2, ct_fn3, ct_read;
uint16_t ct_read_cnt, ct_fn1_cnt, ct_fn2_cnt, ct_fn3_cnt;
/*--------- Number Time config CPU ---------*/
const uint16_t ct_read_ = 400;        //400 microsec
const unsigned int ct_fn1_ = 100000;  //100  ms
const unsigned int ct_fn2_ = 100000;  //100 ms
const unsigned int ct_fn3_ = 100000;  //100 ms

String def_tb[][5] = { 
{ "mc_setup", "30", "1", "", "" }, 
{ "mc_adjust", "31", "1", "", "" }, 
{ "mc_chTool", "32", "1", "", "" }, 
{ "mc_warm", "33", "1", "", "" }, 
{ "mc_other", "34", "1", "", "" }, 
{ "mc_stop", "35", "1", "", "" }, 
{ "mc_run", "36", "1", "", "" }, 
{ "mc_alarm", "37", "1", "", "" }, 
{ "avgct", "115", "3", "", "" }, 
{ "eachct", "116", "3", "", "" }, 
{ "yieldrt", "117", "3", "", "" }, 
{ "idl", "118", "3", "", "" }, 
{ "ng_p", "119", "3", "", "" }, 
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
{ "lot1", "139", "5", "", "" }, 
{ "lot2", "140", "5", "", "" }, 
{ "lot3", "141", "5", "", "" }, 
{ "lot4", "142", "5", "", "" }, 
{ "lot5", "143", "5", "", "" }
 };

#endif