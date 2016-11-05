
// my Chip IDs to give it a cleartype name
#define WA1 14117293
#define WA2 12612352


bool sendTEW(byte sparkfunType, float temp, float hum, float battery) {

  // Use Sparkfun testing stream
  const char* host = "84.117.174.210";
  const char* streamId   = "";
  const char* privateKey = "";

  Serial.print("Connecting to "); Serial.print(host);

  int retries = 5;
  while (!client.connect(host, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!client.connected()) {
    Serial.println("Failed to connect, going back to sleep");
    return false;
  }

  String url = "/test.php";
  
  Serial.println();
  Serial.print("TEW: ");


  Serial.print("Request URL: "); Serial.println(url);

  String message = "0[t]=";
  message.concat(temp);
  message.concat("&0[h]=");
  message.concat(hum);
  message.concat("&0[p]=");
  message.concat(battery);

  String cmd = "POST ";
  cmd.concat(url);
  cmd.concat(" HTTP/1.0\r\nHost: ");
  cmd.concat(host);
  cmd.concat("\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ");
  cmd.concat(message.length());
  cmd.concat("\r\n\r\n");
  cmd.concat(message);
  cmd.concat("\r\n\r\n");
  
  client.print(cmd);

  int timeout = 5 * 10; // 5 seconds
  while (!client.available() && (timeout-- > 0)) {
    delay(100);
  }

  if (!client.available()) {
    Serial.println("No response, going back to sleep");
    return false;
  }
  Serial.println(F("disconnected"));
  return true;
}

bool sendTEWMessage(byte sparkfunType, String message) {

  // Use Sparkfun testing stream
  const char* host = "84.117.174.210";
  const char* streamId   = "";
  const char* privateKey = "";

  Serial.print("Connecting to "); Serial.print(host);

  int retries = 5;
  while (!client.connect(host, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!client.connected()) {
    Serial.println("Failed to connect, going back to sleep");
    return false;
  }

  String url = "/test.php";
  
  Serial.println();
  Serial.print("TEW: ");


  Serial.print("Request URL: "); Serial.println(url);

  

  String cmd = "POST ";
  cmd.concat(url);
  cmd.concat(" HTTP/1.0\r\nHost: ");
  cmd.concat(host);
  cmd.concat("\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ");
  cmd.concat(message.length());
  cmd.concat("\r\n\r\n");
  cmd.concat(message);
  cmd.concat("\r\n\r\n");
  
  client.print(cmd);

  int timeout = 5 * 10; // 5 seconds
  while (!client.available() && (timeout-- > 0)) {
    delay(100);
  }

  if (!client.available()) {
    Serial.println("No response, going back to sleep");
    return false;
  }
  Serial.println(F("disconnected"));
  return true;
}

bool sendTEWInfoMessage(String message) {
  const char* host = "makerspaceweather.thundersoftdev.info";
  //const char* streamId   = ubidotsStreamId;
  //const char* token = "2ee2cb26aa2cde85d8b76dd38615911c2c6de";
  Serial.print("Connecting to "); Serial.print(host);

  WiFiClient client;
  int retries = 5;
  while (!client.connect(host, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!client.connected()) {
    Serial.println("Failed to connect, going back to sleep");
    return false;
  }

  String url = "http://makerspaceweather.thundersoftdev.info/en/weather/add/2ee2cb26aa2cde85d8b76dd38615911c2c6de";
  
  Serial.println();
  Serial.println("UBIDOTS: ");
  Serial.println();

  //String value = String(float(ESP.getVcc() * VCC_ADJ));
  //String valueStr =   "{\"value\":" + value + '}';

  String urlLoad = String("POST ") + url +
                   " HTTP/1.1\r\n" +
                   //"X-Auth-Token: " + token + "\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n" +
                   "Content-Type: application/json\r\n" +
                   "Content-Length: "  + message.length() + "\r\n\r\n" +
                   message + "\r\n\r\n";

  Serial.print(urlLoad);
  client.print(urlLoad);

  int timeout = 5 * 10; // 5 seconds
  while (!client.available() && (timeout-- > 0)) {
    delay(100);
  }

  if (!client.available()) {
    Serial.println("No response, going back to sleep");
    return false;
  }
  Serial.println("Respond:");
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println(F("disconnected"));
  return true;
}
