// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "WIFI_SSID_NAME";
const char* password = "WIFI_PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String redState = "off";
String greenState = "off";
String blueState = "off";

// Assign output variables to GPIO pins
const int blueOutput= 2;
const int greenOutput= 0;
const int redOutput= 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(blueOutput, OUTPUT);
  pinMode(greenOutput, OUTPUT);
  pinMode(redOutput, OUTPUT);
  // Set outputs to LOW
  digitalWrite(blueOutput, LOW);
  digitalWrite(greenOutput, LOW);
  digitalWrite(redOutput, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /red/on") >= 0) {              // red on 
              Serial.println("GPIO red on");
              redState = "on";
              digitalWrite(redOutput, HIGH);
            } else if (header.indexOf("GET /red/off") >= 0) {      // red off
              Serial.println("GPIO red off");
              redState = "off";
              digitalWrite(redOutput, LOW);
            } else if (header.indexOf("GET /blue/on") >= 0) {      // blue on
              Serial.println("GPIO blue on");
              blueState = "on";
              digitalWrite(blueOutput, HIGH);
            } else if (header.indexOf("GET /blue/off") >= 0) {     // blue off
              Serial.println("GPIO blue off");
              blueState = "off";
              digitalWrite(blueOutput, LOW);
            } else if (header.indexOf("GET /green/on") >= 0) {     // green on 
              Serial.println("GPIO green on");
              greenState = "on";
              digitalWrite(greenOutput, HIGH);
            } else if (header.indexOf("GET /green/off") >= 0) {    // green off
              Serial.println("GPIO green off");
              greenState = "off";
              digitalWrite(greenOutput, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // If the any state is off, it displays the ON button and vice versa
            if (redState=="off") {
              client.println("<p><a href=\"/red/on\"><button class=\"button\">RED ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/red/off\"><button class=\"button button2\">RED OFF</button></a></p>");
            } 
            if (greenState=="off") {
              client.println("<p><a href=\"/green/on\"><button class=\"button\">GREEN ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/green/off\"><button class=\"button button2\">GREEN OFF</button></a></p>");
            }
            if (blueState=="off") {
              client.println("<p><a href=\"/blue/on\"><button class=\"button\">BLUE ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/blue/off\"><button class=\"button button2\">BLUE OFF</button></a></p>");
            } 
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
