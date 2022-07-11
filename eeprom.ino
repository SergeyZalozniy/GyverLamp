void saveEEPROM() {
  byte brightness = EEPROM.read(3 * currentMode + 40);
  byte speed = EEPROM.read(3 * currentMode + 40 + 1);
  byte scale = EEPROM.read(3 * currentMode + 40 + 2);
  
  if (modes[currentMode].brightness != brightness ||
      modes[currentMode].speed != speed ||
      modes[currentMode].scale != scale) {
      EEPROM.put(3 * currentMode + 40, modes[currentMode]);
      EEPROM.commit();
  }
}

void eepromTick() {
  if (settChanged && (millis() - eepromTimer > 10000)) {
    settChanged = false;
    eepromTimer = millis();
    saveEEPROM();
    if (EEPROM.read(200) != currentMode) {
      EEPROM.write(200, currentMode);
      EEPROM.commit();
    }
  }
}
