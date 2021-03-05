
/*
    'lightControlMeshBridge1' by Thurstan. WIFI to Mesh bridge for MQTT control.
    Copyright (C) 2021 MTS Standish (Thurstan|mattKsp)
    
    https://github.com/mattThurstan/

    Adapted from painlessMesh examples for mqqt network bridging.
    
    WeMos D1 (R2 &) mini ESP8266, 80 MHz, 115200 baud, 4M, (1M SPIFFS)
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
// To broadcast a message to all nodes in the mesh you can publish it to "mesh/to/all".
// When you publish "getNodes" to "mesh/to/bridge" you receive the mesh topology as JSON
//
// Listens for (subscribes to):
//
// mesh/to/#
// house/bridge1/#
// house/stairs1/#
// house/desk1/#
// house/desk2/#
// house/kitchen1/#
// house/longboard1/#
// house/leaningbookshelves1/#
// house/futonbed1/#
// house/livingroomdivider1/#
//       
//************************************************************

/*----------------------------libraries----------------------------*/
#include <MT_LightControlDefines.h> 
#include <Arduino.h>
#include <painlessMesh.h>                           // https://github.com/gmag11/painlessMesh
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>


/*----------------------------system----------------------------*/
const String _progName = "lightControlMeshBridge1"; // bridge Mesh to WIFI
const String _progVers = "0.605";                   // 2021

boolean DEBUG_GEN = false;                          // realtime serial debugging output - general
boolean DEBUG_COMMS = false;                        // realtime serial debugging output - comms


#define HOSTNAME "MlC_Bridge1"                      // MlC = Mesh light Control
#define WILL_TOPIC "house/bridge1/available"
#define WILL_MESSAGE "offline"

IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);

painlessMesh  mesh;
WiFiClient wifiClient;
SimpleList<uint32_t> nodes;

unsigned long _lastReconnectAttempt = 0;
const long _interval = 5000;

void receivedCallback( const uint32_t &from, const String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);

void newConnectionCallback(uint32_t nodeId) {
  checkDevicesStatus();
  if (DEBUG_COMMS && Serial) { Serial.printf("--> lightControlMeshBridge1: New Connection, nodeId = %u\n", nodeId); }
}

void changedConnectionCallback() {
  checkDevicesStatus();
  if (DEBUG_COMMS && Serial) { Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str()); }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  checkDevicesStatus(); // ..too much?
  if (DEBUG_COMMS && Serial) { Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset); }
}

PubSubClient mqttClient(MQTT_BROKER_IP, MQTT_BROKER_PORT, mqttCallback, wifiClient);


/*----------------------------MAIN----------------------------*/
void setup() {
  
  // start serial regardless but control debug output from mqtt
  Serial.begin(115200);
  
  Serial.println();
  Serial.print(F("Welcome to "));
  Serial.print(_progName);
  Serial.print(", volume");
  Serial.print(_progVers);
  Serial.println();
  Serial.print(F("Brought to you live from the Gardening For Nomes network headquarters!"));
  Serial.println();
  Serial.print("..");
  Serial.println();
  
  //loadConfig();
  setupMesh();
  
  //everything done? ok then..
  Serial.print(F("Setup done"));
  Serial.println("-----");
  Serial.print(F("Device Node ID is "));
  String s = String(mesh.getNodeId());
  Serial.println(s);
  Serial.println("-----");
  Serial.println("");
  
  
  _lastReconnectAttempt = 0;
  delay(1500);
}

void loop() {
  
  mesh.update();

  if(myIP != getlocalIP())
  {
    // this will get triggered on the very first loop
    myIP = getlocalIP();
    if (DEBUG_COMMS && Serial) { 
      Serial.println("My IP is " + myIP.toString());
      String s = String(mesh.getNodeId());
      Serial.print("Device Node ID is ");
      Serial.println(s);
      //attachedNodes();
    }
    mqqtConnect();
    checkDevicesStatus();
  }

  unsigned long now = millis();
  if (!mqttClient.connected()) {
    if (now - _lastReconnectAttempt > _interval) {
      _lastReconnectAttempt = now;
      // Attempt to reconnect
      if (DEBUG_COMMS && Serial) { Serial.println("Attempting to reconnect to MQTT broker..."); }
      if (mqttReconnect()) {
        //attachedNodes();
        checkDevicesStatus();
        _lastReconnectAttempt = 0;
        if (DEBUG_COMMS && Serial) { Serial.println("Reconnect to MQTT broker successful!"); }
      }
    }
  } else {
    // Client connected
    mqttClient.loop();
  }
  
}
