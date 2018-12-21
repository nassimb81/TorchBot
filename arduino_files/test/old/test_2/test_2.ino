const int SEQlength = 1999;     //2000 elements, we see it as 1000 * 2: 1000 step's
int SEQ[SEQlength];
int ReadNr = 0;
String readString;

void setup() {
  Serial.begin(115200); // start serial
  Serial.println("<Arduino is ready>");
}

void loop() {
  while (Serial.available()) {
    char inChar = Serial.read();
    int  inInt = inChar - '0';
    String temp = String(inChar);
    readString += temp;

    if (inChar == '\n') {
      //Check if Stop Code has been send           
      if (readString.equals("-32000\n") > 0 ) {
        for (int i = 0; i < ReadNr; i++) {          
          Serial.print(SEQ[i]);
          Serial.print(',');
        }
        Serial.print("-32000");
      }
      
      SEQ[ReadNr] = readString.toInt();
      ReadNr = ReadNr + 1;
      readString = "";
    }
  }
}
