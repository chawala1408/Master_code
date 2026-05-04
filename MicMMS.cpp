/*------------------- Information Program -------------------*/
//  MicMMS version 2.0.3
/*----------------------------------------------------------*/

#include "MicMMS.h"
#include "conf.h"
#include "esp_system.h"

MicMMS::MicMMS(const char* ssid, const char* password, const char* mqtt_server, int mqtt_port, const char* dp_name, const char* mac_no, const char* ip_address, const char* gateway_address, const char* subnet_mask, const char* ip_address1, const char* vrs_code)
  : wifiClient(), mqttClient(wifiClient), ssid(ssid), password(password), mqtt_server(mqtt_server), mqtt_port(mqtt_port), dp_name(dp_name), mac_no(mac_no), vrs_code(vrs_code) {
  ip.fromString(ip_address);  // Fix IP config for IIoT Box
  gateway.fromString(gateway_address);
  subnet.fromString(subnet_mask);
  ip1.fromString(ip_address1);  // IP for Rx Data from GOT
}

void MicMMS::setupWiFi() {
  int MinRSSI = -85;
  int bestNetworkIndex = -1;
  unsigned long startAttemptTime = millis();

  WiFi.disconnect(true);  // delete old config
  WiFi.mode(WIFI_OFF);
  delay(SaveDisconnectTime);  // 1000ms seems to work in most cases, may depend on AP
  WiFi.mode(WIFI_STA);

  Serial.println("Scanning for WiFi networks...");
  int n = WiFi.scanNetworks();  // WiFi.scanNetworks will return the number of networks found
  if (n == 0) {
    Serial.println("no networks found");
    return;
  }
  // } else {
  //   Serial.printf("%d networks found:\n", n);
  //   for (int i = 0; i < n; ++i) {
  //     // Print SSID and RSSI for each network found
  //     Serial.printf("%d: %s, Signal: %d dBm, BSSID: %s, Channel: %d\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.BSSIDstr(i).c_str(), WiFi.channel(i));
  //   }
  // }
  // Find the network with the best RSSI value
  for (int j = 0; j < n; ++j) {
    if (WiFi.SSID(j) == ssid) {
      int rssi = WiFi.RSSI(j);
      if (rssi > MinRSSI) {
        MinRSSI = rssi;
        bestNetworkIndex = j;
      }
    }
  }
  // Connect to the network with the best RSSI value
  if (bestNetworkIndex != -1) {
    Serial.printf("Best AP Connection:%s, Signal: %d dBm, BSSID: %s, Channel: %d\n", WiFi.SSID(bestNetworkIndex).c_str(), WiFi.RSSI(bestNetworkIndex), WiFi.BSSIDstr(bestNetworkIndex).c_str(), WiFi.channel(bestNetworkIndex));
    // Connect to the selected AP
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password, 0, WiFi.BSSID(bestNetworkIndex));

    while (WiFi.status() != WL_CONNECTED) {
      // printf("WiFi status is %d\n", WiFi.status());
      Serial.println("Connecting WiFi Fail,Restarting...");
      digitalWrite(Pinled2, HIGH);
      delay(100);
      digitalWrite(Pinled2, LOW);
      delay(1000);
      if ((millis() - startAttemptTime) >= 15000) {  // Check WiFi.status() 15s
        WiFi.reconnect();
        WiFi.begin(ssid, password, 0, WiFi.BSSID(bestNetworkIndex));
        startAttemptTime = millis();
      }
    }
    if ((WiFi.status() == WL_CONNECTED)) {
      Serial.println("Connected to WiFi Completed");
      digitalWrite(Pinled2, HIGH);
    }
  } else {
    digitalWrite(Pinled2, HIGH);
    delay(100);
    digitalWrite(Pinled2, LOW);
    delay(500);
    Serial.println("No known networks found");
  }
}

// callback fucn when read or write data modbus
uint16_t MicMMS::onModbusRequest(TRegister* reg, uint16_t val) {
  lastGotSignalTime = millis();  // Last time update for get signal from GOT
  check = 1;
  return val;  // return value to Register
}

void MicMMS::reconnect() {
  if (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = "ESP32Client_";
    clientId += mac_no;
    clientId += String(random(0xffff), HEX);  // ESP32Client_mc_no0xa12f the "0xa12f" is random number form "HEX"
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT Broker");
      digitalWrite(Pinled1, LOW);  // Broker connected!!
    } else {
      printf("Failed with state %d\n", mqttClient.state());
      if (mqttClient.state() == -2) {
        digitalWrite(Pinled1, HIGH);  // Broker don't connection
      }
      delay(1000);
    }
  }
}

void MicMMS::init() {
  std::vector<std::vector<String>> def_tb;
  pinMode(Pinled1, OUTPUT);  //Publish pin
  pinMode(Pinled2, OUTPUT);  //Connect Internet pin

  Serial.begin(115200);
  Ethernet.begin(mac, ip1);
  if (ESP32 == 2) {
    /* ------ "Running on ESP32-S2" ------*/
    Ethernet.init(34);  //Slave select (SS) Pin
  } else if (ESP32 == 3) {
    /* ------ "Running on ESP32-S3" ------*/
    Ethernet.init(10);  //Slave select (SS) Pin
  }
  modbus.server();
  server.begin();
  setupWiFi();
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("IP address IoT Box: ");
  Serial.println(WiFi.localIP());
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(1024);   // Config the size, in bytes, of the internal send/receive buffer
  mqttClient.setKeepAlive(30);      // Config Keep-alive 30s
  mqttClient.setSocketTimeout(10);  // Config Socket timeout 10s

  digitalWrite(Pinled2, HIGH);
  init_heap = esp_get_free_heap_size();
  for (int i = 0; i < num_got_data; i++) {
    modbus.addReg(HREG(i));
    // Add callback for Read and Write Modbus Register at Address 1
    // modbus.onGet(HREG(i), onModbusRequest);  // Detect when GOT reads data
    // Use Lambda function instead of calling a non-static function
    modbus.onSet(HREG(i), [this](TRegister* reg, uint16_t val) -> uint16_t {
      return this->onModbusRequest(reg, val);
    });  // Detect when GOT write data
  }
}

bool MicMMS::publishMessage(char* topic, const char* message) {
  if (mqttClient.publish(topic, message)) {
    digitalWrite(Pinled1, HIGH);
    delay(100);
    digitalWrite(Pinled1, LOW);
    return true;
  } else {
    return false;
  }
}

void MicMMS::run() {
  // Check the client's connection to the server.
  EthernetClient client = server.available();
  modbus.task();
  /*-------------- record raw data to table --------------*/
  unsigned long long int start = micros();
  for (int i = 0; i < num_got_data; i++) {
    got_data[i] = modbus.Reg(HREG(i));
  }
  for (int j = 0; j < sizeof(def_tb) / sizeof(def_tb[0]); j++) {
    def_tb[j][3] = got_data[(def_tb[j][1].toInt()) - 1];
  }
  // for (int k = 109; k < 155; k++) {
  //   Serial.print(modbus.Reg(HREG(k)));
  //   Serial.print(got_data[k]);
  //   Serial.print(": ");
  // }
  // Serial.println();
  // Check the signal is received from the GOT within 3 seconds.
  if (millis() - lastGotSignalTime > GOT_TIMEOUT) {
    modb_check = 0;
    check = 0;  // No signal from GOT for 3 seconds
    // printf("No signal from GOT. check = %d\n", check);
  } else {
    // printf("Signal from GOT detected. check = %d\n", check);
    modb_check = 1;
  }
  ct_read = micros() - start;
  //interval work loop 2.2-2.5 ms
}

void MicMMS::start() {
  if (ESP32 == 2) {
    Serial.println("Running on ESP32-S2");
    // xTaskCreatePinnedToCore(modbus_Task, "Task0", 10000, this, 7, NULL, 0);
    xTaskCreatePinnedToCore(Network_Task, "Task1", 10000, this, 6, NULL, 0);
    xTaskCreatePinnedToCore(func1_Task, "Task2", 10000, this, 5, NULL, 0);
    xTaskCreatePinnedToCore(func2_Task, "Task3", 10000, this, 4, NULL, 0);
    xTaskCreatePinnedToCore(func3_Task, "Task4", 10000, this, 3, NULL, 0);
    xTaskCreatePinnedToCore(broke_modbus_Task, "Task5", 10000, this, 2, NULL, 0);
    xTaskCreatePinnedToCore(esp_Task, "Task6", 10000, this, 1, NULL, 0);
  } else if (ESP32 == 3) {
    Serial.println("Running on ESP32-S3");
    // xTaskCreatePinnedToCore(modbus_Task, "Task0", 10000, this, 7, NULL, 0);
    xTaskCreatePinnedToCore(Network_Task, "Task1", 10000, this, 6, NULL, 1);
    xTaskCreatePinnedToCore(func1_Task, "Task2", 10000, this, 5, NULL, 0);
    xTaskCreatePinnedToCore(func2_Task, "Task3", 10000, this, 4, NULL, 0);
    xTaskCreatePinnedToCore(func3_Task, "Task4", 10000, this, 3, NULL, 0);
    xTaskCreatePinnedToCore(broke_modbus_Task, "Task5", 10000, this, 2, NULL, 0);
    xTaskCreatePinnedToCore(esp_Task, "Task6", 10000, this, 1, NULL, 0);
    /*} else if (ESP32 == 4) {
    Serial.println("Running on ESP32-C6");*/
  } else {
    Serial.println("Unknown ESP32 variant");
  }
}

void MicMMS::Network_Task(void* pvParam) {
  MicMMS* instance = (MicMMS*)pvParam;

  while (1) {
    /*-------- Check Mqtt Client alive --------*/
    instance->mqttClient.loop();
    /*-------- Check Internet & Server MQTT --------*/
    if ((WiFi.status() != WL_CONNECTED)) {
      digitalWrite(Pinled2, HIGH);
      delay(100);
      digitalWrite(Pinled2, LOW);
      instance->setupWiFi();
    }
    if (!(instance->mqttClient.connected())) {
      instance->reconnect();
    }
    vTaskDelay(pdMS_TO_TICKS(itr_network));  //loop get value every 5 sec
  }
}

void MicMMS::func1_Task(void* pvParam) {
  MicMMS* instance = (MicMMS*)pvParam;
  MicMMS* dpName = (MicMMS*)(pvParam);
  MicMMS* macNo = (MicMMS*)(pvParam);

  char topic_pub[30];
  strcpy(topic_pub, topic_pub_1);
  strcat(topic_pub, dpName->dp_name);
  strcat(topic_pub, macNo->mac_no);

  while (1) {
    unsigned long long int start = micros();
    bool change_1 = false;

    StaticJsonDocument<500> json_1;  // size = 30*topic [avg]
    // check data change
    for (int i = 0; i < sizeof(def_tb) / sizeof(def_tb[0]); i++) {
      if (def_tb[i][2] == "3" || def_tb[i][2] == "4") {
        if (def_tb[i][3] != def_tb[i][4]) {
          change_1 = true;
          break;
        }
      }
    }

    if (change_1 == true) {  // data change !!!

      /*----------- RSSI value -----------*/
      json_1["rssi"] = (float)WiFi.RSSI();

      /*----------- Production data -----------*/
      for (int j = 0; j < (sizeof(def_tb) / sizeof(def_tb[0])); j++) {
        /*----------- Production data type a normal -----------*/
        if (def_tb[j][2] == "3") {
          json_1[String(def_tb[j][0])] = (def_tb[j][3]).toInt();
        }
        /*----------- Production data type over value 65535 -----------*/
        if (def_tb[j][2] == "4") {
          total_data = (def_tb[j][3]).toFloat() + ((def_tb[j + 1][3]).toFloat() * Add_convert);
          json_1[String(def_tb[j][0])] = total_data;
          j++;
        }
        /*----------------- ID Number -----------------*/
        if (def_tb[j][2] == "5") {
          if (def_tb[j][3].toInt() != 0) {
            String hex_ = String((def_tb[j][3]).toInt(), HEX);  //convert data to HEX and define -> String
            String firstPart = hex_.substring(2, 4);            // Split data
            String secondPart = hex_.substring(0, 2);
            long ascii_1 = strtol(firstPart.c_str(), NULL, 16);  //convert data HEX to DEC
            long ascii_2 = strtol(secondPart.c_str(), NULL, 16);
            //Id_num = String(ascii_1) + String(ascii_2);
            //json_1[String(def_tb[m][0])] = Id_num.toInt();  //Tx DEC to MQTT type json file
            if (ascii_1 == 32) {
              ascii_1 = 0;
            }
            if (ascii_2 == 32) {
              ascii_2 = 0;
            }
            String Id_num = String(char(ascii_1)) + String(char(ascii_2));
            Id_ttl += Id_num;
          }
        }
      }
      json_1["id_num"] = Id_ttl;

      /*----------- Publish data -----------*/
      String json_topic1;
      serializeJson(json_1, json_topic1);
      // instance->publishMessage(mcNo->mc_no, json_topic1.c_str());
      instance->publishMessage(topic_pub, json_topic1.c_str());
      Serial.println(json_topic1);
      for (int p = 0; p < sizeof(def_tb) / sizeof(def_tb[0]); p++) {
        if (def_tb[p][2] == "3" || def_tb[p][2] == "4" || def_tb[p][2] == "5") {
          def_tb[p][4] = def_tb[p][3];
          if (def_tb[p][2] == "5") {
            if (def_tb[p][3].toInt() != 0) {
              Id_ttl = '\0';
            }
          }
        }
      }
      ct_fn1 = micros() - start;
    }
    //interval work loop 100-120 ms
    vTaskDelay(pdMS_TO_TICKS(itr_fnc_1));  //check every 1 sec
  }
}

void MicMMS::func2_Task(void* pvParam) {
  MicMMS* instance = (MicMMS*)pvParam;
  MicMMS* dpName = (MicMMS*)(pvParam);
  MicMMS* macNo = (MicMMS*)(pvParam);

  char topic_pub[30];
  strcpy(topic_pub, topic_pub_2);
  strcat(topic_pub, dpName->dp_name);
  strcat(topic_pub, macNo->mac_no);

  while (1) {
    unsigned long long int start = micros();
    bool data_check1 = false;
    uint8_t count_data1 = 0;
    for (int i = 0; i < sizeof(def_tb) / sizeof(def_tb[0]); i++) {
      if (def_tb[i][2] == "1") {    //type status
        if (def_tb[i][3] == "1") {  //value to register(number)
          count_data1++;            //count_data1 = 1
        }
      }
    }

    if (count_data1 == 1) {  // condition to protection from many value
      data_check1 = true;
    } else {
      data_check1 = false;
      count_data1 = 0;
    }

    StaticJsonDocument<300> json_2;
    if (data_check1 == true) {  // data change and only one and Date aren't "0"!!
      for (int i = 0; i < sizeof(def_tb) / sizeof(def_tb[0]); i++) {
        if (def_tb[i][2] == "1") {
          if (def_tb[i][3] == "1") {
            status = def_tb[i][0];
            json_2["status"] = status;
          }
        }
      }
    }
    /*----------- Publish data -----------*/
    if (status != prv_status) {
      String json_topic2;
      serializeJson(json_2, json_topic2);
      instance->publishMessage(topic_pub, json_topic2.c_str());
      Serial.println(json_topic2);

      prv_status = status;
      ct_fn2 = micros() - start;
    }
    //interval work loop 100-120 ms
    vTaskDelay(pdMS_TO_TICKS(itr_fnc_2));
  }
}

void MicMMS::func3_Task(void* pvParam) {
  MicMMS* instance = (MicMMS*)pvParam;
  MicMMS* dpName = (MicMMS*)(pvParam);
  MicMMS* macNo = (MicMMS*)(pvParam);

  char topic_pub[30];
  strcpy(topic_pub, topic_pub_3);
  strcat(topic_pub, dpName->dp_name);
  strcat(topic_pub, macNo->mac_no);
  while (1) {
    unsigned long long int start = micros();
    bool Ready_1 = false;
    StaticJsonDocument<300> json_3;

    /*------- alarm list and Publish data -------*/
    for (int j = 0; j < sizeof(def_tb) / sizeof(def_tb[0]); j++) {
      if (def_tb[j][2] == "2") {
        if (def_tb[j][3] == "1" && def_tb[j][4] == "") {
          alarm_ = def_tb[j][0];
          json_3["status"] = alarm_;
          String json_topic3;
          serializeJson(json_3, json_topic3);
          instance->publishMessage(topic_pub, json_topic3.c_str());
          Serial.println(json_topic3);
          def_tb[j][4] = def_tb[j][3];
          ct_fn3 = micros() - start;
        }
        if (def_tb[j][3] == "1" && def_tb[j][4] == "0") {
          alarm_ = def_tb[j][0];
          json_3["status"] = alarm_;
          String json_topic3;
          serializeJson(json_3, json_topic3);
          instance->publishMessage(topic_pub, json_topic3.c_str());
          Serial.println(json_topic3);
          def_tb[j][4] = def_tb[j][3];
          ct_fn3 = micros() - start;
        }
        if (def_tb[j][3] == "0" && def_tb[j][4] == "1") {
          alarm_ = def_tb[j][0];
          json_3["status"] = alarm_ + "_";
          String json_topic3;
          serializeJson(json_3, json_topic3);
          instance->publishMessage(topic_pub, json_topic3.c_str());
          Serial.println(json_topic3);
          def_tb[j][4] = def_tb[j][3];
          ct_fn3 = micros() - start;
        }
      }
    }
    //interval work loop 100-120 ms
    vTaskDelay(pdMS_TO_TICKS(itr_fnc_3));
  }
}

void MicMMS::broke_modbus_Task(void* pvParam) {  //Check modbus,Broker alive
  MicMMS* instance = (MicMMS*)pvParam;
  MicMMS* dpName = (MicMMS*)(pvParam);
  MicMMS* macNo = (MicMMS*)(pvParam);
  MicMMS* vrs_Code = (MicMMS*)(pvParam);

  char topic_pub[30];
  strcpy(topic_pub, topic_broke_modbus);
  strcat(topic_pub, dpName->dp_name);
  strcat(topic_pub, macNo->mac_no);

  while (1) {
    unsigned long long int start = millis();
    StaticJsonDocument<200> json_4;
    String json_topic4;

    if (instance->mqttClient.connected()) {
      bkr_connect = 1;
    } else {
      bkr_connect = 0;
    }
    // Serial.println(modb_check);
    json_4["mac_id"] = WiFi.macAddress();
    json_4["broker"] = bkr_connect;
    json_4["modbus"] = modb_check;                                                                 // Modbus Ethernet
    json_4["version"] = vrs_Code->vrs_code;                                                        // code version
    if (((bkr_connect == 1) && (tigger_1 == 1)) || ((start - prv_time_1) >= (5 * (60 * 1000)))) {  // Use tigger = 1 for Publish first time And Publish data every 5 mins.
      serializeJson(json_4, json_topic4);
      instance->publishMessage(topic_pub, json_topic4.c_str());
      Serial.println(json_topic4);
      prv_time_1 = start;
      tigger_1 = 0;  // End use tigger forever until Start program again.
    }
    vTaskDelay(pdMS_TO_TICKS(itr_bro_mod));
  }
}

void MicMMS::esp_Task(void* pvParam) {  //ESP status
  MicMMS* instance = (MicMMS*)pvParam;
  MicMMS* dpName = (MicMMS*)(pvParam);
  MicMMS* macNo = (MicMMS*)(pvParam);

  char topic_pub[30];
  strcpy(topic_pub, topic_esp_health);
  strcat(topic_pub, dpName->dp_name);
  strcat(topic_pub, macNo->mac_no);

  while (1) {
    unsigned long long int start = millis();
    StaticJsonDocument<200> json_5;
    String json_topic5;
    float use_heap = (1 - (esp_get_free_heap_size() / init_heap)) * 100;
    // check heap
    if (use_heap >= 20.0 && use_heap <= 40.0) {
      heap_cnt1++;
    } else if (use_heap > 40.0 && use_heap <= 60.0) {
      heap_cnt2++;
    } else if (use_heap > 60.0) {
      heap_cnt3++;
    }
    // check cpu
    float read_over = ((ct_read / ct_read_) - 1) * 100;
    if (read_over > 80) {
      ct_read_cnt++;
    }
    float fnc1_over = ((ct_fn1 / ct_fn1_) - 1) * 100;
    if (fnc1_over > 80) {
      ct_fn1_cnt++;
    }
    float fnc2_over = ((ct_fn2 / ct_fn2_) - 1) * 100;
    if (fnc2_over > 80) {
      ct_fn2_cnt++;
    }
    float fnc3_over = ((ct_fn3 / ct_fn3_) - 1) * 100;
    if (fnc3_over > 80) {
      ct_fn3_cnt++;
    }

    if (start - prv_time >= (12 * (60 * (60 * 1000)))) {  // 12hr
      json_5["mem_use"] = use_heap;
      json_5["mem_cnt1"] = heap_cnt1;
      json_5["mem_cnt2"] = heap_cnt2;
      json_5["mem_cnt3"] = heap_cnt3;
      json_5["cpu_fn0"] = ct_read_cnt;
      json_5["cpu_fn1"] = ct_fn1_cnt;
      json_5["cpu_fn2"] = ct_fn2_cnt;
      json_5["cpu_fn3"] = ct_fn3_cnt;

      serializeJson(json_5, json_topic5);
      instance->publishMessage(topic_pub, json_topic5.c_str());
      Serial.println(json_topic5);
      prv_time = start;
      heap_cnt1 = 0;
      heap_cnt2 = 0;
      heap_cnt3 = 0;
      ct_read_cnt = 0;
      ct_fn1_cnt = 0;
      ct_fn2_cnt = 0;
      ct_fn3_cnt = 0;
    }
    ct_read = 0;
    ct_fn1 = 0;
    ct_fn2 = 0;
    ct_fn3 = 0;
    vTaskDelay(pdMS_TO_TICKS(itr_esp));
  }
}
