/*----------------------------utils----------------------------*/

void publishDeviceOffline() {
  mqttClient.publish("house/bridge1/status", "OFF"); 
  mqttClient.publish("house/bridge1/available","offline");
}

void deviceRestart() {
  ESP.restart();
}

void deviceShutdown() { /* ??? */ }

void turnOffWifi() {
  if (DEBUG_GEN && Serial) { Serial.println("Disconnecting wifi..."); }
  WiFi.disconnect();
}

void turnOffMesh() {
  if (DEBUG_GEN && Serial) { Serial.println("Disconnecting mesh..."); }
  mesh.stop();
}

void turnOffSerial() {
  if (DEBUG_GEN && Serial) { Serial.println("Disconnecting serial..."); }
  Serial.end();
}

/*----------------------------main calls-----------------------*/
/* Reset everything to default. */
void doReset() {
  //resetDefaults();
  //deviceRestart(); // ..and restart
}

/* Restart the device (with a delay) */
void doRestart(uint8_t restartTime) {
  delay(restartTime); // delay for restartTime
  deviceRestart(); // ..and restart
}

/*
 * Lockdown.
 * 
 * Emergency global disconnect (requires hard reset). 
 *  0 = do thing (no severity)
 *  1 = disconnect from LAN (bridges shutdown and mesh reboots) 
 *  2 = shutdown mesh (everything reboots in standalone mode) 
 *  3 = shutdown everything. 
 *      - Devices to power off if possible, if not then reboot in standalone emergency mode. 
 *      - These devices will require a hardware reset button implemented to clear the emergency mode.)
 */
void doLockdown(uint8_t severity) {
  if (severity == 0) { /* do nothing */ } 
  else if (severity == 1) {
    turnOffComms();
    // set to restart with no comms
    doRestart(0);
  } else if (severity == 2) {
    turnOffComms();
    // set to restart in standalone mode
    doRestart(0);
  } else if (severity == 3) {
    turnOffComms();
    //set emergency mode
    //set lockdown severity (includes restart)
    deviceShutdown(); // if possible
    // if not then set to restart in standalone emergency mode
    //deviceRestart(); // ..and restart 
  } 
  else { }
}
