void changePower();

void handle_save_volume() {
	int volume = server.arg("volume").toInt();

	jsonWrite(configSetup, "volume", volume); // Получаем значение volume из запроса конвертируем в int сохраняем
	saveConfig();
	server.send(200, "text/plain", "OK");
	brig_changed = 1;
}

void handle_save_scale() {
	int scale = server.arg("scale").toInt();

	jsonWrite(configSetup, "scale", scale); // Получаем значение scale из запроса конвертируем в int сохраняем
	saveConfig();
	server.send(200, "text/plain", "OK");
	scale_changed = 1;
}

void handle_save_speed() {
	int speed = server.arg("speed").toInt();

	jsonWrite(configSetup, "speed", speed); // Получаем значение speed из запроса конвертируем в int сохраняем
	saveConfig();
	server.send(200, "text/plain", "OK");
	speed_changed = 1;
	
}

void handle_save_effect() {
	int effect = server.arg("effect").toInt();

	jsonWrite(configSetup, "effect", effect); // Получаем значение effect из запроса конвертируем в int сохраняем
	saveConfig();
	server.send(200, "text/plain", "OK");
	effect_changed = 1;
}

void handle_switch_state() {
  server.send(200, "text/plain", "OK");
  ONflag = !ONflag;
  changePower();
}

void handle_save_timer() {
  int timer = server.arg("timer").toInt();
  server.send(200, "text/plain", "OK");
  turnOffTime = millis() + timer * 1000;
}

void url_init() {
	server.on("/saveeffect", handle_save_effect); 
	server.on("/volume", handle_save_volume);    
	server.on("/speed", handle_save_speed); 
	server.on("/scale", handle_save_scale);
  server.on("/switchstate", handle_switch_state);
  server.on("/savetimer", handle_save_timer);
}
