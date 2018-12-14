const int SEQlength = 1999;     //2000 elements, we see it as 1000 * 2: 1000 step's
int SEQ[SEQlength];
int StepNr = 0;
int ReadNr = 0;
String readString;
boolean exportArray = false;

void setup() {
  Serial.begin(115200); // start serial
  Serial.println("<Arduino is ready>");
}

void loop() {
  while (Serial.available()) {
    receiveArray();
    sendArray();
  }
}

void receiveArray() {
  char inChar = Serial.read();
  int  inInt = inChar - '0';
  String temp = String(inChar);
  readString += temp;

  if (inChar == '\n') {
    Serial.println("In Char");
    SEQ[ReadNr] = readString.toInt();
    Serial.print("Print in SEQ: ");
    Serial.println(SEQ[ReadNr]);
    ReadNr = ReadNr + 1;
    if (readString.equals("-32000\n")) {
      Serial.println("hello");
      exportArray = true;
    }
    readString = "";
  }
}


void sendArray() {
  if (exportArray) {
    Serial.print("Send_Array");
    for (int i = 0; i < SEQlength; i++) {
      Serial.print(SEQ[i]);
      if ( SEQ[i] == -32000) {
        break;
      }
      Serial.print(',');
    }
    exportArray = false;
  }
}
