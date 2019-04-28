/*
    'lightControlMeshBridge1' by Thurstan. WIFI to Mesh bridge for MQTT control.
    Copyright (C) 2019 MTS Standish (mattThurstan)
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
//       
//************************************************************

/*----------------------------libraries----------------------------*/
// board running at [ this-> 80mhz || 1600mhz ]
#include <Arduino.h>
#include <FastLED.h>                          // WS2812B LED strip control and effects
#include <painlessMesh.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <MT_LightControlDefines.h> 


/*----------------------------system----------------------------*/
const String _progName = "lightControlMeshBridge1"; // bridge Mesh to WIFI
const String _progVers = "0.570";                   // mqtt bridge online status

boolean DEBUG_GEN = true;                           // realtime serial debugging output - general
boolean DEBUG_COMMS = true;                         // realtime serial debugging output - comms


#define HOSTNAME "MlC_Bridge1"                      // MlC = Mesh light Control
#define WILL_TOPIC "house/bridge1/available"
#define WILL_MESSAGE "offline"

IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);

painlessMesh  mesh;
WiFiClient wifiClient;
SimpleList<uint32_t> nodes;

void receivedCallback( const uint32_t &from, const String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);

void newConnectionCallback(uint32_t nodeId) {
  checkDevicesStatus();
  if (DEBUG_COMMS) { Serial.printf("--> lightControlMeshBridge1: New Connection, nodeId = %u\n", nodeId); }
}

void changedConnectionCallback() {
  checkDevicesStatus();
  if (DEBUG_COMMS) { Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str()); }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  checkDevicesStatus(); // ..too much?
  if (DEBUG_COMMS) { Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset); }
}

PubSubClient mqttClient(MQTT_BROKER_IP, MQTT_BROKER_PORT, mqttCallback, wifiClient);


//int _ledState = LOW;
//unsigned long _previousMillis = 0;
unsigned long _lastReconnectAttempt = 0;
const long _interval = 5000;

/*----------------------------MAIN----------------------------*/
void setup() {
  
  Serial.begin(115200);
  
  Serial.println();
  Serial.print(_progName);
  Serial.print(" v");
  Serial.print(_progVers);
  Serial.println();
  Serial.print("..");
  Serial.println();
  
//  pinMode(LED_BUILTIN, OUTPUT);
//  _ledState = HIGH;  // off
//  digitalWrite(LED_BUILTIN, _ledState);

  //loadConfig();
  setupMesh();
  
  delay(1500);
  _lastReconnectAttempt = 0;
}

void loop() {
  
  mesh.update();

  if(myIP != getlocalIP())
  {
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
    String s = String(mesh.getNodeId());
    Serial.print("Device Node ID is ");
    Serial.println(s);
    //Serial.print("Attached Node IDs are ");
    //Serial.println(mesh.subConnectionJson()); //.c_str()

    //nodes = mesh.getNodeList();
    //Serial.printf("Num nodes: %d\n", nodes.size());
    //Serial.printf("Attached Node IDs are : ");
    
    //SimpleList::iterator node = nodes.begin();
    //while (node != nodes.end()) {
    //  Serial.printf(" %u", *node);
    //  node++; 
    //}
//    for (SimpleList<uint32_t>::iterator itr = mesh.getNodeList().begin(); itr != mesh.getNodeList().end(); ++itr)
//    {
//      Serial.print(*itr);
//      Serial.println(" : ");
//    }
//    Serial.println();

    //mqqtConnect(); 
  }

//  unsigned long currentMillis = millis();
//  if (!mqttClient.connected()) {
//    // reconnect
//  } else {
//    if (currentMillis - _previousMillis >= _interval) {
//      _previousMillis = currentMillis;
//      //mqttClient.publish("house/bridge1/available","online");
//      //if (DEBUG_COMMS) { Serial.println("bridge: Sent msg available online"); }
//    }
//  }
  
//  mqttClient.loop();

    unsigned long now = millis();
  if (!mqttClient.connected()) {
    if (now - _lastReconnectAttempt > _interval) {
      _lastReconnectAttempt = now;
      // Attempt to reconnect
      if (mqttReconnect()) {
        _lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    //checkDevicesStatus(); // this needs to be every 30 secs or more
    mqttClient.loop();
  }
  
} // END setup
  
//  EVERY_N_SECONDS(10) {
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

//  } // END EVERY_N_SECONDS
  
//  unsigned long currentMillis = millis();
//  if (mesh.isConnected(mesh.getNodeId())) { //DEBUG_COMMS && 
//    if (currentMillis - _previousMillis >= _interval) {
//      _previousMillis = currentMillis;
//      if (_ledState == LOW) {
//        _ledState = HIGH;  // Note that this switches the LED *off*
//      } else {
//        _ledState = LOW;  // Note that this switches the LED *on*
//      }
//      digitalWrite(LED_BUILTIN, _ledState);
//    }
//  }
  
