boolean blinking = false;
const byte numChars = 32; 

//blinking without delay variables
unsigned long previousMillis = 0;
const long interval = 100;           // interval at which to blink (milliseconds)
int ledState = LOW; 
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


void setup() {
  Serial.begin(96000);
  Serial.println("<Arduino is ready>");
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  if (Serial.available() > 0) {
//    int length = Serial.available();
//    char incomingChar;
//    char incomingChars[numChars];
//    
//    //This reads the incoming stream from the serial interface to a byte or a char
//    incomingChar = Serial.read(); // read the incoming byte
//    
       // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
//    Serial.println(incomingChar); //write to serial interface
    Serial.println("Hello World, welcome to arduino!");

    delay(20);
    
//    Serial.println("stringComplete");
    Serial.flush();
//       
//    if (incomingChar != -1) { // -1 means no data is available
//      blinking = !blinking;     
//    }
  }

  //Blinking without delay function, to make sure multiple processes can happen simultaneously
  if (blinking){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_BUILTIN, ledState);
    }
  }
  
  }
