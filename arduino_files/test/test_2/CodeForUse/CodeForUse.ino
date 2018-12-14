const int SEQlength = 1999;     //2000 elements, we see it as 1000 * 2: 1000 step's
int SEQ[SEQlength];
int StepNr = 0;
int ReadNr = 0;
String readString;
boolean exportArray = false;

void setup() {
  Serial.begin(9600); // start serial
  Serial.println("<Arduino is ready>");
}

void loop() {
  delay(20);
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
    if (readString.equals("-32000\n")) {
      Serial.println("hello");
      exportArray = true;
    }
    SEQ[ReadNr] = readString.toInt();
    Serial.print("Print in SEQ: ");
    Serial.println(SEQ[ReadNr]);
    ReadNr = ReadNr + 1;
    readString = "";
  }
}


void sendArray() {
  if (exportArray) {
    Serial.println("Send Array");
    for (int i = 0; i < 3; i++) {
      Serial.print(SEQ[i]);
      Serial.print(',');
    }
    exportArray=false;
  }
}
