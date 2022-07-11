void parseUDP() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;
    inputBuffer = packetBuffer;

    if (inputBuffer.startsWith("DEB")) {
      inputBuffer = "OK " + timeClient.getFormattedTime();
    } else if (inputBuffer.startsWith("GET")) {
      sendCurrent();
    } else if (inputBuffer.startsWith("EFF")) {
      saveEEPROM();
      currentMode = (byte)inputBuffer.substring(3).toInt();
      loadingFlag = true;
      FastLED.clear();
      delay(1);
      sendCurrent();
      FastLED.setBrightness(modes[currentMode].brightness);
    } else if (inputBuffer.startsWith("BRI")) {
      modes[currentMode].brightness = inputBuffer.substring(3).toInt();
      FastLED.setBrightness(modes[currentMode].brightness);
      settChanged = true;
      eepromTimer = millis();
    } else if (inputBuffer.startsWith("SPD")) {
      modes[currentMode].speed = inputBuffer.substring(3).toInt();
      loadingFlag = true;
      settChanged = true;
      eepromTimer = millis();
    } else if (inputBuffer.startsWith("SCA")) {
      modes[currentMode].scale = inputBuffer.substring(3).toInt();
      loadingFlag = true;
      settChanged = true;
      eepromTimer = millis();
    } else if (inputBuffer.startsWith("P_ON")) {
      ONflag = true;
      changePower();
      sendCurrent();
    } else if (inputBuffer.startsWith("P_OFF")) {
      ONflag = false;
      changePower();
      sendCurrent();
    } else if (inputBuffer.startsWith("DAWN")) {
      dawnMode = inputBuffer.substring(4).toInt() - 1;
    }

    uint8_t reply[inputBuffer.length() + 1];
    inputBuffer.toCharArray((char *)reply, inputBuffer.length() + 1);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(reply, inputBuffer.length());
    Udp.endPacket();
  }
}

void sendCurrent() {
  inputBuffer = "CURR";
  inputBuffer += " ";
  inputBuffer += String(currentMode);
  inputBuffer += " ";
  inputBuffer += String(modes[currentMode].brightness);
  inputBuffer += " ";
  inputBuffer += String(modes[currentMode].speed);
  inputBuffer += " ";
  inputBuffer += String(modes[currentMode].scale);
  inputBuffer += " ";
  inputBuffer += String(ONflag);
}
