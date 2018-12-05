/*----------------------------comms----------------------------*/
void setupMesh()
{
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init( MESH_NAME, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, STATION_CHANNEL );
  mesh.onReceive(&receivedCallback);
  
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}

