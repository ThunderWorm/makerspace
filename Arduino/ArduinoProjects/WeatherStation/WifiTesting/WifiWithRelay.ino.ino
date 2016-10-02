#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <MemoryFree.h>
//#include <DateTime.h>

#define DEST_HOST   "thundersoftdev.org"
#define DEST_IP     "thundersoftdev.org"
#define TIMEOUT     5000 // mS
#define CONTINUE    false
#define HALT        true
#define POST_HEADERPARAM "a=a"

#define ECHO_COMMANDS // Un-comment to echo AT+ commands to serial monitor

//ALREADY CONNECTED

int relay = 10;
SoftwareSerial esp8266(2,3);

long previousMillis = 0;
String postData = POST_HEADERPARAM;
long postIndex = 0;
int *t=new int[50];
int *h=new int[50];

// Print error message and loop stop.
void errorHalt(String msg)
{
  Serial.println(msg);
  Serial.println("HALT");
  delay(1000);
}

// Read characters from WiFi module and echo to serial until keyword occurs or timeout.
boolean echoFind(String keyword)
{
  echoFind(keyword, TIMEOUT);  
}


void readESPData()
{
  if (esp8266.available()) 
  {
    while(esp8266.available())
    {
      char c = esp8266.read();
      Serial.write(c);  
    }  
  }  
}

boolean echoFind(String keyword, int timeout)
{
  byte current_char   = 0;
  byte keyword_length = keyword.length();
  
  // Fail if the target string has not been sent by deadline.
  long deadline = millis() + timeout;
  //delay(1000);
  while(millis() < deadline)
  {
    
    if (esp8266.available())
    {
      char ch = esp8266.read();
      Serial.write(ch);
      if (ch == keyword[current_char])
        if (++current_char == keyword_length)
        {
          Serial.println();
          return true;
        }
    }
  }
  //readESPData();
  return false;  // Timed out
}

// Read and echo all available module output.
// (Used when we're indifferent to "OK" vs. "no change" responses or to get around firmware bugs.)
void echoFlush()
{
  while(esp8266.available())
    Serial.write(esp8266.read());
}
  
// Echo module output until 3 newlines encountered.
// (Used when we're indifferent to "OK" vs. "no change" responses.)
void echoSkip()
{
  echoFind("\n");        // Search for nl at end of command echo
  echoFind("\n");        // Search for 2nd nl at end of response.
  echoFind("\n");        // Search for 3rd nl at end of blank line.
}

// Send a command to the module and wait for acknowledgement string
// (or flush module output if no ack specified).
// Echoes all data received to the serial monitor.
boolean echoCommand(String cmd, String ack, boolean halt_on_fail)
{
  return echoCommand(cmd, ack, halt_on_fail, TIMEOUT);
}

boolean echoCommand(String cmd, String ack, boolean halt_on_fail, int timeout)
{
  esp8266.println(cmd);
  if (timeout == 12000)
  {
    delay(3000);
    while (true)
      readESPData();
  }
  #ifdef ECHO_COMMANDS
    Serial.print("--"); Serial.println(cmd);
  #endif
  
  // If no ack response specified, skip all available module output.
  if (ack == "")
    echoSkip();
  else
    // Otherwise wait for ack.
    if (!echoFind(ack))          // timed out waiting for ack string 
      if (halt_on_fail)
        errorHalt(cmd+" failed");// Critical failure halt.
      else {
        delay(1000);
        return false;            // Let the caller handle it.
      }
  delay(1000);
  return true;                   // ack blank or ack found
}


// Connect to the specified wireless network.
boolean connectWiFi()
{
  String cmd = "AT+CWJAP=\"ThunderEvolverWormWrlssNetwork\",\"~!@ThunderEvolverWormWrlssNetworkN65U~!@\"";
  if (echoCommand(cmd, "WIFI GOT IP", CONTINUE)) // Join Access Point
  {
    Serial.println("Connected to WiFi.");
    return true;
  }
  else
  {
    Serial.println("Connection to WiFi failed.");
    return false;
  }
}



void setup() {
  // put your setup code here, to run once:
  MCUSR = 0;
  Serial.begin(9600);
  esp8266.begin(9600);

  esp8266.setTimeout(TIMEOUT);
  Serial.println("ESP8266 Weather Station");

  pinMode(relay, OUTPUT);
  
}

void startUpWifi() 
{
  delay(2000);
  Serial.println("Turning on WIFI");
  digitalWrite(relay, HIGH);
  delay(2000);
  echoCommand("AT+RST", "ready", HALT);    // Reset & test if the module is ready  
  Serial.println("Module is ready.");
  //echoCommand("AT+GMR", "OK", CONTINUE);   // Retrieves the firmware ID (version number) of the module. 
  //echoCommand("AT+CWMODE?","OK", CONTINUE);// Get module access mode. 
  //echoCommand("AT+CIPMODE=0","OK", CONTINUE);
  // echoCommand("AT+CWLAP", "OK", CONTINUE); // List available access points - DOESN't WORK FOR ME
  
  echoCommand("AT+CWMODE=1", "", HALT);    // Station mode
  echoCommand("AT+CIPMUX=1", "", HALT);    // Allow multiple connections (we'll only use the first).

   //connect to the wifi
  boolean connection_established = false;
  for(int i=0;i<5;i++)
  {
    if(connectWiFi())
    {
      connection_established = true;
      break;
    }
  }
  if (!connection_established) errorHalt("Connection failed");
  delay(5000);

  echoCommand("AT+CWSAP=?", "OK", CONTINUE); // Test connection
  echoCommand("AT+CIFSR", "", HALT);         // Echo IP address. (Firmware bug - should return "OK".)
  //echoCommand("AT+CIPMUX=0", "", HALT);      // Set single connection mode    
}

void loop() {
  // put your main code here, to run repeatedly:
  //char* name;
  //malloc(
  /*String a = "a=a";
  a.concat("dsf dsfdsf");
  a.concat("bbb");
  Serial.println(a);
  */
  /*if (esp8266.available()) 
  {
    while(esp8266.available())
    {
      char c = esp8266.read();
      Serial.write(c);  
    }  
  }

  if (Serial.available())
  {
    delay(1000);

      String command = "";
      while (Serial.available()) 
      {
        command += (char)Serial.read();  
      }
      esp8266.println(command);
  }*/
  Serial.println("millis: " + String(millis()));

  postData.concat("&");
  postData.concat(postIndex);
  postData.concat("[t]=");
  postData.concat(rand() % 100);
  postData.concat("&");
  postData.concat(postIndex);
  postData.concat("[h]=");
  postData.concat(rand()%100);
  postData.concat("&");
  postData.concat(postIndex);
  postData.concat("[p]=");
  postData.concat(rand()%1000);
  //postData = postData + "&" + String(postIndex) + "[t]=" + String(rand() % 100) + "&" + String(postIndex) + "[h]=" + String(rand()%10000);
  Serial.println(postData);
  delay(4000);
  ++postIndex;
  long passedMillis = millis() - previousMillis;
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  if (passedMillis >= 30000)
  {
    startUpWifi();
    /*loadWebpage("0[t]=" + String(rand() % 100) + "&0[h]=" + String(rand()%10000)
      + "&1[t]=" + String(rand() % 100) + "&1[h]=" + String(rand()%10000)
    );*/
    loadWebpage(postData);
    Serial.println("Turning off WIFI");
    delay(2000);
    digitalWrite(relay, LOW);
    delay(1000); 
    previousMillis = millis();
    postIndex = 0;
    postData = POST_HEADERPARAM;

    if(previousMillis >= 300000) // Reset arduino every 10 hours
    {
      ResetArduino();
    } 
  }
}


void ResetArduino()
{
  wdt_enable(WDTO_15MS); // turn on the WatchDog and don't stroke it.
  for(;;) { 
    // do nothing and wait for the eventual...
  }  
}

void loadWebpage(String &message) {
  // Establish TCP connection
  String cmd = "AT+CIPSTART=4,\"TCP\",\""; 
  cmd.concat(DEST_IP);
  cmd.concat("\",80");
  //cmd += DEST_IP; cmd += "\",80";
  if (!echoCommand(cmd, "OK", CONTINUE)) return;
  delay(2000);
  //readESPData();
  // Get connection status 
  if (!echoCommand("AT+CIPSTATUS", "OK", CONTINUE)) return;
  //readESPData();
  // Build HTTP request.
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  Serial.println(message);
  cmd = "POST /terem2/test.php HTTP/1.0\r\nHost: thundersoftdev.org\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
  cmd.concat(message.length());
  cmd.concat("\r\n\r\n");
  cmd.concat(message);
  cmd.concat("\r\n");
  //cmd = "POST /terem2/test.php HTTP/1.0\r\nHost: thundersoftdev.org\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: "+ String(message.length()) +"\r\n\r\n" + message + "\r\n";
  //Serial.println(cmd.length() + " cmd - " + cmd);
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  // Ready the module to receive raw data
  if (!echoCommand("AT+CIPSEND=4," + String(cmd.length()), ">", CONTINUE))
  {
    echoCommand("AT+CIPCLOSE=4", "", CONTINUE); // 5 - close all connections
    Serial.println("Connection timeout.");
    return;
  }
  delay(1000);
  esp8266.print(cmd);
  // Send the raw HTTP request
  //echoCommand(cmd, "OK", CONTINUE, 12000);  // GET
  
  // Loop forever echoing data received from destination server.
  //delay(5000);
  long deadline = millis() + TIMEOUT;
  while(millis() < deadline)
      readESPData();
      
  //errorHalt("ONCE ONLY");  
}
