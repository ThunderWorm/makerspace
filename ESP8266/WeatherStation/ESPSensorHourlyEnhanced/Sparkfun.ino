
// my Chip IDs to give it a cleartype name
#define WA1 14117293
#define WA2 12612352


bool sendSparkfun(byte sparkfunType, float temp, float hum, float voltage) {

  // Use Sparkfun testing stream
  const char* host = "data.sparkfun.com";
  const char* streamId   = "LQGm8OvmjYf2LdXVNROy";
  const char* privateKey = "A1w9APB9zXIrvpP2BRxw";

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

  String url = "/input/";
  url += streamId;
  url += "?private_key=";
  url += privateKey;
  url += "&humidity=";
  url += hum;
  url += "&temp=";
  url += temp; 
  url += "&voltage=";
  url += voltage; 

  Serial.println();
  Serial.print("sparkfun: ");


  Serial.print("Request URL: "); Serial.println(url);

  client.print(String("GET ") + url +
               " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

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
