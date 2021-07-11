/*----------------------------comms----------------------------*/
void setupMesh()
{
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // | CONNECTION set before init() so that you can see startup messages

  // Channel set to .. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  //mesh.init(String ssid, String password, Scheduler *baseScheduler, uint16_t port = 5555, WiFiMode_t connectMode = WIFI_AP_STA, uint8_t channel = 1, uint8_t hidden = 0, uint8_t maxconn = MAX_CONN);
  //mesh.init(String ssid, String password, uint16_t port = 5555, WiFiMode_t connectMode = WIFI_AP_STA, uint8_t channel = 1, uint8_t hidden = 0, uint8_t maxconn = MAX_CONN);
  mesh.init( MESH_NAME, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, _stationChannel, MESH_BRIDGE_HIDDEN );
  
  mesh.setRoot(true); //..don't really want to do this in case bridge is taken offline.. in fact, want another node as root, one that is on all the time..
  //mesh.setContainsRoot(true); 
  
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
}

/*
 * Runs after mesh is setup.
 * Checks the current channel against saved channel.
 * If it doesn't match then sends global mesh channel change, 
 * which then reboots the mesh to match the bridge channel.
 */
void checkChannel() {
  int32_t c = WiFi.channel();
  uint8_t c2 = c;
  if (c2 != _stationChannel) {
    String c3 = (String)c2;
    String message = "channel:";
    message += c3;
    mesh.sendBroadcast(message);
    changeChannel(c2);  // inc. restart
  }
}

void changeChannel(uint8_t channel) {
  _stationChannel = channel;
  saveSettings();
  //deviceRestart();  // straight restart, no delay
  doRestart(0);
}

void turnOffComms() 
{
  publishDeviceOffline();
  DEBUG_COMMS = false;
  turnOffMesh();
  turnOffWifi();
  DEBUG_GEN = false;
  turnOffSerial();
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}

void attachedNodes() {
  
//    for (SimpleList<uint32_t>::iterator itr = mesh.getNodeList().begin(); itr != mesh.getNodeList().end(); ++itr)
//    {
//      Serial.print(*itr);
//      Serial.println(" : ");
//    }
//    Serial.println();

//    Serial.print("Attached Node IDs are ");
//    Serial.println(mesh.subConnectionJson()); //.c_str()

//    nodes = mesh.getNodeList();
//    Serial.printf("Num nodes: %d\n", nodes.size());
//    Serial.printf("Attached Node IDs are : ");
    
//    SimpleList::iterator node = nodes.begin();
//    while (node != nodes.end()) {
//      Serial.printf(" %u", *node);
//      node++; 
//    }

}
  
