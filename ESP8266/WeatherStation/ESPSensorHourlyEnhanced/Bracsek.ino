
// my Chip IDs to give it a cleartype name
#define WA1 14117293
#define WA2 12612352


bool sendBracsek(byte sparkfunType, float temp, float hum, float battery) {

  // Use Sparkfun testing stream
  const char* host = "weather.bracsek.eu";
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

  String url = "/";
  
  Serial.println();
  Serial.print("bracsek: ");


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

bool sendBracsekMessage(byte sparkfunType, String message) {

  // Use Sparkfun testing stream
  const char* host = "weather.bracsek.eu";
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

  String url = "/";
  
  Serial.println();
  Serial.print("bracsek: ");


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
