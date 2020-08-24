/*----------------------------MQTT----------------------------*/
void mqqtConnect()
{
    if (mqttClient.connect(HOSTNAME, MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD))   // "thurstanMeshBridge"
    {
      //mqttClient.publish("mesh/from/bridge1","Ready!");
      mqttClient.publish("house/bridge1/available","online");
      mqttClient.publish("house/bridge1/status","ON");
      mqttClient.subscribe("mesh/to/#");
      mqttClient.subscribe("house/bridge1/#");
      mqttClient.subscribe("house/stairs1/#");
      mqttClient.subscribe("house/desk1/#");
      mqttClient.subscribe("house/desk2/#");
      mqttClient.subscribe("house/kitchen1/#");
      mqttClient.subscribe("house/longboard1/#");
      mqttClient.subscribe("house/leaningbookshelves1/#");
      mqttClient.subscribe("house/testNode/#");
      mqttClient.subscribe("sunrise");
      mqttClient.subscribe("sunset");
      if (DEBUG_COMMS) { Serial.printf("MQTT connected."); Serial.println(); }
    }
}

boolean mqttReconnect() 
{
  if (mqttClient.connect(HOSTNAME, MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD, WILL_TOPIC, 0, true, WILL_MESSAGE)) {
    // Once connected, publish an announcement...
    mqttClient.publish("house/bridge1/available","online");
    mqttClient.publish("house/bridge1/status","ON");
    // ... and resubscribe
    mqttClient.subscribe("mesh/to/#");
    mqttClient.subscribe("house/bridge1/#");
    mqttClient.subscribe("house/stairs1/#");
    mqttClient.subscribe("house/desk1/#");
    mqttClient.subscribe("house/desk2/#");
    mqttClient.subscribe("house/kitchen1/#");
    mqttClient.subscribe("house/longboard1/#");
    mqttClient.subscribe("house/leaningbookshelves1/#");
    mqttClient.subscribe("house/testNode/#");
    mqttClient.subscribe("sunrise");
    mqttClient.subscribe("sunset");
    if (DEBUG_COMMS) { Serial.printf("MQTT connected."); Serial.println(); }
    checkDevicesStatus();
  }
  return mqttClient.connected();
}

/*----------------------------MQTT callbacks----------------------------*/
// callback for Mesh messages
void receivedCallback( const uint32_t &from, const String &msg ) {
  if (DEBUG_COMMS) { Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str()); Serial.println(); }
  
  uint8_t firstMsgIndex = msg.indexOf(':');
  String targetSub = msg.substring(0, firstMsgIndex);
  String msgSub = msg.substring(firstMsgIndex+1);

  if (DEBUG_COMMS) { 
    Serial.printf("mesh msg callback msgSub: ");
    Serial.println(msgSub);
  }
  
  String topic = "house/";
  if (from == DEVICE_ID_STAIRS1)
  { topic += "stairs1/"; } 
  else if (from == DEVICE_ID_DESK1)
  { topic += "desk1/"; } 
  else if (from == DEVICE_ID_DESK2)
  { topic += "desk2/"; } 
  else if (from == DEVICE_ID_KITCHEN1)
  { topic += "kitchen1/"; } 
  else if (from == DEVICE_ID_LONGBOARD1)
  { topic += "longboard1/"; }
  //else if (from == DEVICE_ID_LONGBOARD1)
  //{ topic += "testNode/"; }
  else if (from == DEVICE_ID_LEANINGBOOKSHELVES1)
  { topic += "leaningbookshelves1/"; }
  else if (from == DEVICE_ID_FUTONBED1)
  { topic += "futonbed1/"; }
  
  topic += targetSub;
  mqttClient.publish(topic.c_str(), msgSub.c_str());
}

// callback for LAN messages
void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  if (DEBUG_COMMS) { Serial.printf("bridge: Received MQTT from network: msg=%s\n", topic); Serial.println(); }
  
  char* cleanPayload = (char*)malloc(length+1);
  payload[length] = '\0';
  memcpy(cleanPayload, payload, length+1);
  String msg = String(cleanPayload);
  free(cleanPayload);

  //String targetStr = String(topic).substring(8);  //starting after 'mesh/to/' (removes it)
  String targetStr = String(topic);

  if(targetStr == "mesh/to/bridge")
  {
    if(msg == "getNodes")
    {
      mqttClient.publish("mesh/from/bridge", mesh.subConnectionJson().c_str());
      if (DEBUG_COMMS) { Serial.printf("bridge: Sent msg to mesh: targetStr=%s\n", topic); Serial.println(); }
    }
  }
  else if(targetStr == "mesh/to/all") 
  {
    // ..all wrong, will probably remove this..
    //uint8_t firstTargetStrIndex = targetStr.indexOf('/');
    //String targetNode = targetStr.substring(0, firstTargetStrIndex);  //get the device name - don't need
    //String targetSub = targetStr.substring(firstTargetStrIndex);      //get the rest of the address
    String targetSub = targetStr.substring(12);      // get the rest of the address
    //add the rest of the address to the beginning of the message
    String ts = targetSub;
    ts += ":";
    ts += msg;
    mesh.sendBroadcast(ts); //send message
  }
  else if(targetStr == "sunrise") { mesh.sendBroadcast(msg); }
  else if(targetStr == "sunset") { mesh.sendBroadcast(msg); }
  else if(targetStr == "breath") { mesh.sendBroadcast(msg); }
  else
  {
    parseMQTT(targetStr, msg);
  }
}

/*----------------------------MQTT message parsing----------------------------*/
void parseMQTT(String topic, String msg)
{
  //remove 'house/' from (address) 'targetStr'
  String targetStr2 = String(topic).substring(6);  //starting after 'house/' (removes it)
  
  //at this point we are now expecting the first part of the address to be a device name
  //which we then translate into a node ID ..or not
  uint8_t firstTargetStrIndex = targetStr2.indexOf('/');
  String targetNode = targetStr2.substring(0, firstTargetStrIndex);  //get the device name
  String targetSub = targetStr2.substring(firstTargetStrIndex+1);      //get the rest of the address

  if (DEBUG_COMMS) { 
    Serial.printf("parseMQTT targetSub: ");
    Serial.println(targetSub);
    Serial.printf("parseMQTT msg: ");
    Serial.println(msg);
  }
  
  //get target by way of device name
  //uint32_t target = strtoul(targetNode.c_str(), NULL, 10);bridge1
  uint32_t target;
  if (targetNode == "bridge1")
  { target = 1; } 
  else if (targetNode == "stairs1")
  { target = DEVICE_ID_STAIRS1; } 
  else if (targetNode == "desk1")
  { target = DEVICE_ID_DESK1; } 
  else if (targetNode == "desk2")
  { target = DEVICE_ID_DESK2; } 
  else if (targetNode == "kitchen1")
  { target = DEVICE_ID_KITCHEN1; } 
  else if (targetNode == "longboard1")
  { target = DEVICE_ID_LONGBOARD1; }
  //else if (targetNode == "testNode")
  //{ target = DEVICE_ID_LONGBOARD1; }
  else if (targetNode == "leaningbookshelves1")
  { target = DEVICE_ID_LEANINGBOOKSHELVES1; }
  else if (targetNode == "futonbed1" )
  { target = DEVICE_ID_FUTONBED1; }

  if (target == 0) { /* SYSTEM SPARE */ }
  else if (target == 1) { 
    String o1 = "ON";
    String o2 = "OFF";
    if (targetSub == "debug/general/set") 
    {
      if(msg == "ON") { 
        DEBUG_GEN = true;
        mqttClient.publish("house/bridge1/debug/general/status", o1.c_str());
      } 
      else if(msg == "OFF") { 
        DEBUG_GEN = false; 
        mqttClient.publish("house/bridge1/debug/general/status", o2.c_str()); 
      }
    }
    else if(targetSub == "debug/comms/set") 
    {
      if(msg == "ON") { 
        DEBUG_COMMS = true;
        mqttClient.publish("house/bridge1/debug/comms/status", o1.c_str()); 
      } 
      else if(msg == "OFF") { 
        DEBUG_COMMS = false;
        mqttClient.publish("house/bridge1/debug/comms/status", o2.c_str()); 
      }
    }
    else if(targetSub == "debug/available/scan") 
    {
      if(msg == "ON") { 
        //DEBUG_COMMS = true;
        checkDevicesStatus();
        //mqttClient.publish("house/bridge1/debug/comms/status", o1.c_str()); 
      } 
      else if(msg == "OFF") { 
        //DEBUG_COMMS = false;
        //mqttClient.publish("house/bridge1/debug/comms/status", o1.c_str()); 
      }
    }
  }
  else if (target == 2) { /* SYSTEM SPARE */ }
  else {
    if (mesh.isConnected(target))
    {
      //add the rest of the address to the beginning of the message
      String ts = targetSub;
      ts += ":";
      ts += msg;
      //send message to target
      mesh.sendSingle(target, ts);
      //the target will decode all it's sub-parts
      if (DEBUG_COMMS) { 
        Serial.printf("bridge: Sent msg to mesh: "); 
        Serial.print(ts);
        Serial.print(",");
        Serial.println(target);
      }
    }
    else
    {
      String sm = "Node ";
      sm += targetNode;
      sm += " not connected!";
      mqttClient.publish("mesh/from/bridge", sm.c_str());
    }
  }
  if (DEBUG_COMMS) { Serial.println(); }
}

void buildMQTT() { }
void parseMessages() { }
void buildMessages() { }

void checkDevicesStatus() {
  // check the mesh to see if known devices are online
  uint32_t target;
  String cd1 = "h";
  //String ds = DEVICE_OFFLINE;
  String ds = "offline";
  for (int i = 0; i < 8; i++) 
  {
    if (i == 0) { target = DEVICE_ID_BRIDGE1; cd1 = "bridge1/available"; }
    else if (i == 1) { target = DEVICE_ID_STAIRS1; cd1 = "house/stairs1/available"; }
    else if (i == 2) { target = DEVICE_ID_DESK1; cd1 = "house/desk1/available"; }
    else if (i == 3) { target = DEVICE_ID_DESK2; cd1 = "house/desk2/available"; }
    else if (i == 4) { target = DEVICE_ID_KITCHEN1; cd1 = "house/kitchen1/available"; }
    else if (i == 5) { target = DEVICE_ID_LONGBOARD1; cd1 = "house/longboard1/available"; }
    else if (i == 6) { target = DEVICE_ID_LEANINGBOOKSHELVES1; cd1 = "house/leaningbookshelves1/available"; }
    else if (i == 7) { target = DEVICE_ID_FUTONBED1; cd1 = "house/futonbed1/available"; }

    if (mesh.isConnected(target)) {
      // is device is online then broadcast online mqtt message
      //ds = DEVICE_ONLINE;
      ds = "online";

      //send status request message to target
      String ts = "status/request";
      ts += ":";
      ts += "ON";
      mesh.sendSingle(target, ts);
      
    } else {
      // is device is online then broadcast offline mqtt message
      //ds = DEVICE_OFFLINE;
      ds = "offline";
    }
    if (i == 0) { ds = "online"; }  // hack cos we know this (bridge) is online
    mqttClient.publish(cd1.c_str(), ds.c_str());
    
    if (DEBUG_COMMS) { 
      Serial.print("bridge: Check and publish device online status, "); 
      Serial.print(cd1);
      Serial.print(" - ");
      Serial.print(ds); 
      Serial.println(); 
    }
    
  }
  //if (DEBUG_COMMS) { Serial.printf("bridge: Checked devices online status."); Serial.println(); }
}
