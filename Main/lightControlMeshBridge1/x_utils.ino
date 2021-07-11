/*----------------------------utils----------------------------*/
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

void factoryReset() { /* TODO */ }

void deviceShutdown() { /* ??? */ }

void deviceRestart() {
  ESP.restart();
}

/*----------------------------main calls-----------------------*/
/* Reset everything to default. */
void doReset() {
  resetDefaults();
  deviceRestart(); // ..and restart
}

/* Restart the device (with a delay) */
void doRestart(uint8_t restartTime) {
  uint16_t dly = (restartTime * 60 * 1000); // static ???
  delay(dly);
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
