/*
    'lightControlMeshBridge1' by Thurstan. WIFI to Mesh bridge for MQTT control.
    Copyright (C) 2018 MTS Standish (mattThurstan)
    https://github.com/mattThurstan/

    Adapted from painlessMesh examples for mqqt network bridging.
*/

//************************************************************
// this is a simple example that uses the painlessMesh library to
// connect to a another network and relay messages from a MQTT broker to the nodes of the mesh network.
// To send a message to a mesh node, you can publish it to "painlessMesh/to/12345678" where 12345678 equals the nodeId.
// To broadcast a message to all nodes in the mesh you can publish it to "painlessMesh/to/broadcast".
// When you publish "getNodes" to "painlessMesh/to/gateway" you receive the mesh topology as JSON
// Every message from the mesh which is send to the gateway node will be published to "painlessMesh/from/12345678" where 12345678 
// is the nodeId from which the packet was send.
//************************************************************

//************************************************************
// --edited version--
// connect to a another network and relay messages from a MQTT broker to the nodes of the mesh network.
// To send a message to a mesh node, you can publish it to "mesh/to/12345678" where 12345678 equals the nodeId.
// To broadcast a message to all nodes in the mesh you can publish it to "mesh/to/all".
// When you publish "getNodes" to "mesh/to/bridge" you receive the mesh topology as JSON
// Every message from the mesh which is send to the bridge node will be published to "mesh/from/12345678" where 12345678 
// is the nodeId from which the packet was send.
//************************************************************

#include <Arduino.h>
#include <painlessMesh.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <MT_LightControlDefines.h> 


/*----------------------------system----------------------------*/
const String _progName = "lightControlMeshBridge1"; // bridge Mesh to WIFI
const String _progVers = "0.5";                     // cleanup
#define DEBUG 1

// MlC = Mesh light Control
// LlC = Longboard light Control
#define HOSTNAME "MlC_Bridge1"
//#define HOSTNAME "lightControlBridge1"

// Prototypes
void receivedCallback( const uint32_t &from, const String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);

IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);

painlessMesh  mesh;
WiFiClient wifiClient;
PubSubClient mqttClient(MQTT_BROKER_IP, MQTT_BROKER_PORT, mqttCallback, wifiClient);
 

/*----------------------------MAIN----------------------------*/
void setup() {
  Serial.begin(115200);
  Serial.println();
  
  if (DEBUG) {
    Serial.println();
    Serial.print(_progName);
    Serial.print(" ");
    Serial.print(_progVers);
    Serial.println();
    Serial.print("..");
    Serial.println();
  }

  //loadConfig();
  setupMesh();
}

void loop() {
  mesh.update();
  mqttClient.loop();

  if(myIP != getlocalIP())
  {
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
    
    String s = String(mesh.getNodeId());
    Serial.print("Device Node ID is ");
    Serial.println(s);

    if (mqttClient.connect("meshClient")) {
      mqttClient.publish("mesh/from/bridge","Ready!");
      mqttClient.subscribe("mesh/to/#");
    } 
  }
}

void receivedCallback( const uint32_t &from, const String &msg ) {
  if (DEBUG) { Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str()); }
  
  uint8_t firstMsgIndex = msg.indexOf(':');
  String targetSub = msg.substring(0, firstMsgIndex);
  String msgSub = msg.substring(firstMsgIndex);
  
  String topic = "mesh/from/" + String(from);
  topic += targetSub;
  mqttClient.publish(topic.c_str(), msgSub.c_str());
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  char* cleanPayload = (char*)malloc(length+1);
  payload[length] = '\0';
  memcpy(cleanPayload, payload, length+1);
  String msg = String(cleanPayload);
  free(cleanPayload);

  String targetStr = String(topic).substring(8);  //starting after 'mesh/to/' (removes it)

  if(targetStr == "bridge")
  {
    if(msg == "getNodes")
    {
      mqttClient.publish("mesh/from/bridge", mesh.subConnectionJson().c_str());
    }
  }
  else if(targetStr == "all") 
  {
    uint8_t firstTargetStrIndex = targetStr.indexOf('/');
    //String targetNode = targetStr.substring(0, firstTargetStrIndex);  //get the device name - don't need
    String targetSub = targetStr.substring(firstTargetStrIndex);      //get the rest of the address
    //add the rest of the address to the beginning of the message
    String ts = targetSub;
    ts += ":";
    ts += msg;
    mesh.sendBroadcast(ts); //send message
  }
  else
  {
    parseMQTT(targetStr, msg);
  }
}

