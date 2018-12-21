boolean blinking = false;
const byte numChars = 32; 

//blinking without delay variables
unsigned long previousMillis = 0;
const long interval = 100;           // interval at which to blink (milliseconds)
int ledState = LOW; 
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


void setup() {
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  if (Serial.available() > 0) {
    delay(1000);
    Serial.println("Hello is this abc #1138");
    Serial.flush();
  }
}
