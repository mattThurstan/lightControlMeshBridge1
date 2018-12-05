/*----------------------------MQTT----------------------------*/
void parseMQTT(String targetStr, String msg)
{
  //at this point we are now expecting the first part of the address to be a device name
  //which we then translate into a node ID ..or not
  uint8_t firstTargetStrIndex = targetStr.indexOf('/');
  String targetNode = targetStr.substring(0, firstTargetStrIndex);  //get the device name
  String targetSub = targetStr.substring(firstTargetStrIndex);      //get the rest of the address

  //get target by way of device name
  uint32_t target = strtoul(targetNode.c_str(), NULL, 10);
/*  uint32_t target;
  if (targetNode == "stairs1")
  { target = DEVICE_ID_STAIRS1; } 
  else if (targetNode == "desk1")
  { target = DEVICE_ID_DESK1; } 
  else if (targetNode == "desk2")
  { target = DEVICE_ID_DESK2; } 
  else if (targetNode == "kitchen1")
  { target = DEVICE_ID_KITCHEN1; } 
  else if (targetNode == "longboard1")
  { target = DEVICE_ID_LONGBOARD1; }
*/  
  if (target == 0) { /* SYSTEM SPARE */ }
  else if (target == 1) { /* SYSTEM SPARE */ }
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
    }
    else
    {
      String sm = "Node ";
      sm += targetNode;
      sm += " not connected!";
      mqttClient.publish("mesh/from/bridge", sm.c_str());
    }
  }
}

void buildMQTT() { }
void parseMessages() { }
void buildMessages() { }

