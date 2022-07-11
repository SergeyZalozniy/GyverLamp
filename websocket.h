#include <WebSocketsServer.h> // WebSockets lib

WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketSendCurrentMode() {
    String text = String("effect") + currentMode;
    webSocket.broadcastTXT(text);
    text = String("volume") + modes[currentMode].brightness;
    webSocket.broadcastTXT(text);
    text = String("speed") + modes[currentMode].speed;
    webSocket.broadcastTXT(text);
    text = String("scale") + modes[currentMode].scale;
    webSocket.broadcastTXT(text);
    if (turnOffTime > 0) {
      String text = String("timer") + (turnOffTime - millis()) / 1000;
      webSocket.broadcastTXT(text);          
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  switch(type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED:
      {
        String text = String("effect") + currentMode;
    		webSocket.sendTXT(num,  text);
        text = String("volume") + modes[currentMode].brightness;
        webSocket.sendTXT(num,  text);
        text = String("speed") + modes[currentMode].speed;
        webSocket.sendTXT(num,  text);
        text = String("scale") + modes[currentMode].scale;
        webSocket.sendTXT(num,  text);
        if (turnOffTime > 0) {
          String text = String("timer") + (turnOffTime - millis()) / 1000;
          webSocket.sendTXT(num,  text);          
        }
      }
      break;
    case WStype_TEXT:
      webSocket.broadcastTXT(payload, length);
      break;
    case WStype_BIN:
      webSocket.sendBIN(num, payload, length);
      break;
    default:
      break;
  }
}
