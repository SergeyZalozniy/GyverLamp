boolean brightDirection;

void buttonTick() {
  plusButton.tick();
  minusButton.tick();
  powerButton.tick();
  changeEffectButton.tick();

  if (powerButton.isPress()) {
    Serial.println("powerButton press");
    ONflag = !ONflag;
    changePower();
  }

  if (!ONflag) {
    return ;
  }

//  if (changeEffectButton.isHold()) {
    if (plusButton.isSingle()) {
      setNextEffect();
    }
    if (minusButton.isSingle()) {
      setPreviousEffect();
    }
//  }

//  if (changeEffectButton.isSingle()) {
//    setNextEffect();
//  }

  if (plusButton.isStep()) {
    if (modes[currentMode].brightness < 10) 
        modes[currentMode].brightness += 1;
    else if (modes[currentMode].brightness < 250) 
      modes[currentMode].brightness += 3;
    else 
      modes[currentMode].brightness = 255;

    FastLED.setBrightness(modes[currentMode].brightness);
    settChanged = true;
    eepromTimer = millis();
    webSocketSendCurrentMode();
  }

  if (minusButton.isStep()) {
     if (modes[currentMode].brightness > 15) 
        modes[currentMode].brightness -= 3;
    else if (modes[currentMode].brightness > 1) 
      modes[currentMode].brightness -= 1;
    else 
      modes[currentMode].brightness = 1;

    FastLED.setBrightness(modes[currentMode].brightness);
    settChanged = true;
    eepromTimer = millis();
    webSocketSendCurrentMode();
  }
}

void setNextEffect() {
    currentMode = (currentMode + 1) % MODE_AMOUNT;
    FastLED.setBrightness(modes[currentMode].brightness);
    loadingFlag = true;
    settChanged = true;
    eepromTimer = millis();
    FastLED.clear();
    delay(1);
    webSocketSendCurrentMode();
}

void setPreviousEffect() {
    if (--currentMode < 0) currentMode = (MODE_AMOUNT - 1);
    FastLED.setBrightness(modes[currentMode].brightness);
    loadingFlag = true;
    settChanged = true;
    eepromTimer = millis();
    FastLED.clear();
    delay(1);
    webSocketSendCurrentMode();
}
