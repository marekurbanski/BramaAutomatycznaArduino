/*
 * 
 * =================== MODUL WIFI DO BRAMY ======================
 * 
 * Moduł ESP8266 - ten z arduino do samodzielnego monatzu
 * Płytka: NodeMCU 0.9 (ESP-12 Modula)
 * CPU Frequency: 80 MHz
 * Flash size: 4M (3m SPIFFS)
 * Upload speed: 115200 
 * ...
 * Programator: AVRISP mkii 
 * 
 */

#include  <ESP8266WiFi.h>  

const char* ssid     = "U-Dom"; // Tu wpisz nazwę swojego wifi
const char* password = "....."; // Tu wpisz hasło do swojego wifi

int pinDoor = 16; // D0
int pinOpen = 5; // D1
int pinClose = 4; // D2
int pinState = 12; // D6

WiFiServer server(80);
 
void setup() {
Serial.begin(115200);
delay(10);
 
pinMode(pinOpen, OUTPUT);
digitalWrite(pinOpen, LOW);

pinMode(pinClose, OUTPUT);
digitalWrite(pinClose, LOW);

pinMode(pinDoor, OUTPUT);
digitalWrite(pinClose, LOW);
 
// Connect to WiFi network
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.setSleepMode(WIFI_NONE_SLEEP);
WiFi.mode(WIFI_STA);
//WiFi.mode(WIFI_AP_STA);
WiFi.setPhyMode(WIFI_PHY_MODE_11G);
//WiFi.setPhyMode(WIFI_PHY_MODE_11N);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {

  if (WiFi.status() == WL_IDLE_STATUS) { // when not connected to a network, but powered on 
    Serial.println("WL_IDLE_STATUS");
  }
  else if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WL_CONNECTED ");
    Serial.println(WiFi.localIP());
  }
  else if (WiFi.status() == WL_NO_SSID_AVAIL) {
    Serial.println("WL_NO_SSID_AVAIL");
  }
  else if (WiFi.status() == WL_CONNECT_FAILED) {
    Serial.println("WL_CONNECT_FAILED");
  }
  else if (WiFi.status() == WL_CONNECTION_LOST) {
    Serial.println("WL_CONNECTION_LOST");
  }
  else if (WiFi.status() == WL_DISCONNECTED) {
    Serial.println("WL_DISCONNECTED");
  }
  else {
    Serial.print("unknown status: ");
    Serial.println(WiFi.status());
  }
delay(2000);
WiFi.printDiag(Serial);
Serial.print(".");
Serial.print(WiFi.localIP());
Serial.setDebugOutput(true);
}
 
Serial.println("");
Serial.println("WiFi connected");
 
// Start the server
server.begin();
Serial.println("Server started");
 
// Print the IP address
Serial.print("Use this URL to connect: ");
Serial.print("http://");
Serial.print(WiFi.localIP());
Serial.println("/"); 
}

 
void loop() {
// Check if a client has connected
WiFiClient client = server.available();
if (!client) {
  return;
}
 
// Wait until the client sends some data
Serial.println("new client");
int timewate = 0;
while(!client.available()){
  delay(1);
  timewate = timewate +1;
  if(timewate>1800)
  {
    Serial.println(">>> Client Timeout !");
    client.stop();
    return;
  }
}
 
// Read the first line of the request
String request = client.readStringUntil('\r');
Serial.println(request);
client.flush();
String wynik="Dzialam :)";

// Match the request 
if (request.indexOf("/=OPEN") != -1)  {
  wynik = "Otwieram";
  Serial.println("Sygnal: Otwieram");
  digitalWrite(pinOpen, HIGH);
  delay(1000);
  Serial.println("Sygnal: OFF");
  digitalWrite(pinOpen, LOW);
}
 
if (request.indexOf("/=CLOSE") != -1)  {
  wynik = "Zamykam";
  Serial.println("Sygnal: Zamykam");
  digitalWrite(pinClose, HIGH);
  delay(1000);
  Serial.println("Sygnal: OFF");
  digitalWrite(pinClose, LOW);
}

if (request.indexOf("/=DOOR") != -1)  {
  wynik = "Otwieram jedno skrzydlo";
  Serial.println("Sygnal: Otwieram jedno skrzydlo");
  digitalWrite(pinDoor, HIGH);
  delay(1000);
  Serial.println("Sygnal: OFF");
  digitalWrite(pinDoor, LOW);
}

if (request.indexOf("/=STATE") != -1)  {
  int get_status = analogRead(pinState);
  if(get_status > 500) 
    get_status = 1;
  else
    get_status = 0;
  wynik = "STATE:"+String(get_status)+":";
  Serial.println(wynik);
  digitalWrite(pinState, HIGH);
  delay(1000);
  Serial.println("Sygnal: OFF");
  digitalWrite(pinState, LOW);
}

// Return the response
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println(""); //  do not forget this one
client.println(wynik);
client.println("");
Serial.println("Client disonnected");
Serial.println("");
}
