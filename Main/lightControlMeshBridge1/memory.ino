/*----------------------------memory----------------------------*/
void loadSettings() { }

void saveSettings() { }

void clearSettings() { }

void loopSaveSettings() { }

void resetDefaults() {
  clearSettings();
  setDefaults();
  saveSettings();
}

void setDefaults() {
  DEBUG_GEN = false;
  DEBUG_COMMS = false;
  
  _stationChannel = STATION_CHANNEL;
}
